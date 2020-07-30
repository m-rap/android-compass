//
// Created by Rian Prakoso on 7/28/20.
//

#include "Container.h"

Container container;

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
                //initEgl(*container1);
                container1->initEgl();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            //deinitEgl(*container1);
            container1->deinitEgl();
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
                //deinitEgl(container);
                container.deinitEgl();
                return;
            }
        }

        if (container.running && container.animating) {

            cairo_set_source_rgb(container.cr, 0.0, 0.0, 0.0);
            cairo_paint(container.cr);

            cairo_set_line_width(container.cr, 1);
            cairo_set_source_rgb(container.cr, 1.0, 1.0, 1.0);
            cairo_rectangle(container.cr, 25, 25, 100, 100);
            cairo_stroke(container.cr);
            //cairo_fill(container.cr);
            cairo_surface_flush(container.crSurface);
            cairo_gl_surface_swapbuffers(container.crSurface);
            //eglSwapBuffers(container.display, container.surface);
        }

        prevSecDiff = secDiff;
    }
}