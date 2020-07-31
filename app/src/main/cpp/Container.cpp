//
// Created by Rian Prakoso on 7/30/20.
//

#include "Container.h"

int Container::initEgl() {
    LOGI("initEgl");

    const EGLint attribs[] = {
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RED_SIZE,        8,
            EGL_GREEN_SIZE,      8,
            EGL_BLUE_SIZE,       8,

            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
//            EGL_ALPHA_SIZE,   EGL_DONT_CARE,
//            EGL_DEPTH_SIZE,   16,
//            EGL_SAMPLES,      4,

            EGL_NONE
    };
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

    EGLint w, h, format;
    EGLint numConfigs = 128;
    EGLConfig config = NULL;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, NULL, NULL);

    eglChooseConfig(display, attribs, NULL, 0, &numConfigs);

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

    if (config == NULL) {
        LOGI("Unable to initialize EGLConfig");
        return -1;
    }

    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    surface = eglCreateWindowSurface(display, config, app->window, NULL);
    context = eglCreateContext(display, config, NULL, NULL);
    //context = eglCreateContext(display, config, NULL, contextAttribs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGI("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    LOGI("OpenGL Info: %s", glGetString(GL_VENDOR));
    LOGI("OpenGL Info: %s", glGetString(GL_RENDERER));
    LOGI("OpenGL Info: %s", glGetString(GL_VERSION));
    LOGI("OpenGL Info: %s", glGetString(GL_EXTENSIONS));

    width = w;
    height = h;
    this->display = display;
    this->surface = surface;
    this->context = context;

    LOGI("w %d h %d", width, height);

    //cdt = cairo_egl_device_create(display, context);
    //crSurface = cairo_gl_surface_create_for_egl(cdt, surface, width, height);
    //
    //LOGI("cdt err: %s\nsurface: err %s",
    //     cairo_status_to_string(cairo_device_status(cdt)),
    //     cairo_status_to_string(cairo_surface_status(crSurface)));
    //
    //cr = cairo_create(crSurface);
    //LOGI("cdt %08x surface %08x cr %08x", cdt, crSurface, cr);
    //LOGI("cr w %d h %d", cairo_gl_surface_get_width(crSurface), cairo_gl_surface_get_height(crSurface));

    initGl();

    running = true;

    LOGI("initEgl done");

    return 0;
}

int Container::deinitEgl() {
    if (!running) {
        return 0;
    }
    LOGI("deinitEgl");
    running = false;
    animating = false;

    deinitGl();

    //cairo_destroy(cr);
    //cairo_surface_destroy(crSurface);
    //cairo_device_destroy(cdt);

    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context != EGL_NO_CONTEXT) {
        eglDestroyContext(display, context);
    }
    if (surface != EGL_NO_SURFACE) {
        eglDestroySurface(display, surface);
    }
    eglTerminate(display);

    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;

    return 0;
}

void Container::initGl() {
    glGenBuffers(1, &vbo);
}

void Container::deinitGl() {
    glDeleteBuffers(1, &vbo);
}

void Container::addVtx(float x, float y) {
    vtxBuffer[buffSize++] = x;
    vtxBuffer[buffSize++] = y;
    vtxBuffer[buffSize++] = 0;
    vtxBuffer[buffSize++] = r;
    vtxBuffer[buffSize++] = g;
    vtxBuffer[buffSize++] = b;
    vtxBuffer[buffSize++] = a;
}

void Container::rect(float x, float y, float width, float height) {
    addVtx(x, y);
    addVtx(x + width, y);
    addVtx(x + width, y + height);

    addVtx(x, y);
    addVtx(x + width, y + height);
    addVtx(x, y + height);
}

void Container::end() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffSize * sizeof(float), vtxBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Container::draw() {
    //cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    //cairo_paint(cr);
    //
    //cairo_set_line_width(cr, 1);
    //cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    //cairo_rectangle(cr, 25, 25, 100, 100);
    //cairo_stroke(cr);
    ////cairo_fill(cr);
    //cairo_surface_flush(crSurface);
    //cairo_gl_surface_swapbuffers(crSurface);

    buffSize = 0;
    r = 255; g = 255; b = 255; a = 1;
    //rect(25, 25, 100, 100);
    rect(0.1, 0.1, 1, 1);
    end();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(float) * 7, 0);
    glColorPointer(4, GL_FLOAT, sizeof(float) * 7, (void*)(3 * sizeof(float)));
    glDrawArrays(GL_TRIANGLES, 0, buffSize / 7);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    eglSwapBuffers(display, surface);
}




//int initEgl(Container& container1) {
//    LOGI("initEgl");
//
//    const EGLint attribs[] = {
//            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
//            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
//            EGL_RED_SIZE,        8,
//            EGL_GREEN_SIZE,      8,
//            EGL_BLUE_SIZE,       8,
//
//            EGL_ALPHA_SIZE,   EGL_DONT_CARE,
//            EGL_DEPTH_SIZE,   16,
//            EGL_SAMPLES,      4,
//
//            EGL_NONE
//    };
//    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
//
//    EGLint w, h, format;
//    EGLint numConfigs = 128;
//    EGLConfig config = NULL;
//    EGLSurface surface;
//    EGLContext context;
//
//    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
//
//    eglInitialize(display, NULL, NULL);
//
//    eglChooseConfig(display, attribs, NULL, 0, &numConfigs);
//
//    LOGI("numConfigs: %d", numConfigs);
//
//    EGLConfig supportedConfigs[128];
//    eglChooseConfig(display, attribs, supportedConfigs, 128, &numConfigs);
//
//    LOGI("numConfigs: %d", numConfigs);
//
//    int i = 0;
//    for (; i < numConfigs; i++) {
//        EGLConfig cfg = supportedConfigs[i];
//        EGLint r, g, b, d;
//        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r)   &&
//            eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
//            eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b)  &&
//            eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
//            r == 8 && g == 8 && b == 8 && d == 0 ) {
//
//            config = supportedConfigs[i];
//            break;
//        }
//    }
//    if (i == numConfigs) {
//        config = supportedConfigs[0];
//    }
//
//    if (config == NULL) {
//        LOGI("Unable to initialize EGLConfig");
//        return -1;
//    }
//
//    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
//    surface = eglCreateWindowSurface(display, config, container1.app->window, NULL);
//    //context = eglCreateContext(display, config, NULL, NULL);
//    context = eglCreateContext(display, config, NULL, contextAttribs);
//
//    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
//        LOGI("Unable to eglMakeCurrent");
//        return -1;
//    }
//
//    eglQuerySurface(display, surface, EGL_WIDTH, &w);
//    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
//
//    LOGI("OpenGL Info: %s", glGetString(GL_VENDOR));
//    LOGI("OpenGL Info: %s", glGetString(GL_RENDERER));
//    LOGI("OpenGL Info: %s", glGetString(GL_VERSION));
//    LOGI("OpenGL Info: %s", glGetString(GL_EXTENSIONS));
//
//    container1.width = w;
//    container1.height = h;
//    container1.display = display;
//    container1.surface = surface;
//    container1.context = context;
//
//    LOGI("w %d h %d", container1.width, container1.height);
//
//    container1.cdt = cairo_egl_device_create(container1.display, container1.context);
//    container1.crSurface = cairo_gl_surface_create_for_egl (container1.cdt, container1.surface, container1.width, container1.height);
//
//    LOGI("cdt err: %s\nsurface: err %s",
//         cairo_status_to_string(cairo_device_status(container1.cdt)),
//         cairo_status_to_string(cairo_surface_status(container1.crSurface)));
//
//    container1.cr = cairo_create(container1.crSurface);
//    LOGI("cdt %08x surface %08x cr %08x", container1.cdt, container1.crSurface, container1.cr);
//    LOGI("cr w %d h %d", cairo_gl_surface_get_width(container1.crSurface), cairo_gl_surface_get_height(container1.crSurface));
//
//    container1.running = true;
//
//    LOGI("initEgl done");
//
//    return 0;
//}
//
//int deinitEgl(Container& container1) {
//    if (!container1.running) {
//        return 0;
//    }
//    LOGI("deinitEgl");
//    container1.running = false;
//    container1.animating = false;
//
//    cairo_destroy(container1.cr);
//    cairo_surface_destroy(container1.crSurface);
//    cairo_device_destroy(container1.cdt);
//
//    eglMakeCurrent(container1.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
//    if (container1.context != EGL_NO_CONTEXT) {
//        eglDestroyContext(container1.display, container1.context);
//    }
//    if (container1.surface != EGL_NO_SURFACE) {
//        eglDestroySurface(container1.display, container1.surface);
//    }
//    eglTerminate(container1.display);
//
//    container1.display = EGL_NO_DISPLAY;
//    container1.context = EGL_NO_CONTEXT;
//    container1.surface = EGL_NO_SURFACE;
//
//    return 0;
//}