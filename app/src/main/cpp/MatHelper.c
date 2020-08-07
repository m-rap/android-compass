//
// Created by Rian Prakoso on 8/7/20.
// ported from android.hardware.SensorManager.java
//

#include "MatHelper.h"
#include <math.h>

uint8_t getRotationMatrix(float* R, float* gravity, float* geomagnetic) {

    float Ax = gravity[0];
    float Ay = gravity[1];
    float Az = gravity[2];

    const float normsqA = (Ax * Ax + Ay * Ay + Az * Az);
    const float g = 9.81f;
    const float freeFallGravitySquared = 0.01f * g * g;
    if (normsqA < freeFallGravitySquared) {
        // gravity less than 10% of normal value
        return 0;
    }

    const float Ex = geomagnetic[0];
    const float Ey = geomagnetic[1];
    const float Ez = geomagnetic[2];
    float Hx = Ey * Az - Ez * Ay;
    float Hy = Ez * Ax - Ex * Az;
    float Hz = Ex * Ay - Ey * Ax;
    const float normH = (float) sqrt(Hx * Hx + Hy * Hy + Hz * Hz);

    if (normH < 0.1f) {
        // device is close to free fall (or in space?), or close to
        // magnetic north pole. Typical values are  > 100.
        return 0;
    }
    const float invH = 1.0f / normH;
    Hx *= invH;
    Hy *= invH;
    Hz *= invH;
    const float invA = 1.0f / (float) sqrt(Ax * Ax + Ay * Ay + Az * Az);
    Ax *= invA;
    Ay *= invA;
    Az *= invA;
    const float Mx = Ay * Hz - Az * Hy;
    const float My = Az * Hx - Ax * Hz;
    const float Mz = Ax * Hy - Ay * Hx;

    R[0]  = Hx;    R[1]  = Hy;    R[2]  = Hz;   R[3]  = 0;
    R[4]  = Mx;    R[5]  = My;    R[6]  = Mz;   R[7]  = 0;
    R[8]  = Ax;    R[9]  = Ay;    R[10] = Az;   R[11] = 0;
    R[12] = 0;     R[13] = 0;     R[14] = 0;    R[15] = 1;

    //if (I != null) {
    //    // compute the inclination matrix by projecting the geomagnetic
    //    // vector onto the Z (gravity) and X (horizontal component
    //    // of geomagnetic vector) axes.
    //    final float invE = 1.0f / (float) Math.sqrt(Ex * Ex + Ey * Ey + Ez * Ez);
    //    final float c = (Ex * Mx + Ey * My + Ez * Mz) * invE;
    //    final float s = (Ex * Ax + Ey * Ay + Ez * Az) * invE;
    //    if (I.length == 9) {
    //        I[0] = 1;     I[1] = 0;     I[2] = 0;
    //        I[3] = 0;     I[4] = c;     I[5] = s;
    //        I[6] = 0;     I[7] = -s;     I[8] = c;
    //    } else if (I.length == 16) {
    //        I[0] = 1;     I[1] = 0;     I[2] = 0;
    //        I[4] = 0;     I[5] = c;     I[6] = s;
    //        I[8] = 0;     I[9] = -s;     I[10] = c;
    //        I[3] = I[7] = I[11] = I[12] = I[13] = I[14] = 0;
    //        I[15] = 1;
    //    }
    //}


    return 1;
}

void getOrientation(float* R, float* values) {
    /*
     * 4x4 (length=16) case:
     *   /  R[ 0]   R[ 1]   R[ 2]   0  \
     *   |  R[ 4]   R[ 5]   R[ 6]   0  |
     *   |  R[ 8]   R[ 9]   R[10]   0  |
     *   \      0       0       0   1  /
     *
     * 3x3 (length=9) case:
     *   /  R[ 0]   R[ 1]   R[ 2]  \
     *   |  R[ 3]   R[ 4]   R[ 5]  |
     *   \  R[ 6]   R[ 7]   R[ 8]  /
     *
     */

    values[0] = (float) atan2(R[1], R[5]);
    values[1] = (float) asin(-R[9]);
    values[2] = (float) atan2(-R[8], R[10]);
}

void getOrientation2(float* gravity, float* geomagnetic, float* values) {
    static float R[16];
    uint8_t res = getRotationMatrix(R, gravity, geomagnetic);
    if (res) {
        getOrientation(R, values);
        return;
    }
    values[0] = 0; values[1] = 0; values[2] = 0;
}