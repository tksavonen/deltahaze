// src/code/eeg.c

#include <math.h>
#include <stdbool.h>
#include "eeg.h"

#define PI 3.14159265358979323846

// [-1, 1]
static double random_signed(EEGGenerator *g)
{
    uint32_t x = g->rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    g->rng_state = x;

    return 2.0 * ((double)x / UINT32_MAX) - 1.0;
}

void eeg_init(EEGGenerator *g, uint32_t seed)
{
    *g = (EEGGenerator){0};
    g->rng_state = seed ? seed : 1; // xorshift 
    g->alpha_amplitude = 7.0;
    g->alpha_target = 7.0;
    g->alpha_burst_active = true;
    g->samples_until_change = SIM_SAMPLE_RATE / 2;
}

EEGSample eeg_generate_baseline(double t)
{
    double slow = 8.0 * sin(2.0 * PI * 2.0 * t);

    return (EEGSample){
        .fp1 = slow + 4.0 * sin(2.0 * PI * 9.0 * t),
        .fp2 = slow + 4.0 * sin(2.0 * PI * 9.0 * t + 0.4),
        .cz  = slow + 3.0 * sin(2.0 * PI * 11.0 * t + 1.0)
    };
}

EEGSample eeg_next_sample(EEGGenerator *g, double t)
{
    g->shared_noise = 0.98 * g->shared_noise + 0.20 * random_signed(g);
    g->posterior_noise = 0.97 * g->posterior_noise + 0.24 * random_signed(g);

    if (--g->samples_until_change <= 0) {
        g->alpha_burst_active = !g->alpha_burst_active;

        if (g->alpha_burst_active)
            g->alpha_target = 6.0 + 3.0 * (random_signed(g) + 1.0) / 2.0;
        else 
            g->alpha_target = 0.7;

        // Stay in this state for roughly 0.4–0.9 seconds
        double interval = 0.4 + 0.5 * (random_signed(g) + 1.0) / 2.0;
        g->samples_until_change = (int)(interval * SIM_SAMPLE_RATE);
    }

    // Gradually approach target
    g->alpha_amplitude += 0.025 * (g->alpha_target - g->alpha_amplitude);

    double alpha = g->alpha_amplitude * sin(2.0 * PI * 9.5 * t);

    double broad_activity = 4.0 * g->shared_noise;
    double posterior_activity = 3.0 * g->posterior_noise;

    // initial artistic/model parameters, not measured values
    return (EEGSample){
        .fp1 = 0.18 * alpha + broad_activity + 1.5 * random_signed(g),
        .fp2 = 0.18 * alpha + broad_activity + 1.5 * random_signed(g),
        .cz  = 0.35 * alpha + 0.8 * broad_activity + 1.2 * random_signed(g),
        .o1  = alpha + 0.5 * broad_activity + posterior_activity + 1.0 * random_signed(g),
        .o2  = alpha + 0.5 * broad_activity + posterior_activity + 1.0 * random_signed(g)
    };
}

void eeg_add_blink(EEGSample *sample, const Event *blink, double sim_time)
{
    if (blink->type != EVENT_BLINK || blink->duration <= 0.0f)
        return;

    double progress = (sim_time - blink->timestamp) / blink->duration;

    if (progress < 0.0 || progress > 1.0)
        return;

    const double PEAK = 0.30; // Peak 30% of the way through the blink
    double shape;

    if (progress <= PEAK) {
        double x = progress / PEAK;
        shape = x * x * (3.0 - 2.0 * x); // Smooth rise
    } else {
        double x = (progress - PEAK) / (1.0 - PEAK);
        shape = 1.0 - x * x * (3.0 - 2.0 * x); // Slower recovery
    }

    double artifact = 100.0 * blink->intensity * shape;

    sample->fp1 += artifact;
    sample->fp2 += artifact * 0.95;
    sample->cz  += artifact * 0.12;
}
