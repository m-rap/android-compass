//
// Created by Rian Prakoso on 8/7/20.
// ported from android.hardware.SensorManager.java
//

#ifndef COMPASS_MATHELPER_H
#define COMPASS_MATHELPER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t getRotationMatrix(float *R, float *gravity, float *geomagnetic);

void getOrientation(float *R, float *values);

void getOrientation2(float *gravity, float *geomagnetic, float *values);

#ifdef __cplusplus
}
#endif

#endif //COMPASS_MATHELPER_H
