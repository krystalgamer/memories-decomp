#include "../../types.h"
#include "effect_9.h"

void func_801593A8(void *buffer, s32 phase)
{
    MATRIX world;
    SVECTOR translation;
    SVECTOR rotation;
    VECTOR scale;
    SVECTOR quad[4];
    MATRIX saved;
    DuelEffect9Work *work;
    s32 i;
    s32 frame_step;

    memset(&translation, 0, sizeof(translation));
    memset(&rotation, 0, sizeof(rotation));
    scale = D_80146238;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 5) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015B650[phase];
            for (i = 0; i < 3; i++) {
                func_8014EA7C(work->config->radii[i], work->rings[i]);
            }
            setVector(&work->negative, 0, -120, 0);
            work->negative_step = 0;
            setVector(&work->positive, 0, 0, 0);
            work->positive_step = 0;
            func_8014EF2C(work->config->rotation_count, work->rotations);
            func_8014F030(work->config->particle_speed, work->positions,
                         work->velocities, 32);
            func_8014F020((u8 *)&work->negative_color, 255, 64, 64);
            func_8014F020((u8 *)&work->background_color, work->config->color[0] >> 2,
                         work->config->color[1] >> 2, work->config->color[2] >> 2);
            func_8014F020((u8 *)&work->effects_color, work->config->color[0],
                         work->config->color[1], work->config->color[2]);
            func_8014F020((u8 *)&work->positive_color, work->config->number_color[0],
                         work->config->number_color[1], work->config->number_color[2]);
            work->bounce_count = 1;
            work->unused_72C = 0;
            work->negative_mode = 0;
            work->positive_mode = 0;
            work->stage = 0;
            work->scale = 0x7000;
            work->frame = 0;
            work->tick = 0;
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
        func_801513F4(&world, &saved, &translation, &rotation, &scale, 1);
        if ((u16)func_8014D3AC((u8 *)&work->background_color)) {
            func_80155798((u8 *)&work->background_color);
            if (work->stage != 0) {
                func_80153F28((u8 *)&work->background_color, 4);
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->negative_color)) {
            func_801566D4(-work->config->value, (u8 *)&work->negative_color,
                         &work->negative, work->negative_mode, 16, 0);
            work->negative.vy += work->negative_step;
            work->negative_step += 4;
            if (work->negative.vy >= 0) {
                work->negative.vy = 0;
                if (work->stage == 0) {
                    work->positive_step = work->negative_step;
                    work->negative_step = -work->config->bounce_speed;
                    work->stage = 1;
                } else if (work->bounce_count < 5) {
                    work->bounce_count++;
                    work->negative_step = -work->config->bounce_speed /
                                          work->bounce_count;
                } else {
                    work->negative_mode = 1;
                    work->negative_step = 0;
                    func_80153F28((u8 *)&work->negative_color, 15);
                }
            }
        }
        if ((u16)func_8014D3AC((u8 *)&work->positive_color) &&
            work->positive.vy < 200) {
            func_801566D4(work->config->value, (u8 *)&work->positive_color,
                         &work->positive, work->positive_mode, 16, 0);
            work->positive.vy += work->positive_step;
            if (work->stage != 0) {
                work->positive_mode = 1;
                func_80153F28((u8 *)&work->positive_color, 15);
            }
        } else if (work->positive.vy >= 200) {
            func_8014F010((u8 *)&work->positive_color, 0);
        }
        if ((u16)func_8014D3AC((u8 *)&work->effects_color)) {
            if (work->stage >= 2) {
                for (i = 0; i < 32; i++) {
                    func_8014F2D4(quad, &work->positions[i]);
                    func_80156064((u8 *)&work->effects_color, quad, 0);
                    addVector(&work->positions[i], &work->velocities[i]);
                }
            }
            scale.vx = work->scale >> 3;
            scale.vy = work->scale >> 3;
            scale.vz = 4096;
            func_801514BC(&saved, &scale);
            func_80156448((u8 *)&work->effects_color, work->rings[0],
                         work->rings[1], work->rings[2], 0);
            scale.vx = work->scale;
            scale.vy = work->scale;
            scale.vz = 4096;
            func_801514BC(&saved, &scale);
            func_80155BC0((u8 *)&work->effects_color, work->config->sprite_size,
                         0, &translation);
            scale.vx = 4096;
            scale.vy = work->scale;
            scale.vz = 4096;
            for (i = 0; i < work->config->rotation_count; i++) {
                func_801513F4(&world, &saved, &translation, &work->rotations[i],
                             &scale, 2);
                func_80155D90((u8 *)&work->effects_color, work->config->widths,
                             work->config->height, 0);
            }
            if (work->stage != 0) {
                func_80153F28((u8 *)&work->effects_color, 15);
            }
        }
        if (work->stage == 1) {
            SD_SEPlayFull(26);
            D_8009B264->field_1D = 1;
            func_8014F020((u8 *)&work->background_color, 63, 16, 63);
            func_8014F020((u8 *)&work->effects_color, 255, 64, 255);
            work->stage = 2;
        }
        work->frame += frame_step;
        work->tick++;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->negative_color) &&
            (u16)func_8014D378((u8 *)&work->background_color) &&
            (u16)func_8014D378((u8 *)&work->effects_color) &&
            (u16)func_8014D378((u8 *)&work->positive_color)) {
            D_8009B261 = 1;
        }
    }
}
