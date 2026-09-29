#include "../../types.h"
#include "effect_6.h"

#ifdef VERSION_EUROPE
#define EFFECT_6_START_HEIGHT 106
#define EFFECT_6_TARGET_SPREAD 13
#else
#define EFFECT_6_START_HEIGHT 98
#define EFFECT_6_TARGET_SPREAD 12
#endif

void func_80154B30(void *buffer, s32 phase)
{
    MATRIX world;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    CVECTOR color;
    MATRIX saved;
    DuelEffect6Work *work;
    s32 i;
    s32 j;
    s32 x;
    s32 y;
    s32 frame_step;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = D_801461F8;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 6) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015B30C[phase];
            for (i = 0; i < 16; i++) {
                x = (rand() - rand()) % 4096;
                y = (rand() - rand()) % 4096;
                for (j = 0; j < 4; j++) {
                    setVector(&work->trails[i][j],
                              x * 70 / 4096, y * EFFECT_6_START_HEIGHT / 4096, 0);
                }
                work->velocities[i].vx =
                    (((rand() - rand()) % 4096) * 35 / 4096 -
                     work->trails[i][0].vx) / work->config->duration;
                work->velocities[i].vy =
                    (work->config->target_y +
                     ((rand() - rand()) % 4096) * EFFECT_6_TARGET_SPREAD / 4096 -
                     work->trails[i][0].vy) / work->config->duration;
                func_8014F010((u8 *)&work->colors[i], 1);
                func_8014F020((u8 *)&work->colors[i],
                             work->config->color[0], work->config->color[1],
                             work->config->color[2]);
                work->ages[i] = 0;
                work->sizes[i] = work->config->size;
                work->scales[i] = 0;
                work->states[i] = 0;
                func_8014F030(work->config->particle_speed, work->particles[i],
                             work->particle_velocities[i], 32);
            }
            setVector(&work->column, 0, -60, 0);
            work->column_step = 12;
            work->column_size = 3;
            for (i = 0; i < 3; i++) {
                func_8014EA7C(work->config->radii[i], work->rings[i]);
            }
            func_8014F010((u8 *)&work->background_color, 1);
            func_8014F020((u8 *)&work->column_color,
                         work->config->column_color[0],
                         work->config->column_color[1],
                         work->config->column_color[2]);
            work->column_mode = 0;
            work->active = 0;
            work->hold = 0;
            work->frame = 0;
            work->tick = 0;
            work->variant = phase;
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
                    work->sizes[i] -= work->config->shrink;
                }
            } else if (work->states[i] == 1) {
                D_8009B264->field_1D = 1;
                if ((u16)func_8014D3AC((u8 *)&work->colors[i]) &&
                    work->variant < 5) {
                    setVector(&scale, 4096, 4096, 4096);
                    func_801513F4(&world, &saved, &work->trails[i][0],
                                 &rotation, &scale, 1);
                    for (j = 0; j < work->config->particle_count; j++) {
                        func_80156C40((u8 *)&work->colors[i],
                                     work->config->particle_size,
                                     &work->particles[i][j]);
                        addVector(&work->particles[i][j],
                                  &work->particle_velocities[i][j]);
                    }
                    scale.vx = work->scales[i] >> 1;
                    scale.vy = work->scales[i] >> 1;
                    scale.vz = 0;
                    func_801514BC(&saved, &scale);
                    func_8015616C((u8 *)&work->colors[i], work->rings[0],
                                 work->rings[1], work->rings[2], 0);
                    func_80155BC0((u8 *)&work->colors[i], work->sizes[i] * 2,
                                 0, &position);
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
            (u16)func_8014D3AC((u8 *)&work->column_color) &&
            work->variant < 5) {
            func_801566D4(work->config->column_value, (u8 *)&work->column_color,
                         &work->column, work->column_mode, work->column_size, 0);
            if (work->column_size >= 15) {
                if (work->hold >= 32) {
                    work->column_mode = 1;
                    func_80153F28((u8 *)&work->column_color, 15);
                } else {
                    work->hold++;
                }
            } else {
                work->column.vy += work->column_step;
                work->column_size += 3;
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->background_color) &&
            work->variant < 5) {
            func_80155798((u8 *)&work->background_color);
            if (work->states[work->config->count - 1] == 0) {
                func_80153F98((u8 *)&work->background_color,
                             work->config->color[0] >> 1,
                             work->config->color[1] >> 1,
                             work->config->color[2] >> 1, 4);
            } else {
                func_80153F28((u8 *)&work->background_color, 4);
            }
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if (work->variant >= 5) {
            if (work->states[work->config->count - 1] != 0) {
                D_8009B261 = 1;
            }
        } else {
            if ((u16)func_8014D378((u8 *)&work->colors[work->config->count - 1]) &&
                (u16)func_8014D378((u8 *)&work->background_color) &&
                (u16)func_8014D378((u8 *)&work->column_color)) {
                D_8009B261 = 1;
            }
        }
    }
}
