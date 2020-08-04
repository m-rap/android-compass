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
float magXAvgs[navg];
float magYAvgs[navg];
float magZAvgs[navg];
float aXAvgs[navg];
float aYAvgs[navg];
float aZAvgs[navg];
float smoothMag[3], smoothAccel[3];
int filterIdx = 0;
int filterCount = 0;

void enableSensor(Container* container1) {
    if (container1->compassSensor == NULL)
        return;
    memset(magXAvgs, 0, sizeof(float) * navg);
    filterIdx = 0;
    filterCount = 0;
    ASensorEventQueue_enableSensor(container1->sensorEventQueue, container1->compassSensor);
    ASensorEventQueue_enableSensor(container1->sensorEventQueue, container1->accelSensor);
    // 60 events per second.
    ASensorEventQueue_setEventRate(container1->sensorEventQueue, container1->compassSensor, (1000L/60)*1000);
    ASensorEventQueue_setEventRate(container1->sensorEventQueue, container1->accelSensor, (1000L/60)*1000);
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
    container.accelSensor = ASensorManager_getDefaultSensor(container.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    container.sensorEventQueue = ASensorManager_createEventQueue(container.sensorManager, state->looper, LOOPER_ID_USER,
            NULL, NULL);

    //enableSensor(&container);


    JNIEnv* env = NULL;
    state->activity->vm->AttachCurrentThread(&env, NULL);
    jclass sensorMgrClz = env->FindClass("android/hardware/SensorManager");
    jmethodID metIdGetRotationMatrix = env->GetStaticMethodID(sensorMgrClz, "getRotationMatrix", "([F[F[F[F)Z");
    jmethodID metIdGetOrientation = env->GetStaticMethodID(sensorMgrClz, "getOrientation", "([F[F)[F");

    jfloatArray jrotMatrix = env->NewFloatArray(9);
    jfloatArray jinclMatrix = env->NewFloatArray(9);
    jfloatArray jsmoothAccel = env->NewFloatArray(3);
    jfloatArray jsmoothMag = env->NewFloatArray(3);
    jfloatArray jorientation = env->NewFloatArray(3);


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
                        //magXAvgs[filterIdx] = atan2(event.magnetic.y, event.magnetic.x) * 180 / M_PI;

                        magXAvgs[filterIdx] = event.magnetic.x;
                        magYAvgs[filterIdx] = event.magnetic.y;
                        magZAvgs[filterIdx] = event.magnetic.z;

                        aXAvgs[filterIdx] = event.acceleration.x;
                        aYAvgs[filterIdx] = event.acceleration.y;
                        aZAvgs[filterIdx] = event.acceleration.z;

                        filterIdx = (filterIdx + 1) % navg;
                        if (filterCount < navg) {
                            filterCount++;
                        }
                        float div;
                        if (filterCount > 0 && filterCount < navg) {
                            div = filterCount;
                        } else {
                            div = navg;
                        }
                        smoothMag[0] = 0;
                        smoothMag[1] = 0;
                        smoothMag[2] = 0;
                        for (int i = 0; i < div; i++) {
                            smoothMag[0] += magXAvgs[i];
                            smoothMag[1] += magYAvgs[i];
                            smoothMag[2] += magZAvgs[i];
                        }
                        smoothMag[0] /= div;
                        smoothMag[1] /= div;
                        smoothMag[2] /= div;

                        smoothAccel[0] = 0;
                        smoothAccel[1] = 0;
                        smoothAccel[2] = 0;
                        for (int i = 0; i < div; i++) {
                            smoothAccel[0] += aXAvgs[i];
                            smoothAccel[1] += aYAvgs[i];
                            smoothAccel[2] += aZAvgs[i];
                        }
                        smoothAccel[0] /= div;
                        smoothAccel[1] /= div;
                        smoothAccel[2] /= div;

                        env->SetFloatArrayRegion(jsmoothAccel, 0, 3, smoothAccel);
                        env->SetFloatArrayRegion(jsmoothMag, 0, 3, smoothMag);
                        jboolean jsuccess = env->CallStaticBooleanMethod(sensorMgrClz, metIdGetRotationMatrix, jrotMatrix, jinclMatrix, jsmoothAccel, jsmoothMag);
                        float orientation[3];
                        if (jsuccess) {
                            env->CallStaticObjectMethod(sensorMgrClz, metIdGetOrientation, jrotMatrix, jorientation);
                            env->GetFloatArrayRegion(jorientation, 0, 3, orientation);
                            compass->rotation = orientation[0];
                        }

                        if (secDiff != prevSecDiff) {
                            //LOGI("azimuth %f xyz %f %f %f v %f %f %f",
                            //        event.magnetic.azimuth,
                            //        event.magnetic.x,
                            //        event.magnetic.y,
                            //        event.magnetic.z,
                            //        event.magnetic.v[0],
                            //        event.magnetic.v[1],
                            //        event.magnetic.v[2]);
                            LOGI("%d orientation %f %f %f accel %f %f %f mag %f %f %f",
                                    jsuccess, orientation[0], orientation[1], orientation[2],
                                    smoothAccel[0], smoothAccel[1], smoothAccel[2],
                                    smoothMag[0], smoothMag[1], smoothMag[2]);
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

    env->DeleteLocalRef(jrotMatrix);
    env->DeleteLocalRef(jinclMatrix);
    env->DeleteLocalRef(jsmoothAccel);
    env->DeleteLocalRef(jsmoothMag);
    env->DeleteLocalRef(jorientation);

    state->activity->vm->DetachCurrentThread();


    LOGI("main end");
}