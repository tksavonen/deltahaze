// src/code/face.c

#include "states.h"
#include "face.h"

void face_init(FaceSimulator *face)
{
    *face = (FaceSimulator){0};
    face->state.eye_openness = 1.0f;
}

void face_start_blink(FaceSimulator *face, Event event)
{
    face->state.left_eye_closed = true;
    face->state.right_eye_closed = true;
    face->state.eye_openness = 0.0f;
    face->blink_end_time = (double)event.timestamp + event.duration;
}

void face_update(FaceSimulator *face, double sim_time)
{
    if (face->state.left_eye_closed &&
        face->state.right_eye_closed &&
        sim_time >= face->blink_end_time) {
        face->state.left_eye_closed = false;
        face->state.right_eye_closed = false;
        face->state.eye_openness = 1.0f;
    }
}

void apply_event(FaceState *face, Event event) {
    switch (event.type) {
        case EVENT_BLINK:
            face->left_eye_closed = true;
            face->right_eye_closed = true;
            face->eye_openness = 0.0f;
            break;
        case EVENT_SMILE_START:
            face->smiling = true;
            face->smile_wideness = event.intensity;
            break;
        case EVENT_SMILE_END:
            face->smiling = false;
            face->smile_wideness = 0.0f;
            break;
        case EVENT_MOUTH_OPEN:
            face->mouth_openness = event.intensity;
            break;
        case EVENT_MOUTH_CLOSE:
            face->mouth_openness = 0.0f;
            break;
        case EVENT_SPEECH_START:
            face->speaking = true;
            break;
        case EVENT_SPEECH_END:
            face->speaking = false;
            break;
        case EVENT_KISS:
            face->mouth_openness = 0.0f;
            break;
    }
}