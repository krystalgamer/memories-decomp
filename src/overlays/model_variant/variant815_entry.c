#include "../../types.h"
#include "variant815_entry.h"

/* Header 815's entry: at the opposing slot, shards appear one after another
 * and break into four animated quarters, a three-layer glow rises and
 * stretches above them, a growing textured cone is drawn at three scales,
 * and falling sparks and embers spill out while the ambient light dims and
 * recovers. */

static const VECTOR D_8013CFA0 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model815State *work;
    MATRIX base, matrix;
    POLY_FT4 quad;
    POLY_GT4 glow;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX saved;
    PSXLONG flag, interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    s32 i, j;
    PSXLONG depth;
    s16 sign;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013CFA0;
    work = (Model815State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &D_8013D03C[command];
        for (i = 0; i < 48; i++) {
            setVector(&work->shards[i], work->config->spread * ((rand() - rand()) % 4096) / 4096,
                -(work->config->spread >> 1) * (rand() % 4096) / 4096,
                work->config->spread * ((rand() - rand()) % 4096) / 4096);
            work->ages[i] = 0;
        }
        for (i = 0; i < 16; i++) {
            setVector(&work->ring[i][0], work->config->top_radius * ccos(i << 8) / 4096, 0,
                work->config->top_radius * csin(i << 8) / 4096);
            setVector(&work->ring[i][1], work->config->middle_radius * ccos(i << 8) / 4096,
                -work->config->ring_height / 2, work->config->middle_radius * csin(i << 8) / 4096);
            setVector(&work->ring[i][2], work->config->base_radius * ccos(i << 8) / 4096,
                -work->config->ring_height, work->config->base_radius * csin(i << 8) / 4096);
        }
        for (i = 0; i < 64; i++) {
            setVector(&work->sparks[i], 0, 0, 0);
            setVector(&work->spark_velocities[i],
                work->config->spark_speed * ((rand() - rand()) % 4096) / 4096,
                -work->config->spark_speed * (rand() % 4096) / 4096,
                direction * (work->config->spark_speed / 4 + work->config->spark_speed * (rand() % 4096) / 4096));
        }
        for (i = 0; i < 64; i++) {
            setVector(&work->embers[i], 0, 0, 0);
            setVector(&work->ember_velocities[i], (rand() - rand()) % 4096 * 48 / 4096,
                -(rand() % 4096 * 48) / 4096, direction * (rand() % 4096 * 48 / 4096 + 12));
        }
        for (i = 0; i < 5; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013CFB0[i]);
        }
        work->glow.r = 255;
        work->glow.g = 255;
        work->glow.b = 255;
        work->color.r = work->config->r;
        work->color.g = work->config->g;
        work->color.b = work->config->b;
        work->scale = 0;
        work->level = 2048;
        work->phase = 0;
        work->elapsed = 0;
        work->frame = 0;
        work->count = 0;
        setVector(&work->position, 0, 0, 0);
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->near_time) {
        Model_QueueCameraOrbitFromPreset(slot, 1, &rotation, work->config->near_zoom);
    } else if (work->elapsed < work->config->far_time) {
        Model_QueueCameraOrbitFromPreset(slot, 2, &rotation, work->config->far_zoom);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rotation, work->config->end_zoom);
    }
    if (work->phase == 0) {
        if (work->level > -512) {
            work->level -= step << 6;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 2) {
        if (work->level < 2048) {
            work->level += step << 6;
        } else {
            work->phase = 3;
            work->level = 2048;
        }
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->start) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->phase == 0) {
        Model_CopySlotPosition(slot ^ 1, (u16 *)&work->position);
        work->position.vy = 0;
        work->phase = 1;
    }
    GsSetLsMatrix(&base);
    RotTrans(&work->position, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    saved = matrix;
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    setPolyFT4(&quad);
    setRGB0(&quad, 128, 128, 128);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    for (i = 0; i < work->count; i++) {
        if (work->ages[i] < 16) {
            if (work->ages[i] < 2) {
                setVector(&vertices[0], -work->config->shard_size, -work->config->shard_size, 0);
                setVector(&vertices[1], work->config->shard_size, -work->config->shard_size, 0);
                setVector(&vertices[2], -work->config->shard_size, work->config->shard_size, 0);
                setVector(&vertices[3], work->config->shard_size, work->config->shard_size, 0);
                addVector(&vertices[0], &work->shards[i]);
                addVector(&vertices[1], &work->shards[i]);
                addVector(&vertices[2], &work->shards[i]);
                addVector(&vertices[3], &work->shards[i]);
                setUV4(&quad, 0, 0, 31, 0, 0, 31, 31, 31);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                work->ages[i]++;
            } else {
                setVector(&vertices[0], -work->config->shard_size, -work->config->shard_size, 0);
                setVector(&vertices[1], 0, -work->config->shard_size, 0);
                setVector(&vertices[2], -work->config->shard_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->shards[i]);
                addVector(&vertices[1], &work->shards[i]);
                addVector(&vertices[2], &work->shards[i]);
                addVector(&vertices[3], &work->shards[i]);
                setUV4(&quad, work->ages[i] / 2 % 4 << 5, work->ages[i] / 8 << 5,
                    (work->ages[i] / 2 % 4 << 5) + 31, work->ages[i] / 8 << 5,
                    work->ages[i] / 2 % 4 << 5, (work->ages[i] / 8 << 5) + 30,
                    (work->ages[i] / 2 % 4 << 5) + 31, (work->ages[i] / 8 << 5) + 30);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                setVector(&vertices[0], work->config->shard_size, -work->config->shard_size, 0);
                setVector(&vertices[1], 0, -work->config->shard_size, 0);
                setVector(&vertices[2], work->config->shard_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->shards[i]);
                addVector(&vertices[1], &work->shards[i]);
                addVector(&vertices[2], &work->shards[i]);
                addVector(&vertices[3], &work->shards[i]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                setVector(&vertices[0], -work->config->shard_size, work->config->shard_size, 0);
                setVector(&vertices[1], 0, work->config->shard_size, 0);
                setVector(&vertices[2], -work->config->shard_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->shards[i]);
                addVector(&vertices[1], &work->shards[i]);
                addVector(&vertices[2], &work->shards[i]);
                addVector(&vertices[3], &work->shards[i]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                setVector(&vertices[0], work->config->shard_size, work->config->shard_size, 0);
                setVector(&vertices[1], 0, work->config->shard_size, 0);
                setVector(&vertices[2], work->config->shard_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->shards[i]);
                addVector(&vertices[1], &work->shards[i]);
                addVector(&vertices[2], &work->shards[i]);
                addVector(&vertices[3], &work->shards[i]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                work->ages[i]++;
            }
        }
    }
    work->count += step;
    if (work->count > 48) {
        work->count = 48;
    }
    setPolyGT4(&glow);
    /* The glow colour is tested as a 24-bit word. */
    if ((*(u32 *)&work->glow & 0xFFFFFF) && work->count >= 7) {
        setRGB0(&glow, work->glow.r, work->glow.g, work->glow.b);
        setRGB1(&glow, work->glow.r, work->glow.g, work->glow.b);
        setRGB2(&glow, 0, 0, 0);
        setRGB3(&glow, 0, 0, 0);
        glow.tpage = work->texture[2] >> 16;
        glow.clut = work->texture[2];
        setVector(&vertices[0], -work->config->glow_width, 0, 0);
        setVector(&vertices[1], work->config->glow_width, 0, 0);
        setVector(&vertices[2], -work->config->glow_width, -work->config->glow_height, 0);
        setVector(&vertices[3], work->config->glow_width, -work->config->glow_height, 0);
        setVector(&scale, 4096, work->scale + 4096, 4096);
        matrix = saved;
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
            (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3,
            &interpolation, &flag);
        depth -= 128;
        setUV4(&glow, work->frame / 2 % 3 << 6, 64, (work->frame / 2 % 3 << 6) + 63, 64,
            work->frame / 2 % 3 << 6, 191, (work->frame / 2 % 3 << 6) + 63, 191);
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
        }
        setUV4(&glow, (work->frame + 1) / 3 % 3 << 6, 64, ((work->frame + 1) / 3 % 3 << 6) + 63, 64,
            (work->frame + 1) / 3 % 3 << 6, 191, ((work->frame + 1) / 3 % 3 << 6) + 63, 191);
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
        }
        setUV4(&glow, (work->frame + 2) / 4 % 3 << 6, 64, ((work->frame + 2) / 4 % 3 << 6) + 63, 64,
            (work->frame + 2) / 4 % 3 << 6, 191, ((work->frame + 2) / 4 % 3 << 6) + 63, 191);
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
        }
        if (work->glow.r >= 10) work->glow.r -= work->config->fade;
        else work->glow.r = 0;
        if (work->glow.g >= 10) work->glow.g -= work->config->fade;
        else work->glow.g = 0;
        if (work->glow.b >= 10) work->glow.b -= work->config->fade;
        else work->glow.b = 0;
    }
    /* The green and blue channels are tested together as a halfword. */
    if (work->color.r != 0 || *(u16 *)&work->color.g != 0) {
        setRGB0(&glow, 0, 0, 0);
        setRGB1(&glow, 0, 0, 0);
        setRGB2(&glow, work->color.r >> 1, work->color.g >> 1, work->color.b >> 1);
        setRGB3(&glow, work->color.r >> 1, work->color.g >> 1, work->color.b >> 1);
        glow.tpage = work->texture[0] >> 16;
        glow.clut = work->texture[0];
        setUV4(&glow, 0, 192, 31, 192, 0, 255, 31, 255);
        for (j = 0; j < 3; j++) {
            setVector(&scale, work->scale / (j + 1) + 4096, work->scale / (3 - j), work->scale / (j + 1) + 4096);
            matrix = saved;
            MulMatrix2(&base, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (i = 0; i < 16; i++) {
                depth = RotAverage4(&work->ring[i][0], &work->ring[(i + 1) % 16][0],
                    &work->ring[i][1], &work->ring[(i + 1) % 16][1],
                    (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
                }
                depth = RotAverage4(&work->ring[i][2], &work->ring[(i + 1) % 16][2],
                    &work->ring[i][1], &work->ring[(i + 1) % 16][1],
                    (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
                }
            }
        }
        setVector(&scale, 4096, 4096, 4096);
        quad.tpage = work->texture[3] >> 16;
        quad.clut = work->texture[3];
        setRGB0(&quad, work->color.r, work->color.g, work->color.b);
        setUV4(&quad, 32, 192, 63, 192, 32, 223, 63, 223);
        for (i = 0; i < 64; i++) {
            if (work->sparks[i].vy <= 0) {
                sign = 1;
                for (j = 0; j < 4; j++) {
                    sign *= -1;
                    /* The sign multiply keeps the size load ahead of the branch. */
                    setVector(&vertices[0], -sign * work->config->spark_size,
                        (j - 2 < 0 ? 1 : -1) * work->config->spark_size, 0);
                    setVector(&vertices[1], 0, (j - 2 >= 0 ? -1 : 1) * work->config->spark_size, 0);
                    setVector(&vertices[2], -sign * work->config->spark_size, 0, 0);
                    setVector(&vertices[3], 0, 0, 0);
                    setVector(&position, 0, 0, direction * 450);
                    addVector(&position, &work->sparks[i]);
                    GsSetLsMatrix(&base);
                    RotTrans(&position, (VECTOR *)matrix.t, &flag);
                    RotMatrix(&rotation, &matrix);
                    ScaleMatrix(&matrix, &scale);
                    GsSetLsMatrix(&matrix);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                        &interpolation, &flag);
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                    }
                }
                addVector(&work->sparks[i], &work->spark_velocities[i]);
                work->spark_velocities[i].vy += 2;
            }
        }
        quad.tpage = work->texture[4] >> 16;
        quad.clut = work->texture[4];
        setRGB0(&quad, work->color.r, work->color.g, work->color.b);
        setUV4(&quad, 64, 192, 127, 192, 64, 255, 127, 255);
        setVector(&vertices[0], -64, -64, 0);
        setVector(&vertices[1], 64, -64, 0);
        setVector(&vertices[2], -64, 64, 0);
        setVector(&vertices[3], 64, 64, 0);
        for (i = 0; i < 64; i++) {
            if (work->embers[i].vy <= 0) {
                setVector(&position, 0, 0, direction * 450);
                addVector(&position, &work->embers[i]);
                GsSetLsMatrix(&base);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                addVector(&work->embers[i], &work->ember_velocities[i]);
                work->ember_velocities[i].vy += 2;
            }
        }
        if (work->color.r >= 10) work->color.r -= work->config->fade;
        else work->color.r = 0;
        if (work->color.g >= 10) work->color.g -= work->config->fade;
        else work->color.g = 0;
        if (work->color.b >= 10) work->color.b -= work->config->fade;
        else work->color.b = 0;
    }
    if (work->scale < 0x4000) {
        work->scale += 0x800;
    } else if (work->scale < 0x6000) {
        work->scale += 0x80;
    }
    work->elapsed += step;
    work->frame++;
    PopMatrix();
    if (work->phase == 3 && work->ages[work->count - 1] >= 16) {
        return 2;
    }
    if (work->phase == 1 && work->elapsed >= work->config->start) {
        work->phase = 2;
        return 1;
    }
    return 0;
}
