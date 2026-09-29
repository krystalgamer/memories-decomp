#include "../../types.h"
#include "effect_7.h"

#ifdef VERSION_EUROPE
#define EFFECT_7_TRAIL_SPREAD 13
#define EFFECT_7_TARGET_HEIGHT 106
#else
#define EFFECT_7_TRAIL_SPREAD 12
#define EFFECT_7_TARGET_HEIGHT 98
#endif

void func_801587D8(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX matrix;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    CVECTOR color;
    MATRIX saved;
    DuelEffect7Work *work;
    s32 i;
    s32 j;
    s32 x;
    s32 y;
    s32 frame_step;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = D_80146228;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 5) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015B5B8[phase];
            for (i = 0; i < 16; i++) {
                x = (rand() - rand()) % 4096;
                y = (rand() - rand()) % 4096;
                for (j = 0; j < 4; j++) {
                    setVector(&work->trails[i][j], x * 35 / 4096,
                              work->config->start_y + y * EFFECT_7_TRAIL_SPREAD / 4096, 0);
                }
                work->velocities[i].vx =
                    (((rand() - rand()) % 4096) * 70 / 4096 -
                     work->trails[i][0].vx) / work->config->duration;
                work->velocities[i].vy =
                    (((rand() - rand()) % 4096) * EFFECT_7_TARGET_HEIGHT / 4096 -
                     work->trails[i][0].vy) / work->config->duration;
                func_8014F020((u8 *)&work->colors[i], work->config->color[0],
                             work->config->color[1], work->config->color[2]);
                work->ages[i] = 0;
                work->sizes[i] = work->config->size;
                work->scales[i] = 0;
                work->states[i] = 0;
                func_8014F030(work->config->particle_speed, work->particles[i],
                             work->particle_velocities[i], 32);
            }
            setVector(&work->number_position, 0, 0, 0);
            for (i = 0; i < 3; i++) {
                func_8014EA7C(work->config->radii[i], work->rings[i]);
            }
            func_8014F010((u8 *)&work->background_color, 1);
            func_8014F020((u8 *)&work->number_color, work->config->number_color[0],
                         work->config->number_color[1], work->config->number_color[2]);
            work->number_mode = 0;
            work->active = 0;
            work->bounces = 0;
            work->frame = 0;
            work->tick = 0;
            setVector(&work->number_position, 0, 0, 0);
            setVector(&work->number_velocity, 0, -16, 0);
        }
    } else if (work->cross_frame != 0) {
        if (work->cross_frame == 1) {
            func_8014E35C(1);
        } else {
            func_8014E35C(0);
        }
        work->cross_frame++;
        if (work->cross_frame > 180) {
            D_8009B261 = 1;
        }
    } else {
        frame_step = Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        func_801531C4(&world);
        for (i = 0; i < work->active; i++) {
            if (work->states[i] == 0) {
                setVector(&scale, 4096, 4096, 4096);
                func_801513F4(&world, &saved, &position, &rotation, &scale, 0);
                for (j = 0; j < 4; j++) {
                    color = work->colors[i];
                    color.r /= j + 1;
                    color.g /= j + 1;
                    color.b /= j + 1;
                    func_80156C40((u8 *)&color, work->sizes[i] / (j + 1),
                                 &work->trails[i][j]);
                    work->trails[i][j].vx += work->velocities[i].vx / (j + 1);
                    work->trails[i][j].vy += work->velocities[i].vy / (j + 1);
                }
                if (work->ages[i] >= work->config->duration) {
                    work->states[i] = 1;
                } else {
                    work->ages[i]++;
                    work->sizes[i] += work->config->growth;
                }
            } else if (work->states[i] == 1) {
                D_8009B264->field_1D = 1;
                if ((u16)func_8014D3AC((u8 *)&work->colors[i])) {
                    setVector(&scale, 4096, 4096, 4096);
                    func_801513F4(&world, &saved, &work->trails[i][0],
                                 &rotation, &scale, 1);
                    for (j = 0; j < work->config->particle_count; j++) {
                        func_80156C40((u8 *)&work->colors[i], work->config->particle_size,
                                     &work->particles[i][j]);
                        addVector(&work->particles[i][j],
                                  &work->particle_velocities[i][j]);
                    }
                    scale.vx = work->scales[i] >> 1;
                    scale.vy = work->scales[i] >> 1;
                    scale.vz = 0;
                    matrix = saved;
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    func_8015616C((u8 *)&work->colors[i], work->rings[0],
                                 work->rings[1], work->rings[2], 0);
                    func_80155BC0((u8 *)&work->colors[i], work->sizes[i], 0, &position);
                    if (work->scales[i] < 0x6000) {
                        work->scales[i] += 0x1000;
                    }
                    func_80153F28((u8 *)&work->colors[i], 15);
                } else {
                    if ((u16)func_8014D378((u8 *)&work->colors[i])) {
                        work->states[i] = 2;
                    }
                }
            }
        }
        if (!(work->tick & 1)) {
            work->active++;
            if (work->active > work->config->count) {
                work->active = work->config->count;
            }
        }
        scale.vx = 4096;
        scale.vy = 4096;
        scale.vz = 4096;
        func_801513F4(&world, &saved, &position, &rotation, &scale, 0);
        if (work->states[work->config->count - 1] != 0 &&
            (u16)func_8014D3AC((u8 *)&work->number_color)) {
            func_801566D4(work->config->number, (u8 *)&work->number_color,
                         &work->number_position, work->number_mode, 16, 0);
            addVector(&work->number_position, &work->number_velocity);
            work->number_velocity.vy += 4;
            if (work->number_position.vy >= 0) {
                work->number_position.vy = 0;
                if (work->bounces < 5) {
                    work->bounces++;
                    work->number_velocity.vy = -16 / work->bounces;
                } else {
                    work->number_mode = 1;
                    work->number_velocity.vy = 0;
                    func_80153F28((u8 *)&work->number_color, 15);
                }
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->background_color)) {
            func_80155798((u8 *)&work->background_color);
            if (work->states[work->config->count - 1] == 0) {
                func_80153F98((u8 *)&work->background_color,
                             work->config->color[0] >> 1, work->config->color[1] >> 1,
                             work->config->color[2] >> 1, 4);
            } else {
                func_80153F28((u8 *)&work->background_color, 4);
            }
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->colors[work->config->count - 1]) &&
            (u16)func_8014D378((u8 *)&work->background_color) &&
            (u16)func_8014D378((u8 *)&work->number_color)) {
            D_8009B261 = 1;
        }
    }
}
