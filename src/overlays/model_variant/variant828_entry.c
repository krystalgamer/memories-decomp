#include "../../types.h"
#include "variant828_entry.h"
#include "../../game/screen_projection.h"

/* Header 828's entry: one of three camera sequences, picked at random, follows
 * the slot while a fan of light rays opens in front of it and fades, sparks
 * flash out through their texture, a glow and embers mark the burst while its
 * colour lasts, and debris flies out. The ambient level dips during the delay
 * and comes back afterwards. */

static const VECTOR D_8013CA38 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model828State *work;
    MATRIX base, matrix;
    POLY_GT4 rays;
    POLY_FT4 burst;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX saved, turned;
    PSXLONG flag, interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    s32 i;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CA38;
    work = (Model828State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013CAB8[command];
        for (i = 0; i < work->config->ray_count; i++) {
            setVector(&work->rays[i], (rand() - rand()) % 4096, (rand() - rand()) % 4096,
                -direction * (rand() % 4096));
        }
        for (i = 0; i < work->config->spark_max; i++) {
            setVector(&work->sparks[i], work->config->spark_speed * ((rand() - rand()) % 4096) / 4096,
                work->config->spark_speed * ((rand() - rand()) % 4096) / 4096,
                -direction * work->config->spark_speed * (rand() % 4096) / 4096);
            work->frames[i] = 0;
        }
        for (i = 0; i < work->config->debris_count; i++) {
            setVector(&work->debris[i], 0, 0, 0);
            setVector(&work->debris_velocities[i], work->config->debris_speed * ((rand() - rand()) % 4096) / 4096,
                work->config->debris_speed * ((rand() - rand()) % 4096) / 4096,
                -direction * work->config->debris_speed * (rand() % 4096) / 4096);
            work->debris_frame = 0;
        }
        for (i = 0; i < work->config->ember_count; i++) {
            setVector(&work->embers[i], 0, 0, 0);
            setVector(&work->ember_velocities[i], work->config->ember_speed * ((rand() - rand()) % 4096) / 4096,
                work->config->ember_speed * ((rand() - rand()) % 4096) / 4096,
                work->config->ember_speed * ((rand() - rand()) % 4096) / 4096);
        }
        for (i = 0; i < 4; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CA48[i]);
        }
        work->color.rgb.r = work->config->r;
        work->color.rgb.g = work->config->g;
        work->color.rgb.b = work->config->b;
        work->camera = rand() % 4;
        work->ray_scale = 0x1000;
        work->level = 2048;
        work->phase = 0;
        work->elapsed = 0;
        work->spark_count = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->camera == 0) {
        if (work->elapsed < work->config->camera_times[0]) {
            Model_QueueCameraOrbitFromPreset(slot, 2, &rotation, work->config->camera_args[0]);
        } else if (work->elapsed < work->config->camera_times[1]) {
            Model_QueueCameraOrbitFromPreset(slot, 2, &rotation, work->config->camera_args[1]);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 2, &rotation, work->config->camera_args[2]);
        }
    }
    if (work->camera == 1) {
        if (work->elapsed < work->config->camera_times[0]) {
            Model_QueueCameraOrbitFromPreset(slot, 4, &rotation, work->config->camera_args[3]);
        } else if (work->elapsed < work->config->camera_times[1]) {
            Model_QueueCameraOrbitFromPreset(slot, 0, &rotation, work->config->camera_args[4]);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rotation, work->config->camera_args[5]);
        }
    }
    if (work->camera == 2) {
        if (work->elapsed < work->config->camera_times[0]) {
            Model_QueueCameraOrbitFromPreset(slot, 0, &rotation, work->config->camera_args[6]);
        } else if (work->elapsed < work->config->camera_times[1]) {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rotation, work->config->camera_args[7]);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 3, &rotation, work->config->camera_args[8]);
        }
    }
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step << 6;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 1) {
        if (work->level < 2048) {
            work->level += step << 6;
        } else {
            work->phase = 2;
            work->level = 2048;
        }
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    setVector(&position, 0, -350, direction * 450);
    GsSetLsMatrix(&base);
    RotTrans(&position, (VECTOR *)matrix.t, &flag);
    saved = matrix;
    if (work->color.word & 0xFFFFFF) {
        setPolyGT4(&rays);
        rays.tpage = work->texture[1] >> 16;
        rays.clut = work->texture[1];
        setRGB0(&rays, 0, 0, 0);
        setRGB1(&rays, 0, 0, 0);
        setVector(&vertices[0], -work->config->ray_width, -work->config->ray_height, 0);
        setVector(&vertices[1], work->config->ray_width, -work->config->ray_height, 0);
        setVector(&vertices[2], -work->config->ray_base, 0, 0);
        setVector(&vertices[3], work->config->ray_base, 0, 0);
        setUV4(&rays, 0, 0, 31, 0, 0, 127, 31, 127);
        for (i = 0; i < work->config->ray_count; i++) {
            setRGB2(&rays, work->color.rgb.r >> 1, work->color.rgb.g >> 1, work->color.rgb.b >> 1);
            setRGB3(&rays, work->color.rgb.r >> 1, work->color.rgb.g >> 1, work->color.rgb.b >> 1);
            setVector(&scale, 4096, work->ray_scale, 4096);
            matrix = saved;
            RotMatrix(&work->rays[i], &matrix);
            MulMatrix2(&base, &matrix);
            turned = matrix;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&rays.x0, (PSXLONG *)&rays.x1, (PSXLONG *)&rays.x2, (PSXLONG *)&rays.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&rays, ot, (u16)depth, 1);
            }
            setRGB2(&rays, work->color.rgb.r, work->color.rgb.g, work->color.rgb.b);
            setRGB3(&rays, work->color.rgb.r, work->color.rgb.g, work->color.rgb.b);
            setVector(&scale, 2048, work->ray_scale >> 1, 2048);
            matrix = turned;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&rays.x0, (PSXLONG *)&rays.x1, (PSXLONG *)&rays.x2, (PSXLONG *)&rays.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&rays, ot, (u16)depth, 1);
            }
        }
        if (work->ray_scale >= 0x3000) work->ray_scale += 0x100;
        else work->ray_scale += 0x800;
        if (work->color.rgb.r > 15) work->color.rgb.r -= work->config->fade;
        else work->color.rgb.r = 0;
        if (work->color.rgb.g > 15) work->color.rgb.g -= work->config->fade;
        else work->color.rgb.g = 0;
        if (work->color.rgb.b > 15) work->color.rgb.b -= work->config->fade;
        else work->color.rgb.b = 0;
    }
    setVector(&rotation, 0, 0, 0);
    setVector(&scale, 4096, 4096, 4096);
    matrix = saved;
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    setPolyFT4(&burst);
    burst.tpage = work->texture[2] >> 16;
    burst.clut = work->texture[2];
    setRGB0(&burst, 255, 0, 0);
    for (i = 0; i < work->spark_count; i++) {
        if (work->frames[i] < 8) {
            setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
            setVector(&vertices[1], work->config->spark_size, -work->config->spark_size, 0);
            setVector(&vertices[2], -work->config->spark_size, work->config->spark_size, 0);
            setVector(&vertices[3], work->config->spark_size, work->config->spark_size, 0);
            addVector(&vertices[0], &work->sparks[i]);
            addVector(&vertices[1], &work->sparks[i]);
            addVector(&vertices[2], &work->sparks[i]);
            addVector(&vertices[3], &work->sparks[i]);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2, (PSXLONG *)&burst.x3,
                &interpolation, &flag);
            setUV4(&burst, work->frames[i] % 4 * 32 + 32, work->frames[i] / 4 * 32 + 64,
                work->frames[i] % 4 * 32 + 63, work->frames[i] / 4 * 32 + 64,
                work->frames[i] % 4 * 32 + 32, work->frames[i] / 4 * 32 + 95,
                work->frames[i] % 4 * 32 + 63, work->frames[i] / 4 * 32 + 95);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
            }
            work->frames[i]++;
        }
    }
    work->spark_count++;
    if (work->config->spark_max < work->spark_count) {
        work->spark_count = work->config->spark_max;
    }
    if (work->color.word & 0xFFFFFF) {
        burst.tpage = work->texture[0] >> 16;
        burst.clut = work->texture[0];
        setRGB0(&burst, work->color.rgb.r, work->color.rgb.g, work->color.rgb.b);
        setVector(&vertices[0], -work->config->glow_size, -work->config->glow_size, 0);
        setVector(&vertices[1], work->config->glow_size, -work->config->glow_size, 0);
        setVector(&vertices[2], -work->config->glow_size, work->config->glow_size, 0);
        setVector(&vertices[3], work->config->glow_size, work->config->glow_size, 0);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2, (PSXLONG *)&burst.x3,
            &interpolation, &flag);
        setUV4(&burst, 16, 0, 79, 0, 16, 63, 79, 63);
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
        }
        setRGB0(&burst, work->color.rgb.r >> 1, work->color.rgb.g >> 1, work->color.rgb.b >> 1);
        setVector(&vertices[0], -work->config->glow_size * 2, -work->config->glow_size * 2, 0);
        setVector(&vertices[1], work->config->glow_size * 2, -work->config->glow_size * 2, 0);
        setVector(&vertices[2], -work->config->glow_size * 2, work->config->glow_size * 2, 0);
        setVector(&vertices[3], work->config->glow_size * 2, work->config->glow_size * 2, 0);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2, (PSXLONG *)&burst.x3,
            &interpolation, &flag);
        setUV4(&burst, 16, 0, 79, 0, 16, 63, 79, 63);
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
        }
        for (i = 0; i < work->config->ember_count; i++) {
            setVector(&vertices[0], -work->config->ember_size, -work->config->ember_size, 0);
            setVector(&vertices[1], work->config->ember_size, -work->config->ember_size, 0);
            setVector(&vertices[2], -work->config->ember_size, work->config->ember_size, 0);
            setVector(&vertices[3], work->config->ember_size, work->config->ember_size, 0);
            addVector(&vertices[0], &work->embers[i]);
            addVector(&vertices[1], &work->embers[i]);
            addVector(&vertices[2], &work->embers[i]);
            addVector(&vertices[3], &work->embers[i]);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2, (PSXLONG *)&burst.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
            }
            addVector(&work->embers[i], &work->ember_velocities[i]);
        }
    }
    if (work->debris_frame < 24) {
        burst.tpage = work->texture[2] >> 16;
        burst.clut = work->texture[2];
        setRGB0(&burst, 128, 128, 128);
        for (i = 0; i < work->config->debris_count; i++) {
            setVector(&vertices[0], -work->config->debris_size, -work->config->debris_size, 0);
            setVector(&vertices[1], work->config->debris_size, -work->config->debris_size, 0);
            setVector(&vertices[2], -work->config->debris_size, work->config->debris_size, 0);
            setVector(&vertices[3], work->config->debris_size, work->config->debris_size, 0);
            addVector(&vertices[0], &work->debris[i]);
            addVector(&vertices[1], &work->debris[i]);
            addVector(&vertices[2], &work->debris[i]);
            addVector(&vertices[3], &work->debris[i]);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&burst.x0, (PSXLONG *)&burst.x1, (PSXLONG *)&burst.x2, (PSXLONG *)&burst.x3,
                &interpolation, &flag);
            setUV4(&burst, work->debris_frame / 3 % 4 * 32 + 32, (u16)(work->debris_frame / 12) * 32 + 64,
                work->debris_frame / 3 % 4 * 32 + 63, (u16)(work->debris_frame / 12) * 32 + 64,
                work->debris_frame / 3 % 4 * 32 + 32, (u16)(work->debris_frame / 12) * 32 + 95,
                work->debris_frame / 3 % 4 * 32 + 63, (u16)(work->debris_frame / 12) * 32 + 95);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&burst, ot, (u16)depth, 1);
            }
            addVector(&work->debris[i], &work->debris_velocities[i]);
        }
        work->debris_frame++;
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 2 && !(work->color.word & 0xFFFFFF) && work->debris_frame >= 24) {
        return 2;
    }
    if (work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    return 0;
}
