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

struct Shape {
    Vertex2 vtxBuffer[200];
    GLushort idxBuffer[200];
    GLushort vtxBuffSize;
    GLushort idxBuffSize;
    unsigned char r, g, b, a;
    GLuint vbo;
    GLuint ibo;
    bool initialized = false;

    ~Shape() { deinit(); }

    void init(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void deinit();
    void addVtx(float x, float y);
    void addTriangle(int* idx);
};

struct Canvas {
    int width, height;
    uint8_t r, g, b, a;

    Shape shapes[100];
    int shapeCount;

    void initGl();
    void deinitGl();
    void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void rect(float x, float y, float width, float height);
    void circle(float x, float y, float r);
    void end();
    void draw();
};


#endif //COMPASS_CANVAS_H
