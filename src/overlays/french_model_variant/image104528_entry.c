#include "../../types.h"
#include "image104528_entry.h"

s32 func_8013B004(u8 *context, s32 command)
{
    Image104528State *work;
    MATRIX light, matrix;
    POLY_FT4 quad;
    POLY_G3 triangle;
    Image104528Rotation rotation;
    SVECTOR position;
    VECTOR scale;
    SVECTOR corners[4];
    MATRIX saved;
    PSXLONG flag, p;
    s32 slot, other, step;
    GsOT *ot;
    s32 i, j;
    PSXLONG depth;

    memset(&rotation.vector, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C0C8;
    work = (Image104528State *)context;
    slot = Model_GetActiveSlotIndex();
    if (command >= 0) {
        work->config = &D_8013C110[command];
        for (i = 0; i < 64; i++) {
            setVector(&work->flashes[i], 0, 0, 0);
            setVector(&work->bursts[i], 0, 0, 0);
            work->frames[i] = 0;
            work->red[i] = work->config->red;
            work->green[i] = work->config->green;
            work->blue[i] = work->config->blue;
            for (j = 0; j < 16; j++) {
                setVector(&work->offsets[i][j],
                    work->config->spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->spread * ((rand() - rand()) % 4096) / 4096,
                    work->config->spread * ((rand() - rand()) % 4096) / 4096);
            }
        }
        for (i = 0; i < 2; i++) {
            work->textures[i] = ModelTexture_PackPageClutCopy(
                Model_GetActiveSlotIndex(), 1, &D_8013C0D8[i]);
        }
        work->ambient = 2048;
        work->flash_count = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->updates = 0;
        work->burst_count = 0;
        return 0;
    }

    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->camera_time) {
        Model_QueueCameraOrbitFromPreset(slot, 0, &rotation.vector, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rotation.vector, -16);
    }
    if (work->flash_count == 0) {
        if (work->ambient > -512) {
            work->ambient -= step * 64;
        } else {
            work->ambient = -512;
        }
    } else if (work->phase == 1) {
        if (work->ambient < 2048) {
            work->ambient += step * 64;
        } else {
            work->phase = 2;
            work->ambient = 2048;
        }
    }
    Model_SetSlotAmbient(2, work->ambient, work->ambient, work->ambient);
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    light = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < work->config->spawn_end && work->flash_count < 64) {
        other = slot ^ 1;
        Model_CopySlotPosition(other, (u16 *)&position);
        Model_GetSlotDimensionsWithMinimums(other, &rotation.dimensions);
        position.vx += rotation.dimensions.x / 2 * ((rand() - rand()) % 4096) / 4096;
        position.vy += rotation.dimensions.y / 2 * ((rand() - rand()) % 4096) / 4096;
        position.vz += rotation.dimensions.z / 2 * ((rand() - rand()) % 4096) / 4096;
        copyVector(&work->flashes[work->flash_count], &position);
        work->flash_count++;
        if (work->updates % work->config->burst_interval == 0 && work->burst_count < 64) {
            Model_CopySlotPosition(other, (u16 *)&position);
            Model_GetSlotDimensionsWithMinimums(other, &rotation.dimensions);
            position.vx += rotation.dimensions.x / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vy += rotation.dimensions.y / 2 * ((rand() - rand()) % 4096) / 4096;
            position.vz += rotation.dimensions.z / 2 * ((rand() - rand()) % 4096) / 4096;
            copyVector(&work->bursts[work->burst_count], &position);
            work->burst_count++;
        }
    }
    for (i = 0; i < work->flash_count; i++) {
        if (work->frames[i] < 8) {
            setPolyFT4(&quad);
            quad.tpage = work->textures[0] >> 16;
            quad.clut = work->textures[0];
            setRGB0(&quad, 255, 255, 255);
            setVector(&corners[0], -work->config->flash_extent, -work->config->flash_extent, 0);
            setVector(&corners[1], work->config->flash_extent, -work->config->flash_extent, 0);
            setVector(&corners[2], -work->config->flash_extent, work->config->flash_extent, 0);
            setVector(&corners[3], work->config->flash_extent, work->config->flash_extent, 0);
            setVector(&position, 0, 0, 0);
            setVector(&rotation.vector, 0, 0, 0);
            setVector(&scale, 4096, 4096, 4096);
            GsSetLsMatrix(&light);
            RotTrans(&work->flashes[i], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation.vector, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&corners[0], &corners[1], &corners[2], &corners[3],
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &p, &flag);
            if (work->frames[i] < 8) {
                setUV4(&quad, work->frames[i] % 4 * 64, 0,
                    work->frames[i] % 4 * 64 + 63, 0,
                    work->frames[i] % 4 * 64, 63,
                    work->frames[i] % 4 * 64 + 63, 63);
            } else {
                setUV4(&quad, work->frames[i] % 4 * 64, 64,
                    work->frames[i] % 4 * 64 + 63, 64,
                    work->frames[i] % 4 * 64, 127,
                    work->frames[i] % 4 * 64 + 63, 127);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
            }
            work->frames[i]++;
        }
    }
    for (i = 0; i < work->burst_count; i++) {
        if (work->red[i] || work->green[i] || work->blue[i]) {
            setPolyFT4(&quad);
            quad.tpage = work->textures[1] >> 16;
            quad.clut = work->textures[1];
            setRGB0(&quad, work->red[i], work->green[i], work->blue[i]);
            setVector(&corners[0], -work->config->burst_extent, -work->config->burst_extent, 0);
            setVector(&corners[1], work->config->burst_extent, -work->config->burst_extent, 0);
            setVector(&corners[2], -work->config->burst_extent, work->config->burst_extent, 0);
            setVector(&corners[3], work->config->burst_extent, work->config->burst_extent, 0);
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
                (PSXLONG *)&quad.x0, (PSXLONG *)&quad.x1, (PSXLONG *)&quad.x2,
                (PSXLONG *)&quad.x3, &p, &flag);
            setUV4(&quad, 0, 128, 63, 128, 0, 191, 63, 191);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&quad, ot, (u16)depth, 1);
            }
            setPolyG3(&triangle);
            setRGB0(&triangle, 0, 0, 0);
            setVector(&corners[0], -work->config->fan_extent, 0, 0);
            setVector(&corners[1], work->config->fan_extent, 0, 0);
            setRGB1(&triangle, work->red[i], work->green[i], work->blue[i]);
            setRGB2(&triangle, work->red[i], work->green[i], work->blue[i]);
            for (j = 0; j < 16; j++) {
                matrix = saved;
                GsSetLsMatrix(&light);
                setVector(&scale, 4096, 4096, 4096);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage3(&work->offsets[i][j], &corners[0], &corners[1],
                    (PSXLONG *)&triangle.x0, (PSXLONG *)&triangle.x1,
                    (PSXLONG *)&triangle.x2, &p, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&triangle, ot, (u16)depth, 1);
                }
                setVector(&scale, 8192, 8192, 8192);
                matrix = saved;
                GsSetLsMatrix(&light);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage3(&work->offsets[i][j], &corners[0], &corners[1],
                    (PSXLONG *)&triangle.x0, (PSXLONG *)&triangle.x1,
                    (PSXLONG *)&triangle.x2, &p, &flag);
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
    work->updates++;
    PopMatrix();
    if (work->phase == 2) {
        return 2;
    }
    if (work->elapsed >= work->config->spawn_end && work->phase == 0) {
        work->phase = 1;
        return 1;
    }
    if (work->elapsed >= work->config->delay && work->phase == 0) {
        return 4;
    }
    return 0;
}
