#include "../../types.h"
#include "effect_0.h"

void func_80154688(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX working;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX saved;
    DuelEffect0Work *work;
    s32 i;
    s32 frame_step;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = D_801461E8;
    work = buffer;
    if (phase >= 0) {
        if (phase >= 30) {
            work->cross_frame = 1;
        } else {
            work->cross_frame = 0;
            work->config = &D_8015B0B4[phase];
            for (i = 0; i < 3; i++) {
                func_8014EA7C(work->config->radii[i], work->rings[i]);
            }
            for (i = 0; i < 2; i++) {
                func_8014EB1C(24 + i * 6, 28 + i * 6, work->quads[i], 4);
            }
            func_8014EF2C(16, work->rotations);
            func_8014F010((u8 *)&work->color, 1);
            work->scale = 4096;
            work->frame = 0;
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
        if ((u16)func_8014D3AC((u8 *)&work->color)) {
            setVector(&position, 0, 0, 0);
            setVector(&rotation, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            func_801513F4(&world, &saved, &position, &rotation, &scale, 1);
            func_801558F4((u8 *)&work->color, work->quads[0], work->quads[1], 0, 1);
            if (work->frame >= work->config->delay) {
                D_8009B264->field_1D = 1;
                scale.vx = work->scale >> 3;
                scale.vy = work->scale >> 3;
                scale.vz = 4096;
                working = saved;
                ScaleMatrix(&working, &scale);
                GsSetLsMatrix(&working);
                func_80156448((u8 *)&work->color, work->rings[0],
                             work->rings[1], work->rings[2], 0);
                scale.vx = work->scale;
                scale.vy = work->scale;
                scale.vz = 4096;
                working = saved;
                ScaleMatrix(&working, &scale);
                GsSetLsMatrix(&working);
                func_80155BC0((u8 *)&work->color, work->config->sprite_size,
                             0, &position);
                scale.vx = 4096;
                scale.vy = work->scale;
                scale.vz = 4096;
                for (i = 0; i < 16; i++) {
                    func_801513F4(&world, &saved, &position,
                                 &work->rotations[i], &scale, 2);
                    func_80155D90((u8 *)&work->color, work->config->widths,
                                 work->config->height, 0);
                    applyVector(&work->rotations[i], 128, 128, 128, +=);
                }
                func_80153F28((u8 *)&work->color, 15);
                if (work->scale < 0x7000) {
                    work->scale += 0x1000;
                }
            } else {
                func_80153F98((u8 *)&work->color, work->config->color.r,
                             work->config->color.g, work->config->color.b, 31);
            }
        }
        work->frame += frame_step;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->color)) {
            D_8009B261 = 1;
        }
    }
}
