// src/ui/live.c

#include <stdbool.h>
#include "raylib.h"
#include "eeg.h"
#include "face.h"
#include "events.h"

#define HISTORY (SIM_SAMPLE_RATE * 3)
#define WIDTH   1100
#define HEIGHT  720

static EEGSample history[HISTORY];
static int next_slot = 0;
static int sample_count = 0;

static void push_sample(EEGSample sample)
{
    history[next_slot] = sample;
    next_slot = (next_slot + 1) % HISTORY;
    if (sample_count < HISTORY) 
        sample_count++;
}

static double channel_value(EEGSample s, int channel)
{
    switch (channel) {
        case 0: return s.fp1;
        case 1: return s.fp2;
        case 2: return s.cz;
        case 3: return s.o1;
        default: return s.o2;
    }
}

static void draw_traces(void)
{
    const char *names[] = {"Fp1", "Fp2", "Cz", "O1", "O2"};
    const Color colors[] = {
        {255, 107, 138, 255},
        {255, 174, 102, 255},
        {165, 155, 255, 255},
        {94, 224, 192, 255},
        {89, 191, 255, 255}
    };

    const float left = 90.0f;
    const float width = 970.0f;
    const float top = 145.0f;
    const float lane_height = 101.0f;
    const float pixels_per_uv[] = {0.37f, 0.37f, 1.8f, 2.4f, 2.4f};

    for (int channel = 0; channel < 5; channel++) {
        float center_y = top + channel * lane_height + lane_height / 2;

        DrawText(names[channel], 22, (int)center_y - 10, 20, colors[channel]);
        DrawLine((int)left, (int)center_y, (int)(left + width), (int)center_y, (Color){53, 66, 82, 255});

        for (int i = 1; i < sample_count; i++) {
            int previous = (next_slot - sample_count + i - 1 + HISTORY) % HISTORY;
            int current = (next_slot - sample_count + i + HISTORY) % HISTORY;

            float x0 = left + (HISTORY - sample_count + i - 1) * width / (HISTORY - 1);
            float x1 = left + (HISTORY - sample_count + i) * width / (HISTORY - 1);

            float y0 = center_y - (float)channel_value(history[previous], channel) * pixels_per_uv[channel];
            float y1 = center_y - (float)channel_value(history[current], channel) * pixels_per_uv[channel];

            DrawLineV((Vector2){x0, y0}, (Vector2){x1, y1}, colors[channel]);
        }
    }
}

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(WIDTH, HEIGHT, "deltahaze");
    SetTargetFPS(60);

    Font ui_font = LoadFontEx("assets/fonts/AverageSans-Regular.ttf", 32, NULL, 0);
    SetTextureFilter(ui_font.texture, TEXTURE_FILTER_BILINEAR);
    if (ui_font.texture.id == 0) {
        TraceLog(LOG_WARNING, "Could not load custom font");
        ui_font = GetFontDefault();
    }

    FaceSimulator face;
    face_init(&face);

    EEGGenerator generator;
    eeg_init(&generator, 1035);

    Event active_blink = {0};
    double sim_time = 0.0;
    double accumulator = 0.0;
    const double sample_interval = 1.0 / SIM_SAMPLE_RATE;

    const Rectangle button = {90, 75, 130, 43};

    while (!WindowShouldClose()) {
        accumulator += GetFrameTime();

        bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), button);

        if (clicked || IsKeyPressed(KEY_SPACE)) {
            active_blink = blink((float)sim_time);
            face_start_blink(&face, active_blink);
        }

        while (accumulator >= sample_interval) {
            face_update(&face, sim_time);

            EEGSample sample = eeg_next_sample(&generator, sim_time);
            eeg_add_blink(&sample, &active_blink, sim_time);
            push_sample(sample);

            sim_time += sample_interval;
            accumulator -= sample_interval;
        }

        BeginDrawing();
        ClearBackground((Color){11, 16, 23, 255});

        DrawTextEx(ui_font, "deltahaze", (Vector2){90, 23}, 48, 1.0f, 
            (Color){230, 238, 247, 255});
        DrawTextEx(ui_font, TextFormat("SIMULATED EEG  |  256 Hz  |  %.1f s", sim_time),
            (Vector2){680, 34}, 28, 1.0f, (Color){142, 158, 176, 255});

        DrawRectangleRec(button, (Color){51, 85, 103, 255});
        DrawTextEx(ui_font, "BLINK", (Vector2){119, 85}, 22, 1.0f, RAYWHITE);
        DrawTextEx(ui_font, "or press SPACE", (Vector2){235, 87}, 26, 1.0f,
            (Color){142, 158, 176, 255});

        DrawTextEx(ui_font, TextFormat("Traced RAM  %.1f KiB", sizeof(history) / 1024.0),
            (Vector2){682, 90}, 22, 1.0f, (Color){142, 158, 176, 255});

        DrawCircleLines(440, 96, 25, (Color){142, 158, 176, 255});
        if (face.state.left_eye_closed) {
            DrawLine(430, 92, 435, 92, RAYWHITE);
            DrawLine(445, 92, 450, 92, RAYWHITE);
        } else {
            DrawCircle(432, 92, 2, RAYWHITE);
            DrawCircle(448, 92, 2, RAYWHITE);
        }
        DrawLine(434, 105, 446, 105, RAYWHITE);

        draw_traces();
        EndDrawing();
    }

    if (ui_font.texture.id != GetFontDefault().texture.id)
        UnloadFont(ui_font);

    CloseWindow();
    return 0;
}
