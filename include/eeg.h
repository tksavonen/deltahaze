// src/include/eeg.h

#ifndef EEG_H
#define EEG_H

#include <stdint.h>
#include <stdbool.h>
#include "states.h"

#define SIM_SAMPLE_RATE     256

typedef struct {
    double fp1, fp2, cz, o1, o2;
} EEGSample;

typedef struct {
    uint32_t rng_state;
    int samples_until_change;
    double shared_noise;
    double posterior_noise;
    double alpha_amplitude;
    double alpha_target;
    bool alpha_burst_active;

} EEGGenerator;

void eeg_init(EEGGenerator *generator, uint32_t seed);
void eeg_add_blink(EEGSample *sample, const Event *blink, double sim_time);
EEGSample eeg_next_sample(EEGGenerator *generator, double sim_time);

#endif