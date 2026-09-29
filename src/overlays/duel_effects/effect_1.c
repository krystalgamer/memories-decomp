#include "../../types.h"
#include "effect_1.h"

void func_80157794(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR rotation;
    SVECTOR translation;
    VECTOR scale;
    SVECTOR quad[4];
    CVECTOR color_copy;
    DuelEffect1Work *work;
    s32 i;
    s32 frame_step;

    memset(&rotation, 0, sizeof(rotation));
    memset(&translation, 0, sizeof(translation));
    scale = D_80146208;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 2) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015B420[phase];
            Model_SetFrameStepOverride(1);
            func_8014EF2C(12, work->rotations);
            func_8014F030(work->config->particle_speed, work->positions,
                         work->velocities, 64);
            func_8014F010((u8 *)&work->base_color, 1);
            func_8014F010((u8 *)&work->beam_color, 0);
            func_8014F010((u8 *)&work->screen_color, 0);
            work->rotation_step = work->config->initial_rotation_step;
            work->frame = 0;
            work->tick = 0;
            work->stage = 0;
            work->growth = 0;
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
        if ((u16)func_8014D3AC((u8 *)&work->base_color)) {
            scale.vx = 4096 + (work->growth >> 4);
            scale.vy = 4096 + (work->growth >> 4);
            scale.vz = 4096;
            setVector(&rotation, 0, 0,
                      (work->rotation_step * work->tick) & 0xFFF);
            func_801513F4(&world, &saved, &translation, &rotation, &scale, 0);
            func_80155F94((u8 *)&work->base_color);
            if (work->frame < work->config->first_end) {
                func_80153F98((u8 *)&work->base_color, 128, 128, 128, 8);
                work->rotation_step += work->config->rotation_acceleration;
            } else if (work->frame < work->config->second_end) {
                func_80153F98((u8 *)&work->beam_color, work->config->color.r,
                             work->config->color.g, work->config->color.b, 15);
                if (work->growth < 0x7000) {
                    work->growth += 0x400;
                }
            } else {
                if (work->stage == 0) {
                    work->stage = 1;
                    func_8014F010((u8 *)&work->screen_color, 255);
                    work->rotation_step = work->config->initial_rotation_step;
                }
                func_80153F28((u8 *)&work->base_color, 15);
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->beam_color)) {
            scale.vx = 4096 + (work->growth >> 2);
            scale.vy = 4096 + (work->growth >> 2);
            scale.vz = 4096;
            setVector(&rotation, 0, 0,
                      (work->config->beam_rotation_step * work->tick) & 0xFFF);
            func_801513F4(&world, &saved, &translation, &rotation, &scale, 0);
            color_copy = work->beam_color;
            color_copy.r >>= 2;
            color_copy.g >>= 2;
            color_copy.b >>= 2;
            func_80155BC0((u8 *)&color_copy, work->config->sprite_size,
                         0, &translation);
            if (work->stage == 0) {
                scale.vx = 4096;
                scale.vy = 4096 + (work->growth >> 3);
                scale.vz = 4096;
                for (i = 0; i < 12; i++) {
                    func_801513F4(&world, &saved, &translation, &work->rotations[i],
                                 &scale, 2);
                    /* The strip still uses the original color after this copy. */
                    color_copy = work->beam_color;
                    color_copy.r >>= 2;
                    color_copy.g >>= 2;
                    color_copy.b >>= 2;
                    func_80155D90((u8 *)&work->beam_color, work->config->widths,
                                 work->config->height, 0);
                    applyVector(&work->rotations[i],
                                work->config->beam_rotation_step,
                                work->config->beam_rotation_step,
                                work->config->beam_rotation_step, +=);
                }
            } else {
                setVector(&scale, 4096, 4096, 4096);
                setVector(&rotation, 0, 0,
                          (work->rotation_step * (work->tick << 1)) & 0xFFF);
                func_801513F4(&world, &saved, &translation, &rotation, &scale, 0);
                for (i = 0; i < 64; i++) {
                    func_8014F2D4(quad, &work->positions[i]);
                    func_80156064((u8 *)&work->beam_color, quad, 0);
                    addVector(&work->positions[i], &work->velocities[i]);
                }
            }
            if (work->stage != 0) {
                func_80153F28((u8 *)&work->beam_color, 8);
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->screen_color)) {
            func_801556F4((u8 *)&work->screen_color, 15);
            D_8009B264->field_1D = 1;
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->base_color) &&
            (u16)func_8014D378((u8 *)&work->beam_color) &&
            (u16)func_8014D378((u8 *)&work->screen_color)) {
            D_8009B261 = 1;
        }
    }
}
