//
// Created by m-rap on 02/08/20.
//

#include "Canvas.h"
#include <math.h>

void Shape::deinit() {
    if (initialized) {
        initialized = false;
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
    }
}

void Shape::init(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1) {
    initialized = true;

    r = r1; g = g1; b = b1; a = a1;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    vtxBuffSize = 0;
    idxBuffSize = 0;
}

void Shape::addVtx(float x, float y) {
    vtxBuffer[vtxBuffSize++] = {x, y, 0, r, g, b, a};
}

void Shape::addTriangle(int* idx) {
    if (idxBuffSize > 1024 - 3) {
        return;
    }
    idxBuffer[idxBuffSize++] = idx[0];
    idxBuffer[idxBuffSize++] = idx[1];
    idxBuffer[idxBuffSize++] = idx[2];
}

void Canvas::initGl() {
    shapeCount = 0;

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

void Canvas::deinitGl() {
    for (int i = 0; i < shapeCount; i++) {
        shapes[i].deinit();
    }
}

void Canvas::setColor(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1) {
    r = r1; g = g1; b = b1; a = a1;
}

void Canvas::rect(float x, float y, float width, float height) {
    Shape& s = shapes[shapeCount];
    shapeCount++;
    s.init(r, g, b, a);
    s.addVtx(x, y);
    s.addVtx(x + width, y);
    s.addVtx(x + width, y + height);
    s.addVtx(x, y + height);

    int idx1[] = {s.vtxBuffSize - (GLushort)4, s.vtxBuffSize - (GLushort)3, s.vtxBuffSize - (GLushort)2};
    s.addTriangle(idx1);

    int idx2[] = {s.vtxBuffSize - (GLushort)4, s.vtxBuffSize - (GLushort)2, s.vtxBuffSize - (GLushort)1};
    s.addTriangle(idx2);
}

void Canvas::circle(float x, float y, float radius) {
    GLushort nSegments = 65;

    Shape& s = shapes[shapeCount];
    shapeCount++;
    s.init(r, g, b, a);

    s.addVtx(x, y);
    for (int i = 0; i < nSegments; i++) {
        double degree = (double)(i * 360) / nSegments;
        double radians = degree * M_PI / 180;
        s.addVtx((float)(x + cos(radians) * radius), (float)(y + sin(radians) * radius));
    }

    GLushort idxStart = s.vtxBuffSize - nSegments - (GLushort)1;
    for (GLushort i = 0; i < nSegments - 1; i++) {
        int idx[] = {idxStart, idxStart + i + (GLushort)1, idxStart + i + (GLushort)2};
        s.addTriangle(idx);
    }
    int idx[] = {idxStart, idxStart + nSegments, idxStart + (GLushort)1};
    s.addTriangle(idx);
}

void Canvas::end() {
    for (int i = 0; i < shapeCount; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, shapes[i].vbo);
        glBufferData(GL_ARRAY_BUFFER, shapes[i].vtxBuffSize * sizeof(Vertex2), shapes[i].vtxBuffer, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapes[i].ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[i].idxBuffSize * sizeof(GLushort), shapes[i].idxBuffer, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void Canvas::draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);

    for (int i = 0; i < shapeCount; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, shapes[i].vbo);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(Vertex2), (void*)offsetof(Vertex2, x));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex2), (void*)offsetof(Vertex2, r));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapes[i].ibo);
        glDrawElements(GL_TRIANGLES, shapes[i].idxBuffSize, GL_UNSIGNED_SHORT, 0);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}