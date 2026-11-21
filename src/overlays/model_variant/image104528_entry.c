#include "../../types.h"
#include "image104528_entry.h"

s32 FrenchModelVariant826_HandleRequest(ModelVariantSparkBurstState *context, s32 command)
{
    ModelVariantSparkBurstState *work;
    MATRIX light, matrix;
    POLY_FT4 quad;
    POLY_G3 triangle;
    ModelVariantVectorOrDimensions rotation;
    SVECTOR position;
    VECTOR scale;
    SVECTOR corners[4];
    MATRIX saved;
    long flag, p;
    s32 slot, other, step;
    GsOT *ot;
    s32 i, j;
    long depth;

    memset(&rotation.vector, 0, 8);
    memset(&position, 0, 8);
    scale = gFrenchModelVariant826_UnitScale;
    work = context;
    slot = Model_GetActiveSlotIndex();
    if (command >= 0) {
        work->config = &gFrenchModelVariant826_aConfigs[command];
        for (i = 0; i < 64; i++) {
            setVector(&work->sparks[i], 0, 0, 0);
            setVector(&work->bursts[i], 0, 0, 0);
            work->ages[i] = 0;
            work->red[i] = work->config->red;
            work->green[i] = work->config->green;
            work->blue[i] = work->config->blue;
            for (j = 0; j < 16; j++) {
                setVector(&work->tips[i][j],
                    work->config->spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->spread * ((rand() - rand()) % 4096) / 4096);
            }
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(
                Model_GetActiveSlotIndex(), 1, &gFrenchModelVariant826_aImages[i]);
        }
        work->level = 2048;
        work->spark_count = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->frame = 0;
        work->burst_count = 0;
        return 0;
    }

    ot = Graphics_GetDefaultOrderingTable();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->camera_time) {
        Model_QueueCameraOrbitFromPreset(slot, 0, &rotation.vector, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rotation.vector, -16);
    }
    if (work->spark_count == 0) {
        if (work->level > -512) {
            work->level -= step * 64;
        } else {
            work->level = -512;
        }
    } else if (work->phase == 1) {
        if (work->level < 2048) {
            work->level += step * 64;
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
    light = *Model_GetLightSourceMatrix();
    if (work->elapsed < work->config->end && work->spark_count < 64) {
        other = slot ^ 1;
        Model_CopySlotPosition(other, &position);
        Model_GetSlotDimensionsWithMinimums(other, &rotation.dimensions);
        position.vx += rotation.dimensions.x / 2 * ((rand() - rand()) % 4096) / 4096;
        position.vy += rotation.dimensions.y / 2 * ((rand() - rand()) % 4096) / 4096;
        position.vz += rotation.dimensions.z / 2 * ((rand() - rand()) % 4096) / 4096;
        copyVector(&work->sparks[work->spark_count], &position);
        work->spark_count++;
        if (work->frame % work->config->period == 0 && work->burst_count < 64) {
            Model_CopySlotPosition(other, &position);
            Model_GetSlotDimensionsWithMinimums(other, &rotation.dimensions);
            position.vx += rotation.dimensions.x / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vy += rotation.dimensions.y / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vz += rotation.dimensions.z / 2 * ((rand() - rand()) % 4096) / 4096;
            copyVector(&work->bursts[work->burst_count], &position);
            work->burst_count++;
        }
    }
    for (i = 0; i < work->spark_count; i++) {
        if (work->ages[i] < 8) {
            setPolyFT4(&quad);
            quad.tpage = work->texture[0] >> 16;
            quad.clut = work->texture[0];
            setRGB0(&quad, 255, 255, 255);
            setVector(&corners[0], -work->config->spark_size, -work->config->spark_size, 0);
            setVector(&corners[1], work->config->spark_size, -work->config->spark_size, 0);
            setVector(&corners[2], -work->config->spark_size, work->config->spark_size, 0);
            setVector(&corners[3], work->config->spark_size, work->config->spark_size, 0);
            setVector(&position, 0, 0, 0);
            setVector(&rotation.vector, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&light);
            RotTrans(&work->sparks[i], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation.vector, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                (long *)&quad.x0, (long *)&quad.x1, (long *)&quad.x2,
                (long *)&quad.x3, &p, &flag);
            if (work->ages[i] < 8) {
                setUV4(&quad, work->ages[i] % 4 * 64, 0,
                    work->ages[i] % 4 * 64 + 63, 0,
                    work->ages[i] % 4 * 64, 63,
                    work->ages[i] % 4 * 64 + 63, 63);
            } else {
                setUV4(&quad, work->ages[i] % 4 * 64, 64,
                    work->ages[i] % 4 * 64 + 63, 64,
                    work->ages[i] % 4 * 64, 127,
                    work->ages[i] % 4 * 64 + 63, 127);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
            }
            work->ages[i]++;
        }
    }
    for (i = 0; i < work->burst_count; i++) {
        if (work->red[i] || work->green[i] || work->blue[i]) {
            setPolyFT4(&quad);
            quad.tpage = work->texture[1] >> 16;
            quad.clut = work->texture[1];
            setRGB0(&quad, work->red[i], work->green[i], work->blue[i]);
            setVector(&corners[0], -work->config->burst_size, -work->config->burst_size, 0);
            setVector(&corners[1], work->config->burst_size, -work->config->burst_size, 0);
            setVector(&corners[2], -work->config->burst_size, work->config->burst_size, 0);
            setVector(&corners[3], work->config->burst_size, work->config->burst_size, 0);
            setVector(&position, 0, 0, 0);
            setVector(&rotation.vector, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&light);
            RotTrans(&work->bursts[i], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation.vector, &matrix);
            saved = matrix;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                (long *)&quad.x0, (long *)&quad.x1, (long *)&quad.x2,
                (long *)&quad.x3, &p, &flag);
            setUV4(&quad, 0, 128, 63, 128, 0, 191, 63, 191);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
            }
            setPolyG3(&triangle);
            setRGB0(&triangle, 0, 0, 0);
            setVector(&corners[0], -work->config->spoke_length, 0, 0);
            setVector(&corners[1], work->config->spoke_length, 0, 0);
            setRGB1(&triangle, work->red[i], work->green[i], work->blue[i]);
            setRGB2(&triangle, work->red[i], work->green[i], work->blue[i]);
            for (j = 0; j < 16; j++) {
                matrix = saved;
                GsSetLsMatrix(&light);
                setVector(&scale, 4096, 4096, 4096);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage3(&work->tips[i][j], &corners[0], &corners[1],
                    (long *)&triangle.x0, (long *)&triangle.x1,
                    (long *)&triangle.x2, &p, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&triangle, ot, (u16)depth, 1);
                }
                setVector(&scale, 8192, 8192, 8192);
                matrix = saved;
                GsSetLsMatrix(&light);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage3(&work->tips[i][j], &corners[0], &corners[1],
                    (long *)&triangle.x0, (long *)&triangle.x1,
                    (long *)&triangle.x2, &p, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&triangle, ot, (u16)depth, 3);
                }
            }
            if (work->red[i] > 15) work->red[i] -= work->config->fade;
            else work->red[i] = 0;
            if (work->green[i] > 15) work->green[i] -= work->config->fade;
            else work->green[i] = 0;
            if (work->blue[i] > 15) work->blue[i] -= work->config->fade;
            else work->blue[i] = 0;
        }
    }
    work->elapsed += step;
    work->frame++;
    PopMatrix();
    if (work->phase == 2) {
        return 2;
    }
    if (work->elapsed >= work->config->end && work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    if (work->elapsed >= work->config->delay && work->phase == 0) {
        return 4;
    }
    return 0;
}
