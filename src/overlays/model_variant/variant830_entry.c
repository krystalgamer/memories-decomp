#include "../../types.h"
#include "variant830_entry.h"

/* Header 830's entry: the camera swings through three cues while the slot is
 * tinted and flashes white. After the delay a burst of sparks grows at the
 * opponent's position, a flickering glow rises from it, two sets of
 * expanding rings spread around it and debris is thrown up and falls back.
 * The colours fade every frame, and the entry ends once the light has come
 * back and the last spark has burnt out. */

static const VECTOR gModelVariant830_D2BF8 = {4096, 4096, 4096, 0};

s32 ModelVariant830_HandleRequest(u8 *context, s32 command)
{
    Model830State *work;
    MATRIX light, matrix;
    POLY_FT4 quad;
    POLY_GT4 glow;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX base;
    /* Never referenced, but it still takes the frame slot between the saved matrix and the flag. */
    SVECTOR spare;
    PSXLONG flag, interpolation;
    s32 slot, direction, step;
    GsOT *ot;
    s32 j, k;
    s32 depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant830_D2BF8;
    work = (Model830State *)context;
    slot = Model_GetActiveSlotIndex();
    direction = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant830_D2C78[command];
        for (j = 0; j < 64; j++) {
            setVector(&work->sparks[j], work->config->spark_spread * ((rand() - rand()) % 4096) / 4096,
                -(work->config->spark_spread / 2) * (rand() % 4096) / 4096,
                work->config->spark_spread * ((rand() - rand()) % 4096) / 4096);
            work->spark_frame[j] = 0;
        }
        for (j = 0; j < 16; j++) {
            setVector(&work->inner[j][0], work->config->inner_radius[0] * ccos(j * 256) / 4096, 0,
                work->config->inner_radius[0] * csin(j * 256) / 4096);
            setVector(&work->inner[j][1], work->config->inner_radius[1] * ccos(j * 256) / 4096,
                -work->config->inner_depth / 2, work->config->inner_radius[1] * csin(j * 256) / 4096);
            setVector(&work->inner[j][2], work->config->inner_radius[2] * ccos(j * 256) / 4096,
                -work->config->inner_depth, work->config->inner_radius[2] * csin(j * 256) / 4096);
            setVector(&work->outer[j][0], work->config->outer_radius[0] * ccos(j * 256) / 4096, 0,
                work->config->outer_radius[0] * csin(j * 256) / 4096);
            setVector(&work->outer[j][1], work->config->outer_radius[1] * ccos(j * 256) / 4096,
                work->config->outer_depth / 2, work->config->outer_radius[1] * csin(j * 256) / 4096);
            setVector(&work->outer[j][2], work->config->outer_radius[2] * ccos(j * 256) / 4096,
                work->config->outer_depth, work->config->outer_radius[2] * csin(j * 256) / 4096);
        }
        for (j = 0; j < 64; j++) {
            setVector(&work->debris[j], 0, 0, 0);
            setVector(&work->debris_velocities[j],
                work->config->debris_speed * ((rand() - rand()) % 4096) / 4096,
                -work->config->debris_speed * (rand() % 4096) / 4096,
                work->config->debris_speed * ((rand() - rand()) % 4096) / 4096);
        }
        for (j = 0; j < 4; j++) {
            work->texture[j] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant830_D2C08[j]);
        }
        work->glow.r = 255;
        work->glow.g = 255;
        work->glow.b = 255;
        work->ring.r = work->config->r;
        work->ring.g = work->config->g;
        work->ring.b = work->config->b;
        work->tint.r = 128;
        work->tint.g = 128;
        work->tint.b = 128;
        work->ring_scale = 0;
        work->level = 2048;
        work->phase = 0;
        work->elapsed = 0;
        work->tick = 0;
        work->count = 0;
        setVector(&work->pos, 0, 0, 0);
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->first_cue) {
        Model_QueueCameraOrbitFromPreset(slot, 2, &rotation, work->config->first_cue_arg);
    } else if (work->elapsed < work->config->second_cue) {
        if (!(work->tick & 1)) {
            Model_QueueTintRequestForAllParts(slot, 2, work->config->tint_start, work->config->tint_end, 4);
        }
        if (direction > 0) {
            Model_QueueCameraOrbitFromPreset(2, 12, &rotation, work->config->second_cue_arg);
        } else {
            Model_QueueCameraOrbitFromPreset(2, 1, &rotation, work->config->second_cue_arg);
        }
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 0, &rotation, work->config->final_cue_arg);
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
    Model_SetSlotAmbient(2, 2048, work->level, work->level);
    Model_SetSlotTintTarget(slot, 0, work->tint.r, work->tint.g, work->tint.b);
    if (work->elapsed >= work->config->flash_start) {
        if (work->tint.r < 240) {
            work->tint.r += 15;
        } else {
            work->tint.r = 255;
            if (!(work->tick & 1)) {
                work->tint.g = 255;
                work->tint.b = 255;
            } else {
                work->tint.g = 128;
                work->tint.b = 128;
            }
        }
    }
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->phase == 0) {
        Model_CopySlotPosition(slot ^ 1, (u16 *)&work->pos);
        work->pos.vy = 0;
        work->phase = 1;
    }
    GsSetLsMatrix(&light);
    RotTrans(&work->pos, (VECTOR *)matrix.t, &flag);
    RotMatrix(&rotation, &matrix);
    base = matrix;
    ScaleMatrix(&matrix, &scale);
    GsSetLsMatrix(&matrix);
    setPolyFT4(&quad);
    setRGB0(&quad, 128, 128, 128);
    quad.tpage = work->texture[1] >> 16;
    quad.clut = work->texture[1];
    for (j = 0; j < work->count; j++) {
        if (work->spark_frame[j] < 16) {
            /* Each branch draws and advances the frame itself; the shared tail is merged by the compiler. */
            if (work->spark_frame[j] < 2) {
                setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
                setVector(&vertices[1], work->config->spark_size, -work->config->spark_size, 0);
                setVector(&vertices[2], -work->config->spark_size, work->config->spark_size, 0);
                setVector(&vertices[3], work->config->spark_size, work->config->spark_size, 0);
                addVector(&vertices[0], &work->sparks[j]);
                addVector(&vertices[1], &work->sparks[j]);
                addVector(&vertices[2], &work->sparks[j]);
                addVector(&vertices[3], &work->sparks[j]);
                setUV4(&quad, 0, 0, 31, 0, 0, 31, 31, 31);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                work->spark_frame[j]++;
            } else {
                setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
                setVector(&vertices[1], 0, -work->config->spark_size, 0);
                setVector(&vertices[2], -work->config->spark_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->sparks[j]);
                addVector(&vertices[1], &work->sparks[j]);
                addVector(&vertices[2], &work->sparks[j]);
                addVector(&vertices[3], &work->sparks[j]);
                setUV4(&quad, work->spark_frame[j] / 2 % 4 * 32, work->spark_frame[j] / 8 * 32,
                    work->spark_frame[j] / 2 % 4 * 32 + 31, work->spark_frame[j] / 8 * 32,
                    work->spark_frame[j] / 2 % 4 * 32, work->spark_frame[j] / 8 * 32 + 30,
                    work->spark_frame[j] / 2 % 4 * 32 + 31, work->spark_frame[j] / 8 * 32 + 30);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                setVector(&vertices[0], work->config->spark_size, -work->config->spark_size, 0);
                setVector(&vertices[1], 0, -work->config->spark_size, 0);
                setVector(&vertices[2], work->config->spark_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->sparks[j]);
                addVector(&vertices[1], &work->sparks[j]);
                addVector(&vertices[2], &work->sparks[j]);
                addVector(&vertices[3], &work->sparks[j]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                setVector(&vertices[0], -work->config->spark_size, work->config->spark_size, 0);
                setVector(&vertices[1], 0, work->config->spark_size, 0);
                setVector(&vertices[2], -work->config->spark_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->sparks[j]);
                addVector(&vertices[1], &work->sparks[j]);
                addVector(&vertices[2], &work->sparks[j]);
                addVector(&vertices[3], &work->sparks[j]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                setVector(&vertices[0], work->config->spark_size, work->config->spark_size, 0);
                setVector(&vertices[1], 0, work->config->spark_size, 0);
                setVector(&vertices[2], work->config->spark_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->sparks[j]);
                addVector(&vertices[1], &work->sparks[j]);
                addVector(&vertices[2], &work->sparks[j]);
                addVector(&vertices[3], &work->sparks[j]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                work->spark_frame[j]++;
            }
        }
    }
    work->count += step;
    if (work->count > 64) {
        work->count = 64;
    }
    setPolyGT4(&glow);
    if ((work->glow.r != 0 || work->glow.g != 0 || work->glow.b != 0) && work->count >= 9) {
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
        setVector(&scale, 4096, work->ring_scale + 4096, 4096);
        matrix = base;
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&glow.x0,
                    (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &interpolation, &flag)
            - 128;
        setUV4(&glow, work->tick / 2 % 3 * 64, 64, work->tick / 2 % 3 * 64 + 63, 64, work->tick / 2 % 3 * 64,
            191, work->tick / 2 % 3 * 64 + 63, 191);
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
        }
        setUV4(&glow, (work->tick + 1) / 3 % 3 * 64, 64, (work->tick + 1) / 3 % 3 * 64 + 63, 64,
            (work->tick + 1) / 3 % 3 * 64, 191, (work->tick + 1) / 3 % 3 * 64 + 63, 191);
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
        }
        setUV4(&glow, (work->tick + 2) / 4 % 3 * 64, 64, (work->tick + 2) / 4 % 3 * 64 + 63, 64,
            (work->tick + 2) / 4 % 3 * 64, 191, (work->tick + 2) / 4 % 3 * 64 + 63, 191);
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
        }
        if (work->glow.r >= 10) work->glow.r -= work->config->fade_step;
        else work->glow.r = 0;
        if (work->glow.g >= 10) work->glow.g -= work->config->fade_step;
        else work->glow.g = 0;
        if (work->glow.b >= 10) work->glow.b -= work->config->fade_step;
        else work->glow.b = 0;
    }
    if (work->ring.r != 0 || work->ring.g != 0 || work->ring.b != 0) {
        setRGB0(&glow, 0, 0, 0);
        setRGB1(&glow, 0, 0, 0);
        setRGB2(&glow, work->ring.r >> 1, work->ring.g >> 1, work->ring.b >> 1);
        setRGB3(&glow, work->ring.r >> 1, work->ring.g >> 1, work->ring.b >> 1);
        glow.tpage = work->texture[0] >> 16;
        glow.clut = work->texture[0];
        setUV4(&glow, 0, 192, 31, 192, 0, 255, 31, 255);
        for (k = 0; k < 3; k++) {
            setVector(&scale, work->ring_scale / (k + 1) + 4096, work->ring_scale / (3 - k),
                work->ring_scale / (k + 1) + 4096);
            matrix = base;
            MulMatrix2(&light, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            for (j = 0; j < 16; j++) {
                depth = RotAverage4(&work->inner[j][0], &work->inner[(j + 1) % 16][0], &work->inner[j][1],
                    &work->inner[(j + 1) % 16][1], (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2,
                    (PSXLONG *)&glow.x3, &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
                }
                depth = RotAverage4(&work->inner[j][2], &work->inner[(j + 1) % 16][2], &work->inner[j][1],
                    &work->inner[(j + 1) % 16][1], (PSXLONG *)&glow.x0, (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2,
                    (PSXLONG *)&glow.x3, &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
                }
            }
        }
        setVector(&scale, (work->ring_scale >> 3) + 4096, work->ring_scale + 4096, (work->ring_scale >> 3) + 4096);
        matrix = base;
        MulMatrix2(&light, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        for (j = 0; j < 16; j++) {
            copyVector(&vertices[0], &work->outer[j][0]);
            copyVector(&vertices[1], &work->outer[(j + 1) % 16][0]);
            copyVector(&vertices[2], &work->outer[j][1]);
            copyVector(&vertices[3], &work->outer[(j + 1) % 16][1]);
            vertices[0].vy += -work->config->glow_height / 2;
            vertices[1].vy += -work->config->glow_height / 2;
            vertices[2].vy += -work->config->glow_height / 2;
            vertices[3].vy += -work->config->glow_height / 2;
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&glow.x0,
                (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
            }
            copyVector(&vertices[0], &work->outer[j][2]);
            copyVector(&vertices[1], &work->outer[(j + 1) % 16][2]);
            copyVector(&vertices[2], &work->outer[j][1]);
            copyVector(&vertices[3], &work->outer[(j + 1) % 16][1]);
            vertices[0].vy += -work->config->glow_height / 2;
            vertices[1].vy += -work->config->glow_height / 2;
            vertices[2].vy += -work->config->glow_height / 2;
            vertices[3].vy += -work->config->glow_height / 2;
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&glow.x0,
                (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
            }
        }
        setVector(&scale, (work->ring_scale >> 4) + 4096, work->ring_scale + 4096, (work->ring_scale >> 4) + 4096);
        matrix = base;
        MulMatrix2(&light, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        for (j = 0; j < 16; j++) {
            copyVector(&vertices[0], &work->outer[j][0]);
            copyVector(&vertices[1], &work->outer[(j + 1) % 16][0]);
            copyVector(&vertices[2], &work->outer[j][1]);
            copyVector(&vertices[3], &work->outer[(j + 1) % 16][1]);
            vertices[0].vy += -work->config->glow_height * 3 / 4;
            vertices[1].vy += -work->config->glow_height * 3 / 4;
            vertices[2].vy += -work->config->glow_height * 3 / 4;
            vertices[3].vy += -work->config->glow_height * 3 / 4;
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&glow.x0,
                (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
            }
            copyVector(&vertices[0], &work->outer[j][2]);
            copyVector(&vertices[1], &work->outer[(j + 1) % 16][2]);
            copyVector(&vertices[2], &work->outer[j][1]);
            copyVector(&vertices[3], &work->outer[(j + 1) % 16][1]);
            vertices[0].vy += -work->config->glow_height * 3 / 4;
            vertices[1].vy += -work->config->glow_height * 3 / 4;
            vertices[2].vy += -work->config->glow_height * 3 / 4;
            vertices[3].vy += -work->config->glow_height * 3 / 4;
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3], (PSXLONG *)&glow.x0,
                (PSXLONG *)&glow.x1, (PSXLONG *)&glow.x2, (PSXLONG *)&glow.x3, &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&glow, ot, (u16)depth, 1);
            }
        }
        setVector(&scale, 4096, 4096, 4096);
        matrix = base;
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        quad.tpage = work->texture[3] >> 16;
        quad.clut = work->texture[3];
        setRGB0(&quad, work->ring.r, work->ring.g, work->ring.b);
        setUV4(&quad, 32, 192, 63, 192, 32, 223, 63, 223);
        for (j = 0; j < 64; j++) {
            if (work->debris[j].vy <= 0) {
                setVector(&vertices[0], -work->config->debris_size, -work->config->debris_size, 0);
                setVector(&vertices[1], 0, -work->config->debris_size, 0);
                setVector(&vertices[2], -work->config->debris_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->debris[j]);
                addVector(&vertices[1], &work->debris[j]);
                addVector(&vertices[2], &work->debris[j]);
                addVector(&vertices[3], &work->debris[j]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                setVector(&vertices[0], work->config->debris_size, -work->config->debris_size, 0);
                setVector(&vertices[1], 0, -work->config->debris_size, 0);
                setVector(&vertices[2], work->config->debris_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->debris[j]);
                addVector(&vertices[1], &work->debris[j]);
                addVector(&vertices[2], &work->debris[j]);
                addVector(&vertices[3], &work->debris[j]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                setVector(&vertices[0], -work->config->debris_size, work->config->debris_size, 0);
                setVector(&vertices[1], 0, work->config->debris_size, 0);
                setVector(&vertices[2], -work->config->debris_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->debris[j]);
                addVector(&vertices[1], &work->debris[j]);
                addVector(&vertices[2], &work->debris[j]);
                addVector(&vertices[3], &work->debris[j]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                setVector(&vertices[0], work->config->debris_size, work->config->debris_size, 0);
                setVector(&vertices[1], 0, work->config->debris_size, 0);
                setVector(&vertices[2], work->config->debris_size, 0, 0);
                setVector(&vertices[3], 0, 0, 0);
                addVector(&vertices[0], &work->debris[j]);
                addVector(&vertices[1], &work->debris[j]);
                addVector(&vertices[2], &work->debris[j]);
                addVector(&vertices[3], &work->debris[j]);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2, (PSXLONG *)&quad.x3,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
                }
                addVector(&work->debris[j], &work->debris_velocities[j]);
                work->debris_velocities[j].vy += 2;
            }
        }
        if (work->ring.r >= 10) work->ring.r -= work->config->fade_step;
        else work->ring.r = 0;
        if (work->ring.g >= 10) work->ring.g -= work->config->fade_step;
        else work->ring.g = 0;
        if (work->ring.b >= 10) work->ring.b -= work->config->fade_step;
        else work->ring.b = 0;
    }
    if (work->ring_scale < 0x4000) {
        work->ring_scale += 0x800;
    } else if (work->ring_scale < 0x6000) {
        work->ring_scale += 0x80;
    }
    work->elapsed += step;
    work->tick++;
    PopMatrix();
    if (work->phase == 3 && work->spark_frame[work->count - 1] >= 16) {
        return 2;
    }
    if (work->phase == 1 && work->elapsed >= work->config->delay) {
        work->phase = 2;
        return 1;
    }
    return 0;
}
