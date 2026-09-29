#include "../../types.h"
#include "effect_19.h"

void func_80153ADC(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR position;
    SVECTOR rotation;
    VECTOR scale;
    CVECTOR color_copy;
    SVECTOR quad[4];
    DuelEffect19Work *work;
    s32 i;

    memset(&position, 0, sizeof(position));
    memset(&rotation, 0, sizeof(rotation));
    scale = D_801461C8;
    work = buffer;
    if (phase >= 0) {
        work->widths[0] = 64;
        work->widths[1] = 67;
        work->widths[2] = 68;
        func_8014EF2C(64, work->rotations);
        for (i = 0; i < 3; i++) {
            func_8014EA7C(work->widths[i], work->rings[i]);
        }
        func_8014F030(6, work->positions, work->velocities, 32);
        work->scale = 4096;
        work->frame = 0;
        work->fade_started = 0;
        work->spawned = 0;
        func_8014F020((u8 *)&work->color, 31, 28, 30);
    } else {
        work->widths[0] = 1;
        work->widths[1] = 2;
        Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        func_801531C4(&world);
        func_801513F4(&world, &saved, &position, &rotation, &scale, 1);
        if ((u16)func_8014D3AC((u8 *)&work->color)) {
            if (work->fade_started == 1) {
                for (i = 0; i < 32; i++) {
                    func_8014F2D4(quad, &work->positions[i]);
                    func_80156064((u8 *)&work->color, quad, 0);
                    addVector(&work->positions[i], &work->velocities[i]);
                }
            }
            scale.vx = work->scale;
            scale.vy = work->scale;
            scale.vz = 0;
            func_801514BC(&saved, &scale);
            func_80155BC0((u8 *)&work->color, 12, 0, &position);
            scale.vx = work->scale >> 2;
            scale.vy = work->scale >> 2;
            scale.vz = 0;
            func_801514BC(&saved, &scale);
            func_80156448((u8 *)&work->color, work->rings[0],
                         work->rings[1], work->rings[2], 0);
            scale.vx = 4096;
            scale.vy = 4096 + (work->scale >> 2);
            scale.vz = 4096;
            if (work->fade_started == 0) {
                for (i = 0; i < work->spawned; i++) {
                    func_801513F4(&world, &saved, &position,
                                 &work->rotations[i], &scale, 2);
                    color_copy = work->color;
                    color_copy.r >>= 2;
                    color_copy.g >>= 2;
                    color_copy.b >>= 2;
                    func_80155D90((u8 *)&work->color, work->widths, 48, 0);
                    work->rotations[i].vx += 32;
                    work->rotations[i].vy += 32;
                    work->rotations[i].vz += 32;
                }
                work->spawned++;
                if (work->spawned > 64) {
                    work->spawned = 64;
                }
            }
            if (work->fade_started == 0) {
                work->fade_started =
                    func_80153F98((u8 *)&work->color, 255, 224, 240, 4);
                if (work->scale < 0x5000) {
                    work->scale += 0x100;
                } else {
                    work->scale = 0x5000;
                }
            } else {
                D_8009B264->field_1D = 1;
                func_801556F4((u8 *)&work->color, 8);
            }
        }
        work->frame++;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->color)) {
            D_8009B261 = 1;
        }
    }
}
