// src/include/states.h

#ifndef STATES_H
#define STATES_H

#include <stdio.h>
#include <stdbool.h>

typedef struct {
    bool left_eye_closed;
    bool right_eye_closed;
    bool mouth_closed;

    bool smiling;
    bool frowning;
    bool speaking;
    bool tongue_out;

    float eye_openness;
    float mouth_openness;
    float smile_wideness;
} FaceState;

typedef enum {
    EVENT_BLINK,
    EVENT_KISS,
    EVENT_SMILE_START,
    EVENT_SMILE_END,
    EVENT_MOUTH_OPEN,
    EVENT_MOUTH_CLOSE,
    EVENT_SPEECH_START,
    EVENT_SPEECH_END
} EventType;

typedef struct {
    EventType type; 
    float timestamp;
    float intensity;
    float duration;
} Event;

void apply_event(FaceState *face, Event event);

#endif