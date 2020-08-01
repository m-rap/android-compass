//
// Created by Rian Prakoso on 7/30/20.
//

#ifndef COMPASS_CONTAINER_H
#define COMPASS_CONTAINER_H

#include <EGL/egl.h>
//#include <GLES2/gl2.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
//#include <cairo.h>
//#include <cairo-gl.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

struct Vertex2 {
    float x, y, z;
    unsigned char r, g, b, a;
};

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

    GLuint vbo;
    GLuint ibo;
    Vertex2 vtxBuffer[1024];
    GLushort idxBuffer[1024];
    GLushort vtxBuffSize;
    GLushort idxBuffSize;
    unsigned char r, g, b, a;

    int initEgl();
    int deinitEgl();

    void initGl();
    void deinitGl();
    void addVtx(float x, float y);
    void addTriangle(int* idx);
    void rect(float x, float y, float width, float height);
    void circle(float x, float y, float r);
    void end();

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
