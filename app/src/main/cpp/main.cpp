//
// Created by Rian Prakoso on 7/28/20.
//

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <cairo.h>
#include <cairo-gl.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

struct Container {
    EGLint width, height;
    EGLSurface surface;
    EGLContext context;
    EGLDisplay display;
    cairo_device_t* cdt;
    cairo_surface_t* crSurface;
    cairo_t* cr;
    android_app* app;
    bool running;
    bool animating;
};

Container container;

int initEgl(Container& container1) {
    LOGI("initEgl");

    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint w, h, format;
    EGLint numConfigs = 128;
    EGLConfig config = nullptr;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, nullptr, nullptr);

    eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

    LOGI("numConfigs: %d", numConfigs);

    EGLConfig supportedConfigs[128];
    eglChooseConfig(display, attribs, supportedConfigs, 128, &numConfigs);

    LOGI("numConfigs: %d", numConfigs);

    int i = 0;
    for (; i < numConfigs; i++) {
        EGLConfig cfg = supportedConfigs[i];
        EGLint r, g, b, d;
        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r)   &&
            eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b)  &&
            eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
            r == 8 && g == 8 && b == 8 && d == 0 ) {

            config = supportedConfigs[i];
            break;
        }
    }
    if (i == numConfigs) {
        config = supportedConfigs[0];
    }

    if (config == nullptr) {
        LOGI("Unable to initialize EGLConfig");
        return -1;
    }

    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    surface = eglCreateWindowSurface(display, config, container1.app->window, nullptr);
    context = eglCreateContext(display, config, nullptr, nullptr);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGI("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);

    container1.width = w;
    container1.height = h;
    container1.display = display;
    container1.surface = surface;
    container1.context = context;

    container1.cdt = cairo_egl_device_create(container1.display, container1.context);
    container1.crSurface = cairo_gl_surface_create_for_egl (container1.cdt, container1.surface, container1.width, container1.height);
    container1.cr = cairo_create(container1.crSurface);

    container1.running = true;

    LOGI("initEgl done");

    return 0;
}

int deinitEgl(Container& container1) {
    if (!container1.running) {
        return 0;
    }
    LOGI("deinitEgl");
    container1.running = false;
    container1.animating = false;

    cairo_destroy(container1.cr);
    cairo_surface_destroy(container1.crSurface);

    eglMakeCurrent(container1.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (container1.context != EGL_NO_CONTEXT) {
        eglDestroyContext(container1.display, container1.context);
    }
    if (container1.surface != EGL_NO_SURFACE) {
        eglDestroySurface(container1.display, container1.surface);
    }
    eglTerminate(container1.display);

    container1.display = EGL_NO_DISPLAY;
    container1.context = EGL_NO_CONTEXT;
    container1.surface = EGL_NO_SURFACE;

    return 0;
}

void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    LOGI("engine_handle_cmd %d", cmd);
    if (app->userData == NULL) {
        return;
    }
    Container* container1 = (Container*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            break;
        case APP_CMD_INIT_WINDOW:
            if (container1->app->window != nullptr) {
                initEgl(*container1);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            deinitEgl(*container1);
            break;
        case APP_CMD_GAINED_FOCUS:
            container1->animating = true;
            break;
        case APP_CMD_LOST_FOCUS:
            container1->animating = false;
            break;
        default:
            break;
    }
    LOGI("engine_handle_cmd %d done", cmd);
}

void android_main(struct android_app* state) {
    LOGI("android_main");

    int secDiff = 0, prevSecDiff = 0;
    timeval start;
    gettimeofday(&start, NULL);

    container.running = false;
    container.animating = false;

    container.app = state;
    state->userData = &container;
    state->onAppCmd = engine_handle_cmd;

    LOGI("to loop");
    while (true) {
        int ident;
        int events;
        struct android_poll_source* source;


        timeval now;
        gettimeofday(&now, NULL);
        secDiff = now.tv_sec - start.tv_sec;

        while ((ident=ALooper_pollAll(0, nullptr, &events,
                                      (void**)&source)) >= 0) {

            if (source != nullptr) {
                source->process(state, source);
            }

            if (ident == LOOPER_ID_USER) {

            }

            if (secDiff != prevSecDiff) {
            }

            if (state->destroyRequested != 0) {
                LOGI("destroyRequested");
                deinitEgl(container);
                return;
            }
        }

        if (container.running && container.animating) {
            cairo_set_line_width(container.cr, 1);
            cairo_set_source_rgb(container.cr, 1.0, 1.0, 0.8);
            cairo_rectangle(container.cr, 25, 25, 5, 5);
            cairo_stroke(container.cr);
            cairo_surface_flush(container.crSurface);
            eglSwapBuffers(container.display, container.surface);
        }

        prevSecDiff = secDiff;
    }
}