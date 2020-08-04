//
// Created by m-rap on 02/08/20.
//

#include "Canvas.h"
#include <math.h>


Drawable drawablePool[1024];
int takenDrawableCount = 0;
GLushort nSegments = 65;


void Drawable::deinit() {
    LOGI("drawable deinit %08x %d %d", this, initialized, childrenCount);
    if (initialized) {
        initialized = false;
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);

        for (int i = 0; i < childrenCount; i++) {
            children[i]->deinit();
        }
        childrenCount = 0;
    }
}

void Drawable::init(Drawable* parent1) {
    LOGI("drawable init");
    initialized = true;

    childrenCount = 0;

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    vtxBuffSize = 0;
    idxBuffSize = 0;
    x = 0;
    y = 0;
    rotation = 0;
    scale = 1;
    lineWidth = 1;
    parent = parent1;

    Drawable* tmp = this;
    while (tmp != NULL) {
        if (tmp->parent == NULL) {
            canvas = tmp;
            break;
        }
        tmp = tmp->parent;
    }
    setColor(canvas->r, canvas->g, canvas->b, canvas->a);
    lineWidth = canvas->lineWidth;
}

void Drawable::setColor(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1) {
    r = r1; g = g1; b = b1; a = a1;
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

void Drawable::_rectvtx(float x1, float y1, float width, float height) {
    x = x1;
    y = y1;

    addVtx(-width / 2, -height / 2);
    addVtx(width / 2, -height / 2);
    addVtx(width / 2, height / 2);
    addVtx(-width / 2, height / 2);
}

void Drawable::_circlevtx(float x1, float y1, float radius) {
    x = x1;
    y = y1;

    addVtx(0, 0);
    for (int i = 0; i < nSegments; i++) {
        double degree = (double)(i * 360) / nSegments;
        double radians = degree * M_PI / 180;
        addVtx((float)(cos(radians) * radius), (float)(sin(radians) * radius));
    }
}

void Drawable::_rectfill(float x1, float y1, float width, float height) {
    _rectvtx(x1, y1, width, height);

    mode = GL_TRIANGLES;

    int idx1[] = {vtxBuffSize - (GLushort)4, vtxBuffSize - (GLushort)3, vtxBuffSize - (GLushort)2};
    addTriangle(idx1);

    int idx2[] = {vtxBuffSize - (GLushort)4, vtxBuffSize - (GLushort)2, vtxBuffSize - (GLushort)1};
    addTriangle(idx2);
}

void Drawable::_circlefill(float x1, float y1, float radius) {
    _circlevtx(x1, y1, radius);

    mode = GL_TRIANGLE_FAN;

    GLushort idxStart = vtxBuffSize - nSegments - 1;
    for (GLushort i = 0; i < nSegments - 1; i++) {
        idxBuffer[idxBuffSize++] = idxStart + i;
    }
    idxBuffer[idxBuffSize++] = idxStart + 1;
}

void Drawable::_rectstroke(float x1, float y1, float width, float height) {
    _rectvtx(x1, y1, width, height);

    mode = GL_LINE_LOOP;

    idxBuffer[idxBuffSize++] = vtxBuffSize - 4;
    idxBuffer[idxBuffSize++] = vtxBuffSize - 3;
    idxBuffer[idxBuffSize++] = vtxBuffSize - 2;
    idxBuffer[idxBuffSize++] = vtxBuffSize - 1;
}

void Drawable::_circlestroke(float x, float y, float radius) {
    _circlevtx(x, y, radius);

    mode = GL_LINE_LOOP;

    for (GLushort i = 0; i < nSegments; i++) {
        idxBuffer[idxBuffSize++] = i;
    }
}

//void _rectvtx(Drawable& d, float x1, float y1, float width, float height) {
//    d.x = x1;
//    d.y = y1;
//
//    d.addVtx(-width / 2, -height / 2);
//    d.addVtx(width / 2, -height / 2);
//    d.addVtx(width / 2, height / 2);
//    d.addVtx(-width / 2, height / 2);
//}
//
//void _circlevtx(Drawable& d, float x1, float y1, float radius) {
//    d.x = x1;
//    d.y = y1;
//
//    d.addVtx(0, 0);
//    for (int i = 0; i < nSegments; i++) {
//        double degree = (double)(i * 360) / nSegments;
//        double radians = degree * M_PI / 180;
//        d.addVtx((float)(cos(radians) * radius), (float)(sin(radians) * radius));
//    }
//}
//
//void _rectfill(Drawable& d, float x1, float y1, float width, float height) {
//    _rectvtx(d, x1, y1, width, height);
//
//    d.mode = GL_TRIANGLES;
//
//    int idx1[] = {d.vtxBuffSize - (GLushort)4, d.vtxBuffSize - (GLushort)3, d.vtxBuffSize - (GLushort)2};
//    d.addTriangle(idx1);
//
//    int idx2[] = {d.vtxBuffSize - (GLushort)4, d.vtxBuffSize - (GLushort)2, d.vtxBuffSize - (GLushort)1};
//    d.addTriangle(idx2);
//}
//
//void _circlefill(Drawable& d, float x1, float y1, float radius) {
//    _circlevtx(d, x1, y1, radius);
//
//    d.mode = GL_TRIANGLE_FAN;
//
//    GLushort idxStart = d.vtxBuffSize - nSegments - 1;
//    for (GLushort i = 0; i < nSegments - 1; i++) {
//        d.idxBuffer[d.idxBuffSize++] = idxStart + i;
//    }
//    d.idxBuffer[d.idxBuffSize++] = idxStart + 1;
//}
//
//void _rectstroke(Drawable& d, float x1, float y1, float width, float height) {
//    _rectvtx(d, x1, y1, width, height);
//
//    d.mode = GL_LINE_LOOP;
//
//    d.idxBuffer[d.idxBuffSize++] = d.vtxBuffSize - 4;
//    d.idxBuffer[d.idxBuffSize++] = d.vtxBuffSize - 3;
//    d.idxBuffer[d.idxBuffSize++] = d.vtxBuffSize - 2;
//    d.idxBuffer[d.idxBuffSize++] = d.vtxBuffSize - 1;
//}
//
//void _circlestroke(Drawable& d, float x, float y, float radius) {
//    _circlevtx(d, x, y, radius);
//
//    d.mode = GL_LINE_LOOP;
//
//    for (GLushort i = 0; i < nSegments; i++) {
//        d.idxBuffer[d.idxBuffSize++] = i;
//    }
//}

//Drawable* addchild(Drawable& p) {
//    if (takenDrawableCount >= 1024) {
//        return NULL;
//    }
//    Drawable* d = p.children[p.childrenCount++] = &drawablePool[takenDrawableCount++];
//    d->init(&p);
//    return d;
//}

//Drawable* Drawable::rectfill(float x1, float y1, float width, float height) {
//    Drawable* d = addchild(*this);
//    _rectfill(*d, x1, y1, width, height);
//    return d;
//}
//
//Drawable* Drawable::circlefill(float x1, float y1, float radius) {
//    Drawable* d = addchild(*this);
//    _circlefill(*d, x1, y1, radius);
//    return d;
//}
//
//Drawable* Drawable::rectstroke(float x1, float y1, float width, float height) {
//    Drawable* d = addchild(*this);
//    _rectstroke(*d, x1, y1, width, height);
//    return d;
//}
//
//Drawable* Drawable::circlestroke(float x1, float y1, float radius) {
//    Drawable* d = addchild(*this);
//    _circlestroke(*d, x1, y1, radius);
//    return d;
//}

Drawable* Drawable::addchild() {
    if (takenDrawableCount >= 1024) {
        return NULL;
    }
    Drawable* d = children[childrenCount++] = &drawablePool[takenDrawableCount++];
    d->init(this);
    return d;
}

Drawable* Drawable::rectfill(float x1, float y1, float width, float height) {
    Drawable* d = addchild();
    d->_rectfill(x1, y1, width, height);
    return d;
}

Drawable* Drawable::circlefill(float x1, float y1, float radius) {
    Drawable* d = addchild();
    d->_circlefill(x1, y1, radius);
    return d;
}

Drawable* Drawable::rectstroke(float x1, float y1, float width, float height) {
    Drawable* d = addchild();
    d->_rectstroke(x1, y1, width, height);
    return d;
}

Drawable* Drawable::circlestroke(float x1, float y1, float radius) {
    Drawable* d = addchild();
    d->_circlestroke(x1, y1, radius);
    return d;
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
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(x, y, 0);
    glRotatef(rotation, 0, 0, 1);
    glScalef(scale, scale, scale);

    if (idxBuffSize > 0) {
        glLineWidth(lineWidth);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(Vertex2), (void*)offsetof(Vertex2, x));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex2), (void*)offsetof(Vertex2, r));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(mode, idxBuffSize, GL_UNSIGNED_SHORT, 0);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    for (int i = 0; i < childrenCount; i++) {
        children[i]->draw();
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Canvas::init() {
    LOGI("canvas init");
    clzparent.init(NULL);
    takenDrawableCount = 0;

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
    clzparent.deinit();
    takenDrawableCount = 0;
}

void Canvas::draw() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);

    clzparent.draw();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}