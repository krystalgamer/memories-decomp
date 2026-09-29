#include "../../types.h"
#include "effect_5.h"

#ifdef VERSION_EUROPE
#define EFFECT_5_RISING_HEIGHT 106
#else
#define EFFECT_5_RISING_HEIGHT 98
#endif

void func_80157E10(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX matrix;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    SVECTOR quad[4];
    MATRIX saved;
    DuelEffect5Work *work;
    s32 i;
    s32 frame_step;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = D_80146218;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 10) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015B450[phase];
            for (i = 0; i < 3; i++) {
                func_8014EA7C(work->config->radii[i], work->rings[i]);
            }
            for (i = 0; i < 64; i++) {
                setVector(&work->rising_positions[i],
                          (rand() - rand()) % 4096 * 70 / 4096, -EFFECT_5_RISING_HEIGHT,
                          (rand() - rand()) % 4096 * 70 / 4096);
                func_8014F010((u8 *)&work->rising_colors[i], 1);
            }
            setVector(&work->number_position, 0, 0, 0);
            setVector(&work->number_velocity, 0, -work->config->bounce_speed, 0);
            func_8014F030(work->config->particle_speed, work->positions,
                         work->velocities, work->config->particle_count);
            func_8014EF2C(work->config->strip_count, work->rotations);
            func_8014F010((u8 *)&work->background_color, 1);
            func_8014F020((u8 *)&work->color, work->config->color[0],
                         work->config->color[1], work->config->color[2]);
            func_8014F020((u8 *)&work->number_color, work->config->number_color[0],
                         work->config->number_color[1], work->config->number_color[2]);
            work->spawned = 0;
            work->bounces = 1;
            work->hold_frames = 0;
            work->number_state = 0;
            work->scale = 4096;
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
        func_801513F4(&world, &saved, &position, &rotation, &scale, 1);
        for (i = 0; i < work->spawned; i++) {
            if ((u16)func_8014D3AC((u8 *)&work->rising_colors[i])) {
                func_8014F2D4(quad, &work->rising_positions[i]);
                func_80156064((u8 *)&work->rising_colors[i], quad, 0);
                work->rising_positions[i].vy += work->config->rise_speed;
            }
            if (work->rising_positions[i].vy < 0) {
                func_80153F98((u8 *)&work->rising_colors[i], work->config->color[0],
                             work->config->color[1], work->config->color[2], 31);
            } else {
                func_80153F28((u8 *)&work->rising_colors[i], 15);
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->background_color)) {
            func_80155798((u8 *)&work->background_color);
            if (work->rising_positions[work->config->strip_count - 1].vy < 0) {
                func_80153F98((u8 *)&work->background_color,
                             work->config->color[0] >> 1,
                             work->config->color[1] >> 1,
                             work->config->color[2] >> 1, 4);
            } else if (work->variant < 5) {
                func_80153F28((u8 *)&work->background_color, 4);
            }
        }
        if (work->rising_positions[work->config->strip_count - 1].vy > 0) {
            if ((u16)func_8014D3AC((u8 *)&work->number_color)) {
                func_801566D4(work->config->number, (u8 *)&work->number_color,
                             &work->number_position, work->number_state, 16, 0);
                addVector(&work->number_position, &work->number_velocity);
                work->number_velocity.vy += 4;
                if (work->number_position.vy >= 0) {
                    work->number_position.vy = 0;
                    if (work->bounces < 5) {
                        work->bounces++;
                        work->number_velocity.vy = -work->config->bounce_speed / work->bounces;
                    } else {
                        if (work->variant < 5) {
                            work->number_state = 1;
                            work->number_velocity.vy = 0;
                            func_80153F28((u8 *)&work->number_color, 15);
                        } else {
                            work->hold_frames++;
                        }
                    }
                }
            }
            if ((u16)func_8014D3AC((u8 *)&work->color)) {
                for (i = 0; i < work->config->particle_count; i++) {
                    func_8014F2D4(quad, &work->positions[i]);
                    func_80156064((u8 *)&work->color, quad, 0);
                    if (work->variant < 5 || work->hold_frames == 0) {
                        addVector(&work->positions[i], &work->velocities[i]);
                    }
                }
                scale.vx = work->scale >> 3;
                scale.vy = work->scale >> 3;
                scale.vz = 4096;
                matrix = saved;
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                func_80156448((u8 *)&work->color, work->rings[0],
                             work->rings[1], work->rings[2], 0);
                scale.vx = work->scale;
                scale.vy = work->scale;
                scale.vz = 4096;
                matrix = saved;
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                func_80155BC0((u8 *)&work->color, work->config->sprite_size,
                             0, &position);
                scale.vx = 4096;
                scale.vy = work->scale;
                scale.vz = 4096;
                for (i = 0; i < work->config->strip_count; i++) {
                    func_801513F4(&world, &saved, &position, &work->rotations[i],
                                 &scale, 0);
                    func_80155D90((u8 *)&work->color, work->config->widths,
                                 work->config->height, 0);
                    if (work->variant < 5 || work->hold_frames == 0) {
                        applyVector(&work->rotations[i], 128, 128, 128, +=);
                    }
                }
                if (work->variant < 5) {
                    func_80153F28((u8 *)&work->color, 15);
                }
                if (work->scale < 0x7000) {
                    work->scale += 0x1000;
                }
            }
        }
        work->spawned += frame_step;
        if (work->spawned > work->config->strip_count) {
            work->spawned = work->config->strip_count;
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->background_color) &&
            (u16)func_8014D378((u8 *)&work->color) &&
            (u16)func_8014D378((u8 *)&work->number_color) && work->variant < 5) {
            D_8009B261 = 1;
        } else if ((u16)func_8014D378((u8 *)&work->rising_colors[work->config->strip_count - 1]) &&
                   work->variant >= 5 && work->hold_frames >= 24) {
            D_8009B261 = 1;
        }
    }
}
