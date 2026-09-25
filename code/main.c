// src/code/main.c

#include <stdio.h>
#include "eeg.h"
#include "states.h"
#include "events.h"
#include "face.h"

int run_sim(FaceSimulator *face) {
    FILE *csv = fopen("build/eeg.csv", "w");
    if (csv == NULL) {
        perror("build/eeg.csv");
        return 1;
    }

    fprintf(csv, "time_seconds,Fp1_uV,Fp2_uV,Cz_uV,O1_uV,O2_uV\n");

    EEGGenerator generator;
    eeg_init(&generator, 81035);

    Event scheduled_blink = blink(1.0f);
    bool blink_started = false;

    for (int sample = 0; sample < SIM_SAMPLE_RATE * 5; sample++) {
        double sim_time = (double)sample / SIM_SAMPLE_RATE;

        if (!blink_started && sim_time >= scheduled_blink.timestamp) {
            face_start_blink(face, scheduled_blink);
            blink_started = true;
        }

        face_update(face, sim_time);

        EEGSample eeg = eeg_next_sample(&generator, sim_time);
        eeg_add_blink(&eeg, &scheduled_blink, sim_time);

        fprintf(csv, "%.6f,%.6f,%.6f,%.6f,%.6f,%.6f\n",
            sim_time, eeg.fp1, eeg.fp2, eeg.cz, eeg.o1, eeg.o2);
    }

    if (fclose(csv) != 0) {
        perror("closing build/eeg.csv");
        return 1;
    }

    return 0;
}

int main(void) {
    FaceSimulator face;
    face_init(&face);

    return run_sim(&face);
}
