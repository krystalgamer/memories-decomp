#include "../../types.h"
#include "effect_12.h"

void func_80150E00(void *buffer, s32 phase)
{
    MATRIX world;
    MATRIX saved;
    SVECTOR sprite_rotation;
    SVECTOR rotation;
    VECTOR scale;
    SVECTOR quad[4];
    CVECTOR color_copy;
    DuelEffect12Work *work;
    s32 i;
    s32 frame_step;

    memset(&sprite_rotation, 0, sizeof(sprite_rotation));
    memset(&rotation, 0, sizeof(rotation));
    scale = D_80146188;
    work = buffer;
    if (phase >= 0) {
        work->config = &D_8015AEE4;
        for (i = 0; i < 3; i++) {
            func_8014EE0C(work->config->radii[i], work->config->radii[i],
                         work->config->ring_height * i, work->rings[i], 32);
        }
        copyVector(&work->origin, &D_8015B7F8);
        func_8014F180(work->config->particle_speed, work->positions,
                     work->velocities, 32);
        work->scale = 4096;
        work->frame = 0;
        func_8014F020((u8 *)&work->color, work->config->color.r,
                     work->config->color.g, work->config->color.b);
    } else {
        frame_step = Model_GetFrameStep();
        Model_SetFrameStepOverride(1);
        PushMatrix();
        world = *(MATRIX *)Model_GetLightSourceMatrix();
        func_801513F4(&world, &saved, &work->origin, &rotation, &scale, 3);
        if ((u16)func_8014D3AC((u8 *)&work->color)) {
            for (i = 0; i < 32; i++) {
                func_8014F2D4(quad, &work->positions[i]);
                func_80156064((u8 *)&work->color, quad, 1);
                addVector(&work->positions[i], &work->velocities[i]);
            }
            color_copy = work->color;
            color_copy.r >>= 1;
            color_copy.g >>= 1;
            color_copy.b >>= 1;
            scale.vx = work->scale;
            scale.vy = 4096;
            scale.vz = work->scale;
            func_801514BC(&saved, &scale);
            func_8015616C((u8 *)&color_copy, work->rings[0],
                         work->rings[1], work->rings[2], 1);
            scale.vx = work->scale >> 1;
            scale.vy = 6144;
            scale.vz = work->scale >> 1;
            func_801514BC(&saved, &scale);
            func_8015616C((u8 *)&work->color, work->rings[0],
                         work->rings[1], work->rings[2], 1);
            scale.vx = work->scale >> 1;
            scale.vy = work->scale >> 1;
            scale.vz = work->scale >> 1;
            func_80155BC0((u8 *)&color_copy, work->config->sprite_size,
                         64, &sprite_rotation);
            if (work->scale < 0x4000) {
                work->scale += 0x1000;
            } else {
                D_8009B264->field_1D = 1;
                func_80153F28((u8 *)&work->color, 15);
            }
        }
        work->frame += frame_step;
        PopMatrix();
        if ((u16)func_8014D378((u8 *)&work->color)) {
            D_8009B261 = 1;
        }
    }
}
