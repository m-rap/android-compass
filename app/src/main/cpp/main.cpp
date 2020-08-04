//
// Created by Rian Prakoso on 7/28/20.
//

#include "Container.h"
#include <string.h>

Drawable* compass = NULL;

void constructDraw(Container* container1) {
    Drawable& canvas = *(Drawable*)&container1->canvas;
    float s1px = 1 / ((Canvas*)&canvas)->small;

    compass = canvas.addchild();
    canvas.setColor(255, 255, 255, 255);
    Drawable* circ1 = compass->circlefill(0, 0, 0.7);
    canvas.setColor(0, 0, 0, 255);
    Drawable* circ2 = compass->circlefill(0, 0, 0.7 - 10 * s1px);
    canvas.setColor(255, 255, 255, 255);

    float needleRadius = 0.7 - 35 * s1px;

    for (int i = 0; i < 36; i++) {
        double degree = i * 10;
        double radians = degree * M_PI / 180;
        float x = (float)(cos(radians) * needleRadius), y = (float)(sin(radians) * needleRadius);
        Drawable* needle = compass->rectfill(x, y, 70 * s1px, 10 * s1px);
        needle->rotation = degree;
    }

    canvas.setColor(255, 0, 0, 255);
    Drawable* hand = compass->addchild();
    float handRadius = 0.7 - 80 * s1px;
    hand->addVtx(0, handRadius);
    hand->addVtx(50 * s1px, 0);
    hand->addVtx(-50 * s1px, 0);
    hand->addVtx(0, -handRadius);
    int tr1[] = {0, 2, 1};
    int tr2[] = {1, 2, 3};
    hand->addTriangle(tr1);
    hand->addTriangle(tr2);
    hand->mode = GL_TRIANGLES;

    canvas.end();
}

const int navg = 60;
float azAvgs[navg];
int azIdx = 0;
int azCount = 0;

void enableSensor(Container* container1) {
    if (container1->compassSensor == NULL)
        return;
    memset(azAvgs, 0, sizeof(float) * navg);
    azIdx = 0;
    azCount = 0;
    ASensorEventQueue_enableSensor(container1->sensorEventQueue, container1->compassSensor);
    // 60 events per second.
    ASensorEventQueue_setEventRate(container1->sensorEventQueue, container1->compassSensor, (1000L/60)*1000);
}

void disableSensor(Container* container1) {
    if (container1->compassSensor == NULL)
        return;
    ASensorEventQueue_disableSensor(container1->sensorEventQueue, container1->compassSensor);
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
                container1->initEgl();
                constructDraw(container1);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            container1->deinitEgl();
            break;
        case APP_CMD_GAINED_FOCUS:
            container1->animating = true;
            enableSensor(container1);
            break;
        case APP_CMD_LOST_FOCUS:
            container1->animating = false;
            disableSensor(container1);
            break;
        default:
            break;
    }
    LOGI("engine_handle_cmd %d done", cmd);
}

#include <dlfcn.h>
ASensorManager* aquireASensorManagerInstance(android_app* app) {
    if(!app)
        return NULL;

    typedef ASensorManager* (*GetInstanceForPackage)(const char *name);
    void* androidHandle = dlopen("libandroid.so", RTLD_NOW);
    GetInstanceForPackage gfpFunc = (GetInstanceForPackage)dlsym(androidHandle, "ASensorManager_getInstanceForPackage");

    if (gfpFunc) {
        JNIEnv* env = NULL;
        app->activity->vm->AttachCurrentThread(&env, NULL);

        jclass activityClz = env->GetObjectClass(app->activity->clazz);
        jmethodID midGetPackageName = env->GetMethodID(activityClz, "getPackageName", "()Ljava/lang/String;");
        jstring packageName = (jstring)env->CallObjectMethod(app->activity->clazz, midGetPackageName);

        const char *nativePackageName = env->GetStringUTFChars(packageName, NULL);
        ASensorManager* mgr = gfpFunc(nativePackageName);
        env->ReleaseStringUTFChars(packageName, nativePackageName);
        app->activity->vm->DetachCurrentThread();

        if (mgr) {
            dlclose(androidHandle);
            return mgr;
        }
    }

    typedef ASensorManager *(*GetInstance)();
    GetInstance getInstanceFunc = (GetInstance)dlsym(androidHandle, "ASensorManager_getInstance");

    // by all means at this point, ASensorManager_getInstance should be available
    dlclose(androidHandle);

    return getInstanceFunc();
}

void android_main(struct android_app* state) {
    LOGI("android_main");

    int secDiff = 0, prevSecDiff = 0;
    timeval start;
    gettimeofday(&start, NULL);

    Container container;
    container.running = false;
    container.animating = false;
    container.compassSensor = NULL;

    container.app = state;
    container.sensorManager = aquireASensorManagerInstance(state);
    container.compassSensor = ASensorManager_getDefaultSensor(container.sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
    container.sensorEventQueue = ASensorManager_createEventQueue(container.sensorManager, state->looper, LOOPER_ID_USER,
            NULL, NULL);

    //enableSensor(&container);

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
                if (container.compassSensor != nullptr) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(container.sensorEventQueue,
                                                       &event, 1) > 0) {
                        azAvgs[azIdx] = atan2(event.magnetic.y, event.magnetic.x) * 180 / M_PI;
                        azIdx = (azIdx + 1) % navg;
                        if (azCount < navg) {
                            azCount++;
                        }
                        if (azCount > 0 && azCount < navg) {
                            float total = 0;
                            for (int i = 0; i < azCount; i++) {
                                total += azAvgs[i];
                            }
                            compass->rotation = total / azCount;
                        } else {
                            float total = 0;
                            for (int i = 0; i < navg; i++) {
                                total += azAvgs[i];
                            }
                            compass->rotation = total / navg;
                        }
                        if (secDiff != prevSecDiff) {
                            LOGI("azimuth %f xyz %f %f %f v %f %f %f",
                                    event.magnetic.azimuth,
                                    event.magnetic.x,
                                    event.magnetic.y,
                                    event.magnetic.z,
                                    event.magnetic.v[0],
                                    event.magnetic.v[1],
                                    event.magnetic.v[2]);
                        }
                    }
                }
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
            if (compass != NULL) {
            }
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