#include "../../types.h"
#include "variant809_entry.h"

/* Header 809's entry: sparks gather around the opposing slot while the
 * ambient light dims, and every period a burst of coloured spokes appears
 * there and fades out until the light recovers. */

static const VECTOR gModelVariant809_D10C8 = {4096, 4096, 4096, 0};

#define RANDOM_SPREAD(range) ((range) * ((rand() - rand()) % 4096) / 4096)

s32 ModelVariant809_HandleRequest(u8 *context, s32 command)
{
    Model809State *work;
    MATRIX base, matrix;
    POLY_FT4 packet;
    POLY_G3 spoke_packet;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    MATRIX saved;
    PSXLONG flag, interpolation;
    s32 step;
    GsOT *ot;
    s32 slot;
    s32 i, j;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant809_D10C8;
    work = (Model809State *)context;
    slot = Model_GetActiveSlotIndex();
    if (command >= 0) {
        work->config = &gModelVariant809_D1110[command];
        for (i = 0; i < 64; i++) {
            setVector(&work->sparks[i], 0, 0, 0);
            setVector(&work->bursts[i], 0, 0, 0);
            work->ages[i] = 0;
            work->red[i] = work->config->red;
            work->green[i] = work->config->green;
            work->blue[i] = work->config->blue;
            for (j = 0; j < 16; j++) {
                setVector(&work->tips[i][j], RANDOM_SPREAD(work->config->spread),
                    RANDOM_SPREAD(work->config->spread), RANDOM_SPREAD(work->config->spread));
            }
        }
        for (i = 0; i < 2; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &gModelVariant809_D10D8[i]);
        }
        work->level = 2048;
        work->spark_count = 0;
        work->phase = 0;
        work->elapsed = 0;
        work->frame = 0;
        work->burst_count = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->camera_time) {
        Model_QueueCameraOrbitFromPreset(slot, 0, &rotation, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rotation, -16);
    }
    if (work->spark_count == 0) {
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
    if (work->elapsed < work->config->end && work->spark_count < 64) {
        Model_CopySlotPosition(slot ^ 1, (u16 *)&position);
        Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rotation);
        position.vx += RANDOM_SPREAD(rotation.vx / 2);
        position.vy += RANDOM_SPREAD(rotation.vy / 2);
        position.vz += RANDOM_SPREAD(rotation.vz / 2);
        setVector(&work->sparks[work->spark_count], position.vx, position.vy, position.vz);
        work->spark_count++;
        if (work->frame % work->config->period == 0 && work->burst_count < 64) {
            Model_CopySlotPosition(slot ^ 1, (u16 *)&position);
            Model_GetSlotDimensionsWithMinimums(slot ^ 1, (ModelEffectAdjustment *)&rotation);
            position.vx += RANDOM_SPREAD(rotation.vx / 2);
            position.vy += RANDOM_SPREAD(rotation.vy / 2);
            position.vz += RANDOM_SPREAD(rotation.vz / 2);
            setVector(&work->bursts[work->burst_count], position.vx, position.vy, position.vz);
            work->burst_count++;
        }
    }
    for (i = 0; i < work->spark_count; i++) {
        if (work->ages[i] < 8) {
            setPolyFT4(&packet);
            packet.tpage = work->texture[0] >> 16;
            packet.clut = work->texture[0];
            setRGB0(&packet, 255, 255, 255);
            setVector(&vertices[0], -work->config->spark_size, -work->config->spark_size, 0);
            setVector(&vertices[1], work->config->spark_size, -work->config->spark_size, 0);
            setVector(&vertices[2], -work->config->spark_size, work->config->spark_size, 0);
            setVector(&vertices[3], work->config->spark_size, work->config->spark_size, 0);
            setVector(&position, 0, 0, 0);
            setVector(&rotation, 0, 0, 0);
            scale.vx = 4096;
            scale.vy = 4096;
            scale.vz = 4096;
            GsSetLsMatrix(&base);
            RotTrans(&work->sparks[i], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&packet.x0, (PSXLONG *)&packet.x1, (PSXLONG *)&packet.x2,
                (PSXLONG *)&packet.x3, &interpolation, &flag);
            if (work->ages[i] < 8) {
                setUV4(&packet, work->ages[i] % 4 * 64, 0, work->ages[i] % 4 * 64 + 63, 0,
                    work->ages[i] % 4 * 64, 63, work->ages[i] % 4 * 64 + 63, 63);
            } else {
                setUV4(&packet, work->ages[i] % 4 * 64, 64, work->ages[i] % 4 * 64 + 63, 64,
                    work->ages[i] % 4 * 64, 127, work->ages[i] % 4 * 64 + 63, 127);
            }
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&packet, ot, (u16)depth, 1);
            }
            work->ages[i]++;
        }
    }
    for (i = 0; i < work->burst_count; i++) {
        if (work->red[i] != 0 || work->green[i] != 0 || work->blue[i] != 0) {
            setPolyFT4(&packet);
            packet.tpage = work->texture[1] >> 16;
            packet.clut = work->texture[1];
            setRGB0(&packet, work->red[i], work->green[i], work->blue[i]);
            setVector(&vertices[0], -work->config->burst_size, -work->config->burst_size, 0);
            setVector(&vertices[1], work->config->burst_size, -work->config->burst_size, 0);
            setVector(&vertices[2], -work->config->burst_size, work->config->burst_size, 0);
            setVector(&vertices[3], work->config->burst_size, work->config->burst_size, 0);
            setVector(&position, 0, 0, 0);
            setVector(&rotation, 0, 0, 0);
            scale.vx = 4096;
            scale.vy = 4096;
            scale.vz = 4096;
            GsSetLsMatrix(&base);
            RotTrans(&work->bursts[i], (VECTOR *)matrix.t, &flag);
            RotMatrix(&rotation, &matrix);
            saved = matrix;
            ScaleMatrix(&matrix, &scale);
            GsSetLsMatrix(&matrix);
            depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                (PSXLONG *)&packet.x0, (PSXLONG *)&packet.x1, (PSXLONG *)&packet.x2,
                (PSXLONG *)&packet.x3, &interpolation, &flag);
            setUV4(&packet, 0, 128, 63, 128, 0, 191, 63, 191);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&packet, ot, (u16)depth, 1);
            }
            setPolyG3(&spoke_packet);
            setRGB0(&spoke_packet, 0, 0, 0);
            setVector(&vertices[0], -work->config->spoke_length, 0, 0);
            setVector(&vertices[1], work->config->spoke_length, 0, 0);
            setRGB1(&spoke_packet, work->red[i], work->green[i], work->blue[i]);
            setRGB2(&spoke_packet, work->red[i], work->green[i], work->blue[i]);
            for (j = 0; j < 16; j++) {
                matrix = saved;
                GsSetLsMatrix(&base);
                scale.vx = 4096;
                scale.vy = 4096;
                scale.vz = 4096;
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage3(&work->tips[i][j], &vertices[0], &vertices[1],
                    (PSXLONG *)&spoke_packet.x0, (PSXLONG *)&spoke_packet.x1, (PSXLONG *)&spoke_packet.x2,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&spoke_packet, ot, (u16)depth, 1);
                }
                scale.vx = 8192;
                scale.vy = 8192;
                scale.vz = 8192;
                matrix = saved;
                GsSetLsMatrix(&base);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage3(&work->tips[i][j], &vertices[0], &vertices[1],
                    (PSXLONG *)&spoke_packet.x0, (PSXLONG *)&spoke_packet.x1, (PSXLONG *)&spoke_packet.x2,
                    &interpolation, &flag);
                if (depth >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&spoke_packet, ot, (u16)depth, 3);
                }
            }
            if (work->red[i] >= 16) {
                work->red[i] -= work->config->fade;
            } else {
                work->red[i] = 0;
            }
            if (work->green[i] >= 16) {
                work->green[i] -= work->config->fade;
            } else {
                work->green[i] = 0;
            }
            if (work->blue[i] >= 16) {
                work->blue[i] -= work->config->fade;
            } else {
                work->blue[i] = 0;
            }
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
