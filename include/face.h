// src/include/face.h

#ifndef FACE_H
#define FACE_H

#include "states.h"

typedef struct {
    FaceState state;
    double blink_end_time;
} FaceSimulator;

void face_init(FaceSimulator *face);
void face_start_blink(FaceSimulator *face, Event event);
void face_update(FaceSimulator *face, double sim_time);

#endif