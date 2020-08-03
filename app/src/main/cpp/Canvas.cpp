//
// Created by m-rap on 02/08/20.
//

#include "Canvas.h"
#include <math.h>


Drawable drawablePool[1024];
int takenDrawableCount = 0;

void _rect(Drawable& s, float x, float y, float width, float height) {
    s.x = x;
    s.y = y;

    //s.addVtx(x, y);
    //s.addVtx(x + width, y);
    //s.addVtx(x + width, y + height);
    //s.addVtx(x, y + height);

    s.addVtx(-width / 2, -height / 2);
    s.addVtx(width / 2, -height / 2);
    s.addVtx(width / 2, height / 2);
    s.addVtx(-width / 2, height / 2);

    int idx1[] = {s.vtxBuffSize - (GLushort)4, s.vtxBuffSize - (GLushort)3, s.vtxBuffSize - (GLushort)2};
    s.addTriangle(idx1);

    int idx2[] = {s.vtxBuffSize - (GLushort)4, s.vtxBuffSize - (GLushort)2, s.vtxBuffSize - (GLushort)1};
    s.addTriangle(idx2);
}

void _circle(Drawable& s, float x, float y, float radius) {
    GLushort nSegments = 65;

    s.x = x;
    s.y = y;

    //s.addVtx(x, y);
    s.addVtx(0, 0);
    for (int i = 0; i < nSegments; i++) {
        double degree = (double)(i * 360) / nSegments;
        //double radians = degree * 3.14159265358979323846 / 180;
        double radians = degree * M_PI / 180;
        //s.addVtx((float)(x + cos(radians) * radius), (float)(y + sin(radians) * radius));
        s.addVtx((float)(cos(radians) * radius), (float)(sin(radians) * radius));
    }

    GLushort idxStart = s.vtxBuffSize - nSegments - (GLushort)1;
    for (GLushort i = 0; i < nSegments - 1; i++) {
        int idx[] = {idxStart, idxStart + i + (GLushort)1, idxStart + i + (GLushort)2};
        s.addTriangle(idx);
    }
    int idx[] = {idxStart, idxStart + nSegments, idxStart + (GLushort)1};
    s.addTriangle(idx);
}

void Drawable::deinit() {
    if (initialized) {
        initialized = false;
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);

        for (int i = 0; i < childrenCount; i++) {
            children[i]->deinit();
        }
    }
}

void Drawable::init(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1) {
    initialized = true;

    r = r1; g = g1; b = b1; a = a1;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    vtxBuffSize = 0;
    idxBuffSize = 0;
}

void Drawable::addVtx(float x, float y) {
    vtxBuffer[vtxBuffSize++] = {x, y, 0, r, g, b, a};
}

void Drawable::addTriangle(int* idx) {
    if (idxBuffSize > 1024 - 3) {
        return;
    }
    idxBuffer[idxBuffSize++] = idx[0];
    idxBuffer[idxBuffSize++] = idx[1];
    idxBuffer[idxBuffSize++] = idx[2];
}

void Drawable::rect(float x, float y, float width, float height) {
    if (takenDrawableCount >= 1024) {
        return;
    }
    Drawable* d = children[childrenCount++] = &drawablePool[takenDrawableCount++];
    d->init(r, g, b, a);
    _rect(*d, x, y, width, height);
}

void Drawable::circle(float x, float y, float radius) {
    if (takenDrawableCount >= 1024) {
        return;
    }
    Drawable* d = children[childrenCount++] = &drawablePool[takenDrawableCount++];
    d->init(r, g, b, a);
    _circle(*d, x, y, radius);
}

void Drawable::end() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vtxBuffSize * sizeof(Vertex2), vtxBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxBuffSize * sizeof(GLushort), idxBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    for (int i = 0; i < childrenCount; i++) {
        children[i]->end();
    }
}

void Drawable::draw() {
    glMatrixMode(GL_MODELVIEW_MATRIX);
    glPushMatrix();

    glTranslatef(x, y, 0);
    glRotatef(rotation, 0, 0, 1);

    LOGI("x, y, rot %f %f, %f", x, y, rotation);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(Vertex2), (void*)offsetof(Vertex2, x));
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex2), (void*)offsetof(Vertex2, r));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glDrawElements(GL_TRIANGLES, idxBuffSize, GL_UNSIGNED_SHORT, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    for (int i = 0; i < childrenCount; i++) {
        children[i]->draw();
    }

    glMatrixMode(GL_MODELVIEW_MATRIX);
    glPopMatrix();
}

void Canvas::init() {
    drawableCount = 0;

    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    //glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);
    float small = width, big = height;
    if (height < width) {
        small = height;
        big = width;
    }
    glViewport(0, big/4, small, small);
}

void Canvas::deinit() {
    for (int i = 0; i < drawableCount; i++) {
        drawables[i].deinit();
    }
}

void Canvas::setColor(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1) {
    r = r1; g = g1; b = b1; a = a1;
}

void Canvas::rect(float x, float y, float width, float height) {
    Drawable& s = drawables[drawableCount++];
    s.init(r, g, b, a);
    _rect(s, x, y, width, height);
}

void Canvas::circle(float x, float y, float radius) {
    Drawable& s = drawables[drawableCount];
    drawableCount++;
    s.init(r, g, b, a);

    _circle(s, x, y, radius);
}

void Canvas::end() {
    for (int i = 0; i < drawableCount; i++) {
        drawables[i].end();
    }
}

void Canvas::draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);

    for (int i = 0; i < drawableCount; i++) {
        drawables[i].draw();
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}