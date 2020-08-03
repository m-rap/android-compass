//
// Created by Rian Prakoso on 7/28/20.
//

#include "Container.h"

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
                container1->initEgl();

                //Canvas& c = container1->canvas;
                Drawable& c = *(Drawable*)&container1->canvas;
                c.setColor(255, 255, 255, 255);
                //c.rectfill(0.1, 0.1, -1, 1);
                //c.rectfill(25, 25, 100, 100);
                //c.circlefill(0, 0, 1);
                c.setColor(0, 255, 0, 255);
                c.circlefill(0, 0, 0.2);
                //c.circlestroke(0, 0, 0.2);
                c.setColor(255, 0, 0, 255);
                c.lineWidth = 3.0f;
                //Drawable* rect = c.rectfill(0, 0, 0.2, 0.2);
                Drawable* rect = c.rectstroke(0, 0, 0.2, 0.2);
                rect->rotation = 45;
                c.end();
            }
            break;
        case APP_CMD_TERM_WINDOW:
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

    Container container;
    container.running = false;
    container.animating = false;

    container.app = state;
    state->userData = &container;
    state->onAppCmd = engine_handle_cmd;

    int fps = 0;

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
                container.deinitEgl();
                break;
            }
        }

        if (state->destroyRequested != 0) {
            break;
        }

        if (secDiff != prevSecDiff) {
            LOGI("fps %d", fps);
            fps = 0;
        }

        if (container.running && container.animating) {

            container.draw();
        }

        fps++;

        prevSecDiff = secDiff;
    }

    LOGI("main end");
}