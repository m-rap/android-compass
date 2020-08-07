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
    GLenum mode;
    Vertex2* vtxBuffer = NULL;
    GLushort* idxBuffer = NULL;
    GLushort vtxBuffSize;
    GLushort idxBuffSize;
    unsigned char r, g, b, a;
    float x, y;
    float rotation;
    float scale;
    GLuint vbo;
    GLuint ibo;
    bool initialized = false;
    //float lineWidth;

    Drawable* canvas;
    Drawable* parent;
    Drawable* children[1024];
    int childrenCount;

    void init(Drawable* parent);
    void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void deinit();
    void addVtx(float x, float y);
    void addTriangle(int* idx);

    void _rectvtx(float x, float y, float width, float height);
    void _circlevtx(float x, float y, float r);

    void _rectfill(float x, float y, float width, float height);
    //void _rectstroke(float x1, float y1, float width, float height);
    void _circlefill(float x1, float y1, float r);
    //void _circlestroke(float x, float y, float r);

    Drawable* addchild();
    Drawable* rectfill(float x1, float y1, float width, float height);
    Drawable* circlefill(float x1, float y1, float r);
    //Drawable* rectstroke(float x1, float y1, float width, float height);
    //Drawable* circlestroke(float x1, float y1, float r);

    void end();

    void draw();
};

struct Container;

struct Canvas {
    Drawable clzparent;

    Container* container;

    float width, height;
    float small, big;
    float alalay = 0;

    void resize(int w, int h);

    void init();
    void deinit();

    void draw();
};


#endif //COMPASS_CANVAS_H
