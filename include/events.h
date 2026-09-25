// src/include/events.h

#ifndef EVENTS_H
#define EVENTS_H

#include "states.h"

static inline Event blink(double time_seconds)
{
    return (Event){
        .type = EVENT_BLINK,
        .timestamp = time_seconds,
        .intensity = 1.0f,
        .duration = 0.2f
    };
}

#endif