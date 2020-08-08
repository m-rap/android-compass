//
// Created by Rian Prakoso on 7/28/20.
//

#include "Container.h"
#include "MatHelper.h"
#include <string.h>
#include <dlfcn.h>

float alpha = 0.05;

struct Filter {
    float smooth[3];

    void add(float* v) {
        for (int i = 0; i < 3; i++) {
            smooth[i] = smooth[i] + alpha * (v[i] - smooth[i]);
        }
    }
};

Drawable* compass = NULL;
Filter accelFilter, magFilter;
bool sensorEnabled = false;
Container container;

void constructDraw(Container* container1) {
    Drawable& canvas = *(Drawable*)&container1->canvas;
    container.s1px = 1 / ((Canvas*)&canvas)->small;
    float s1px = container.s1px;
    //float linewidth = 10 * s1px;
    float linewidth = 0.01;
    LOGI("s1px %f linewidth %f", s1px, linewidth);

    canvas.setColor(50, 50, 50, 255);
    canvas.circlefill(0, 0, 0.83);

    compass = canvas.addchild();
    canvas.setColor(255, 255, 255, 255);
    Drawable* circ1 = compass->circlefill(0, 0, 0.7);
    canvas.setColor(0, 0, 0, 255);
    Drawable* circ2 = compass->circlefill(0, 0, 0.7 - linewidth);
    canvas.setColor(255, 255, 255, 255);

    float needleLength = 0.05;
    //float needleRadius = 0.7 - 35 * s1px;
    float needleRadius = 0.7 - needleLength / 2;

    for (int degree = 0; degree < 360; degree += 5) {
        double radians = degree * M_PI / 180;
        float r;
        float nl;
        //if (degree % 90 == 0) {
        if (degree % 45 == 0) {
            nl = needleLength + 0.04;
            r = needleRadius - 0.02;
        } else if (degree % 5 == 0 && degree % 10 != 0) {
            nl = needleLength - 0.02;
            r = needleRadius + 0.01;
        } else {
            nl = needleLength;
            r = needleRadius;
        }

        float x = (float)(cos(radians) * r), y = (float)(sin(radians) * r);
        Drawable* needle = compass->rectfill(x, y, nl, linewidth);

        if (degree == 90) {
            canvas.setColor(10, 200, 10, 255);
        }
        float xOuter = (float)(cos(radians) * (r + 0.11)), yOuter = (float)(sin(radians) * (r + 0.11));
        Drawable* needleOuter = canvas.rectfill(xOuter, yOuter, nl, linewidth);
        if (degree == 90) {
            canvas.setColor(255, 255, 255, 255);
        }

        needle->rotation = degree;
        needleOuter->rotation = degree;
    }

    canvas.setColor(255, 0, 0, 255);
    Drawable* hand = compass->addchild();
    float handHeight = 0.1;
    float handWidth = 0.08;
    float handRadius = 0.7 - 0.25;

    hand->vtxBuffer = (Vertex2*)malloc(sizeof(Vertex2) * 3);
    hand->idxBuffer = (GLushort *)malloc(sizeof(GLushort) * 3);
    hand->addVtx(0, handRadius);
    hand->addVtx(handWidth / 2, handRadius - handHeight);
    hand->addVtx(-handWidth / 2, handRadius - handHeight);
    //hand->addVtx(0, -handRadius);
    int tr1[] = {0, 2, 1};
    //int tr2[] = {1, 2, 3};
    hand->addTriangle(tr1);
    //hand->addTriangle(tr2);
    hand->mode = GL_TRIANGLES;

    canvas.end();
}

void enableSensor(Container* container1) {
    if (sensorEnabled || container1->compassSensor == NULL)
        return;
    sensorEnabled = true;

    memset(&accelFilter, 0, sizeof(Filter));
    memset(&magFilter, 0, sizeof(Filter));

    ASensorEventQueue_enableSensor(container1->sensorEventQueue, container1->compassSensor);
    ASensorEventQueue_enableSensor(container1->sensorEventQueue, container1->accelSensor);
    // 60 events per second.
    ASensorEventQueue_setEventRate(container1->sensorEventQueue, container1->compassSensor, (1000L/60)*1000);
    ASensorEventQueue_setEventRate(container1->sensorEventQueue, container1->accelSensor, (1000L/60)*1000);
}

void disableSensor(Container* container1) {
    if (!sensorEnabled || container1->compassSensor == NULL)
        return;
    sensorEnabled = false;
    ASensorEventQueue_disableSensor(container1->sensorEventQueue, container1->compassSensor);
}

void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    LOGI("engine_handle_cmd %d", cmd);
    if (app->userData == NULL) {
        return;
    }
    Container* container1 = (Container*)app->userData;
    JNIEnv* env = container1->env;
    jmethodID metIdShowUi = container1->metIdShowUi;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            break;
        case APP_CMD_INIT_WINDOW:
            if (container1->app->window != nullptr) {
                container1->initEgl();
                if (env != NULL) {
                    env->CallVoidMethod(app->activity->clazz, metIdShowUi);
                }
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

ASensorManager* aquireASensorManagerInstance(android_app* app, JNIEnv* env) {
    if(!app)
        return NULL;

    typedef ASensorManager* (*GetInstanceForPackage)(const char *name);
    void* androidHandle = dlopen("libandroid.so", RTLD_NOW);
    GetInstanceForPackage gfpFunc = (GetInstanceForPackage)dlsym(androidHandle, "ASensorManager_getInstanceForPackage");

    if (gfpFunc) {

        jclass activityClz = env->GetObjectClass(app->activity->clazz);
        jmethodID midGetPackageName = env->GetMethodID(activityClz, "getPackageName", "()Ljava/lang/String;");
        jstring packageName = (jstring)env->CallObjectMethod(app->activity->clazz, midGetPackageName);

        const char *nativePackageName = env->GetStringUTFChars(packageName, NULL);
        ASensorManager* mgr = gfpFunc(nativePackageName);
        env->ReleaseStringUTFChars(packageName, nativePackageName);

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

    JNIEnv* env = NULL;
    state->activity->vm->AttachCurrentThread(&env, NULL);

    jclass activityClz = env->GetObjectClass(state->activity->clazz);
    jmethodID metIdShowUi =  env->GetMethodID(activityClz, "showUi", "()V");
    jmethodID metIdSetDegree =  env->GetMethodID(activityClz, "setDegree", "(FF)V");
    jmethodID metIdGetLayoutHeight =  env->GetMethodID(activityClz, "getLayoutDimension", "([I)V");
    jmethodID metIdGetDrawnDegree =  env->GetMethodID(activityClz, "getDrawnDegree", "()F");

    container.running = false;
    container.animating = false;
    container.compassSensor = NULL;
    container.env = env;
    container.metIdShowUi = metIdShowUi;
    container.s1px = 0;

    container.app = state;
    container.sensorManager = aquireASensorManagerInstance(state, env);
    container.compassSensor = ASensorManager_getDefaultSensor(container.sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
    container.accelSensor = ASensorManager_getDefaultSensor(container.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    container.sensorEventQueue = ASensorManager_createEventQueue(container.sensorManager, state->looper, LOOPER_ID_USER,
            NULL, NULL);

    enableSensor(&container);

    state->userData = &container;
    state->onAppCmd = engine_handle_cmd;

    jintArray jdimensions = env->NewIntArray(4);

    int fps = 0;

    LOGI("to loop");

    while (true) {
        int ident;
        int events;
        struct android_poll_source* source;


        timeval now;
        gettimeofday(&now, NULL);
        secDiff = now.tv_sec - start.tv_sec;

        while ((ident=ALooper_pollAll(0, NULL, &events,
                                      (void**)&source)) >= 0) {

            if (source != NULL) {
                source->process(state, source);
            }

            if (ident == LOOPER_ID_USER) {
                if (container.compassSensor != NULL && container.accelSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(container.sensorEventQueue,
                                                       &event, 1) > 0) {

                        if (event.type == ASENSOR_TYPE_ACCELEROMETER) {
                            accelFilter.add(event.acceleration.v);
                        }

                        if (event.type == ASENSOR_TYPE_MAGNETIC_FIELD) {
                            magFilter.add(event.magnetic.v);
                        }

                        float orientation[] = {0, 0, 0};
                        getOrientation2(accelFilter.smooth, magFilter.smooth, orientation);

                        if (compass != NULL) {
                            float ori[3];
                            for (int i = 0; i < 3; i++) {
                                ori[i] = orientation[i] * 180 / M_PI;
                                while (ori[i] < 0) {
                                    ori[i] += 360;
                                }
                                while (ori[i] >= 360) {
                                    ori[i] -= 360;
                                }
                            }
                            env->CallVoidMethod(state->activity->clazz, metIdSetDegree, roundf(ori[0] * 2) / 2, ori[0]);
                        }

                        if (secDiff != prevSecDiff) {
                            //LOGI("orientation %f %f %f accel %f %f %f mag %f %f %f",
                            //        orientation[0], orientation[1], orientation[2],
                            //        accelFilter.smooth[0], accelFilter.smooth[1], accelFilter.smooth[2],
                            //        magFilter.smooth[0], magFilter.smooth[1], magFilter.smooth[2]);
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
            //LOGI("fps %d", fps);
            if (compass != NULL) {
            }
            fps = 0;
        }

        if (container.running && container.animating) {
            //if (jdimensions == 0) {
            //if (container.canvas.height == container.height) {
            env->CallVoidMethod(state->activity->clazz, metIdGetLayoutHeight, jdimensions);
            int* dimensions = env->GetIntArrayElements(jdimensions, 0);
            //LOGI("layout %d %d %d %d", dimensions[0], dimensions[1], dimensions[2], dimensions[3]);
            //if (dimensions[3] != 0 && container.s1px != 0) {
                //float barH = container.height - jdimensions;
                //container.canvas.resize(container.width, dimensions[3]);
                container.canvas.resize(dimensions[0], dimensions[1], dimensions[2], dimensions[3]);
            //}
            //}
            compass->rotation = env->CallFloatMethod(state->activity->clazz, metIdGetDrawnDegree);
            container.draw();
        }

        fps++;

        prevSecDiff = secDiff;
    }

    env->DeleteLocalRef(jdimensions);

    state->activity->vm->DetachCurrentThread();


    LOGI("main end");
}