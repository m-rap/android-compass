//
// Created by m-rap on 02/08/20.
//

#ifndef COMPASS_CANVAS_H
#define COMPASS_CANVAS_H

#ifdef __ANDROID_API__

//#include <GLES2/gl2.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#else

#include <GL/gl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stddef.h>

#define LOGI(...) (printf(__VA_ARGS__))

#endif

struct Vertex2 {
    float x, y, z;
    unsigned char r, g, b, a;
};

struct Drawable {
    Vertex2 vtxBuffer[200];
    GLushort idxBuffer[200];
    GLushort vtxBuffSize;
    GLushort idxBuffSize;
    unsigned char r, g, b, a;
    float x, y;
    float rotation;
    GLuint vbo;
    GLuint ibo;
    bool initialized = false;

    Drawable* children[1024];
    int childrenCount;

    void init();
    void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void deinit();
    void addVtx(float x, float y);
    void addTriangle(int* idx);

    void rect(float x, float y, float width, float height);
    void circle(float x, float y, float r);
    void end();

    void draw();
};

struct Canvas {
    Drawable parent;

    float width, height;

    void init();
    void deinit();

    void draw();
};


#endif //COMPASS_CANVAS_H
