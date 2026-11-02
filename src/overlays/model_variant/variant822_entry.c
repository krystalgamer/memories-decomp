#include "../../types.h"
#include "variant822_entry.h"

/* Header 822's entry: five spinning projectiles leave five model parts and
 * fly to the opposing slot while their colour fades in, growing twice on
 * the way, then burst there into
 * thirty-two clusters; each cluster flashes an animated spark, scatters
 * eight falling shards that fade and three falling debris sprites. The
 * ambient level dips during the flight and comes back afterwards. */

static const VECTOR gModelVariant822_D1BF0 = {4096, 4096, 4096, 0};

s32 ModelVariant822_HandleRequest(u8 *context, s32 command)
{
    Model822State *work;
    MATRIX base, matrix;
    POLY_GT4 head;
    POLY_FT4 sprite;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    s32 i, j;
    s32 size;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant822_D1BF0;
    work = (Model822State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant822_D1C54[command];
        for (i = 0; i < 5; i++) {
            setVector(&work->position[i], 0, 0, 0);
            setVector(&work->velocity[i], 0, 0, 0);
        }
        for (i = 0; i < 32; i++) {
            setVector(&work->centers[i], 0, 0, 0);
            work->spark_frames[i] = rand() % 8;
            work->r[i] = work->config->r;
            work->g[i] = work->config->g;
            work->b[i] = work->config->b;
            work->debris_frames[i] = 0;
            for (j = 0; j < 3; j++) {
                setVector(&work->debris[i][j], work->config->speed * ((rand() - rand()) % 4096) / 4096,
                    work->config->speed * ((rand() - rand()) % 4096) / 4096,
                    work->config->speed * ((rand() - rand()) % 4096) / 4096);
            }
            for (j = 0; j < 8; j++) {
                setVector(&work->shards[i][j], work->config->speed * ((rand() - rand()) % 4096) / 4096,
                    work->config->speed * ((rand() - rand()) % 4096) / 4096,
                    work->config->speed * ((rand() - rand()) % 4096) / 4096);
            }
        }
        work->width = work->config->width;
        work->height = work->config->height;
        setVector(&work->origin, 0, 0, 0);
        work->texture[0] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 2, &gModelVariant822_D1C00[0]);
        work->texture[1] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant822_D1C00[1]);
        work->texture[2] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant822_D1C00[2]);
        work->red = 0;
        work->green = 0;
        work->blue = 0;
        work->frame = 0;
        work->level = 2048;
        work->phase = 0;
        work->stage = 0;
        work->grow = 0;
        work->elapsed = 0;
        work->spin = 0;
        work->count = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (direction < 0) {
        if (work->elapsed < work->config->camera_time) {
            Model_QueueCameraOrbitFromPreset(slot, 13, &rotation, -16);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 14, &rotation, work->config->travel - 2);
        }
    } else {
        if (work->elapsed < work->config->camera_time) {
            Model_QueueCameraOrbitFromPreset(slot, 14, &rotation, -16);
        } else {
            Model_QueueCameraOrbitFromPreset(slot ^ 1, 13, &rotation, work->config->travel - 2);
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
    if (work->elapsed >= work->config->grow_time && work->grow == 0) {
        work->grow = 1;
        work->width *= 2;
        work->height *= 2;
    }
    if (work->elapsed >= work->config->grow_again_time && work->grow == 1) {
        work->grow = 2;
        work->width *= 2;
        work->height *= 2;
    }
    if (work->elapsed <= work->config->camera_time) {
        for (i = 0; i < 5; i++) {
            GsGetLwUnit(Model_GetSlotDataEntry(Model_GetActiveSlotIndex(), work->config->parts[i]), &matrix);
            setVector(&work->position[i], matrix.t[0], matrix.t[1], matrix.t[2]);
            setVector(&work->velocity[i], -work->position[i].vx / work->config->travel,
                (-350 - work->position[i].vy) / work->config->travel,
                (direction * 450 - work->position[i].vz) / work->config->travel);
        }
    }
    for (i = 0; i < 5; i++) {
        if (work->stage == 0) {
            setPolyGT4(&head);
            head.tpage = work->texture[1] >> 16;
            head.clut = work->texture[1];
            setRGB0(&head, 0, 0, 0);
            setRGB1(&head, 0, 0, 0);
            setRGB2(&head, work->red, work->green, work->blue);
            setRGB3(&head, work->red, work->green, work->blue);
            setVector(&vertices[0], -work->width, -work->height, 0);
            setVector(&vertices[1], work->width, -work->height, 0);
            setVector(&vertices[2], -work->width, 0, 0);
            setVector(&vertices[3], work->width, 0, 0);
            setVector(&position, 0, 0, 0);
            setVector(&rotation, 0, 0, work->spin);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&base);
            RotTrans(&work->position[i], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&head.x0, (PSXLONG *)&head.x1, (PSXLONG *)&head.x2, (PSXLONG *)&head.x3,
                &interpolation, &flag);
            setUV4(&head, work->frame % 4 * 64, work->frame / 4 * 87,
                work->frame % 4 * 64 + 63, work->frame / 4 * 87,
                work->frame % 4 * 64, work->frame / 4 * 87 + 86,
                work->frame % 4 * 64 + 63, work->frame / 4 * 87 + 86);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&head, ot, (u16)depth, 1);
            }
            work->frame = (work->frame + 1) % 8;
            if (work->red < work->config->r - 15) work->red += work->config->fade;
            else work->red = work->config->r;
            if (work->green < work->config->g - 15) work->green += work->config->fade;
            else work->green = work->config->g;
            if (work->blue < work->config->b - 15) work->blue += work->config->fade;
            else work->blue = work->config->b;
            if (work->elapsed >= work->config->camera_time) {
                if (ABS(work->spin) < 1024) {
                    work->spin += direction * (step << 8);
                } else {
                    work->spin = direction * 1024;
                }
                addVector(&work->position[i], &work->velocity[i]);
                if (ABS(work->position[i].vz) > 450) {
                    work->stage = 1;
                }
            }
        }
    }
    if (work->stage != 0) {
        if (work->stage == 1) {
            Model_CopySlotPosition(slot ^ 1, (u16 *)&work->origin);
            Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rotation);
            for (i = 0; i < 32; i++) {
                setVector(&work->centers[i], rotation.vx / 2 * ((rand() - rand()) % 4096) / 4096,
                    rotation.vy / 2 * ((rand() - rand()) % 4096) / 4096,
                    -direction * (rotation.vz / 2) * (rand() % 4096) / 4096);
                for (j = 0; j < 3; j++) {
                    addVector(&work->debris[i][j], &work->centers[i]);
                }
                for (j = 0; j < 8; j++) {
                    addVector(&work->shards[i][j], &work->centers[i]);
                }
            }
            work->stage = 2;
        }
        setVector(&rotation, 0, 0, 0);
        setVector(&scale, 4096, 4096, 4096);
        setVector(&position, 0, 0, 0);
        GsSetLsMatrix(&base);
        RotTrans(&work->origin, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        for (i = 0; i < work->count; i++) {
            if (work->r[i] || work->g[i] || work->b[i]) {
                setPolyFT4(&sprite);
                sprite.tpage = work->texture[1] >> 16;
                sprite.clut = work->texture[1];
                setRGB0(&sprite, work->r[i], work->g[i], work->b[i]);
                setVector(&vertices[0], -work->config->spark_width, -work->config->spark_height, 0);
                setVector(&vertices[1], work->config->spark_width, -work->config->spark_height, 0);
                setVector(&vertices[2], -work->config->spark_width, work->config->spark_height, 0);
                setVector(&vertices[3], work->config->spark_width, work->config->spark_height, 0);
                addVector(&vertices[0], &work->centers[i]);
                addVector(&vertices[1], &work->centers[i]);
                addVector(&vertices[2], &work->centers[i]);
                addVector(&vertices[3], &work->centers[i]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&sprite.x0, (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2, (PSXLONG *)&sprite.x3,
                    &interpolation, &flag);
                setUV4(&sprite, work->spark_frames[i] % 4 * 64, work->spark_frames[i] / 4 * 87,
                    work->spark_frames[i] % 4 * 64 + 63, work->spark_frames[i] / 4 * 87,
                    work->spark_frames[i] % 4 * 64, work->spark_frames[i] / 4 * 87 + 86,
                    work->spark_frames[i] % 4 * 64 + 63, work->spark_frames[i] / 4 * 87 + 86);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)depth, 1);
                }
                work->spark_frames[i] = (work->spark_frames[i] + 1) % 8;
                sprite.tpage = work->texture[2] >> 16;
                sprite.clut = work->texture[2];
                setRGB0(&sprite, work->r[i], work->g[i], work->b[i]);
                setUV4(&sprite, 0, 206, 31, 206, 0, 237, 31, 237);
                for (j = 0; j < 8; j++) {
                    size = rand() % work->config->shard_size / 2 + work->config->shard_size / 2;
                    setVector(&vertices[0], -size, -size, 0);
                    setVector(&vertices[1], size, -size, 0);
                    setVector(&vertices[2], -size, size, 0);
                    setVector(&vertices[3], size, size, 0);
                    addVector(&vertices[0], &work->centers[i]);
                    addVector(&vertices[1], &work->centers[i]);
                    addVector(&vertices[2], &work->centers[i]);
                    addVector(&vertices[3], &work->centers[i]);
                    addVector(&vertices[0], &work->shards[i][j]);
                    addVector(&vertices[1], &work->shards[i][j]);
                    addVector(&vertices[2], &work->shards[i][j]);
                    addVector(&vertices[3], &work->shards[i][j]);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&sprite.x0, (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2, (PSXLONG *)&sprite.x3,
                        &interpolation, &flag);
                    if (depth >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)depth, 1);
                    }
                    work->shards[i][j].vy -= work->config->shard_fall;
                }
                if (work->r[i] > 15) work->r[i] -= work->config->fade;
                else work->r[i] = 0;
                if (work->g[i] > 15) work->g[i] -= work->config->fade;
                else work->g[i] = 0;
                if (work->b[i] > 15) work->b[i] -= work->config->fade;
                else work->b[i] = 0;
            }
            if (work->debris_frames[i] < 24) {
                setPolyFT4(&sprite);
                sprite.tpage = work->texture[0] >> 16;
                sprite.clut = work->texture[0];
                setRGB0(&sprite, 48, 48, 48);
                setVector(&vertices[0], -work->config->debris_size, -work->config->debris_size, 0);
                setVector(&vertices[1], work->config->debris_size, -work->config->debris_size, 0);
                setVector(&vertices[2], -work->config->debris_size, work->config->debris_size, 0);
                setVector(&vertices[3], work->config->debris_size, work->config->debris_size, 0);
                setVector(&position, 0, 0, 0);
                setVector(&rotation, 0, 0, 0);
                setVector(&scale, 4096, 4096, 4096);
                setUV4(&sprite, (u16)(work->debris_frames[i] / 3) * 32, 174,
                    (u16)(work->debris_frames[i] / 3) * 32 + 31, 174,
                    (u16)(work->debris_frames[i] / 3) * 32, 205,
                    (u16)(work->debris_frames[i] / 3) * 32 + 31, 205);
                for (j = 0; j < 3; j++) {
                    setVector(&vertices[0], -work->config->debris_size, -work->config->debris_size, 0);
                    setVector(&vertices[1], work->config->debris_size, -work->config->debris_size, 0);
                    setVector(&vertices[2], -work->config->debris_size, work->config->debris_size, 0);
                    setVector(&vertices[3], work->config->debris_size, work->config->debris_size, 0);
                    addVector(&vertices[0], &work->centers[i]);
                    addVector(&vertices[1], &work->centers[i]);
                    addVector(&vertices[2], &work->centers[i]);
                    addVector(&vertices[3], &work->centers[i]);
                    addVector(&vertices[0], &work->debris[i][j]);
                    addVector(&vertices[1], &work->debris[i][j]);
                    addVector(&vertices[2], &work->debris[i][j]);
                    addVector(&vertices[3], &work->debris[i][j]);
                    depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                        (PSXLONG *)&sprite.x0, (PSXLONG *)&sprite.x1, (PSXLONG *)&sprite.x2, (PSXLONG *)&sprite.x3,
                        &interpolation, &flag);
                    if (depth >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&sprite, ot, (u16)depth, 2);
                    }
                    work->debris[i][j].vy -= work->config->debris_fall;
                }
                work->debris_frames[i]++;
            }
        }
        work->count += step * 2;
        if (work->count > 32) {
            work->count = 32;
        }
    }
    work->elapsed += step;
    PopMatrix();
    if (work->phase == 2 && !work->r[31] && !work->g[31] && !work->b[31] && work->debris_frames[31] >= 24) {
        Model_SetFrameStepOverride(0);
        return 2;
    }
    if (work->stage == 2) {
        work->stage = 3;
        work->phase = 1;
        return 1;
    }
    return 0;
}
