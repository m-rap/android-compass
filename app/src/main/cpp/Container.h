//
// Created by Rian Prakoso on 7/30/20.
//

#ifndef COMPASS_CONTAINER_H
#define COMPASS_CONTAINER_H

#include <EGL/egl.h>

#ifdef __ANDROID_API__

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#else

#define LOGI(...) (printf(__VA_ARGS__))

#endif
//#include <cairo.h>
//#include <cairo-gl.h>

#include "Canvas.h"

struct Container {
    EGLint width, height;
    EGLSurface surface;
    EGLContext context;
    EGLDisplay display;
//    cairo_device_t* cdt;
//    cairo_surface_t* crSurface;
//    cairo_t* cr;
    android_app* app;
    bool running;
    bool animating;

    Canvas canvas;

    JNIEnv* env = NULL;
    jclass activityClz;
    jmethodID metIdShowUi;

    ASensorManager* sensorManager;
    const ASensor* compassSensor;
    const ASensor* accelSensor;
    ASensorEventQueue *sensorEventQueue;

    int initEgl();
    int deinitEgl();

    void draw();
};


//struct Container {
//    EGLint width, height;
//    EGLSurface surface;
//    EGLContext context;
//    EGLDisplay display;
//    cairo_device_t* cdt;
//    cairo_surface_t* crSurface;
//    cairo_t* cr;
//    android_app* app;
//    bool running;
//    bool animating;
//};
//
//
//int initEgl(Container& container1);
//int deinitEgl(Container& container1);


#endif //COMPASS_CONTAINER_H
