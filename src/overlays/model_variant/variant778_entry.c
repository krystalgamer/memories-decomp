#include "../../types.h"
#include "variant778_entry.h"
#include "../../game/screen_projection.h"

/* Header 778's entry: smoke puffs rise from random model parts while the
 * ambient light dims; then glows swirl around the opposing slot and rings of
 * spokes expand and fade there until the light recovers. */

static const VECTOR D_8013C4B4 = {4096, 4096, 4096, 0};

s32 func_8013B004(u8 *context, s32 command)
{
    Model778State *work;
    MATRIX base, matrix;
    POLY_FT4 smoke_packet, glow_packet, ring_packet;
    POLY_G3 spoke_packet;
    SVECTOR rotation, position;
    VECTOR scale;
    SVECTOR vertices[4];
    PSXLONG flag, interpolation;
    s32 slot, step;
    GsOT *ot;
    SVECTOR *point;
    s32 i, j;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = D_8013C4B4;
    work = (Model778State *)context;
    slot = Model_GetActiveSlotIndex();
    if (command >= 0) {
        work->config = &D_8013C518[command];
        point = work->smoke;
        for (i = 0; i < 25; i++) {
            setVector(point, 0, 0, 0);
            point++;
            work->smoke_ages[i] = 0;
        }
        point = work->glows;
        for (i = 0; i < 128; point++, i++) {
            point->vx = work->config->radius * ccos(rand() % 4096) / 4096;
            point->vy = rand() % work->config->spread - rand() % work->config->spread;
            point->vz = work->config->radius * csin(rand() % 4096) / 4096;
            work->glow_ages[i] = 0;
        }
        point = &work->axis[0];
        setVector(point, -work->config->ring_height, 0, 0);
        point = &work->axis[1];
        setVector(point, work->config->ring_height, 0, 0);
        for (i = 0; i < 32; i++) {
            setVector(&work->rings[i], work->config->radius / 2 * ccos(rand() % 4096) / 4096,
                rand() % work->config->spread - rand() % work->config->spread,
                work->config->radius / 2 * csin(rand() % 4096) / 4096);
            work->alpha[i] = 0x80;
            work->scales[i] = 4096;
            for (j = 0; j < work->config->segments; j++) {
                setVector(&work->tips[i][j], work->config->ring_spread * (rand() % 4096 - rand() % 4096) / 4096,
                    work->config->ring_spread * (rand() % 4096 - rand() % 4096) / 4096, 0);
            }
        }
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(Model_GetActiveSlotIndex(), 1, &D_8013C4C4[i]);
        }
        work->level = 2048;
        work->phase = 0;
        work->elapsed = 0;
        work->frame = 0;
        work->head = 0;
        work->glow_count = 0;
        work->ring_count = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed < work->config->glow_time) {
        Model_QueueCameraOrbitFromPreset(slot, 0, &rotation, -16);
    } else {
        Model_QueueCameraOrbitFromPreset(slot ^ 1, 1, &rotation, -16);
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
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->elapsed < work->config->end
        && (work->elapsed <= work->config->pause_start || work->elapsed > work->config->pause_end)) {
        GsSetLsMatrix(&base);
        /* One call per branch; cross-jumping merges them but keeps the slot reloads. */
        if (rand() % 4 == 0) {
            GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[0]), &matrix);
        } else if (rand() % 4 == 1) {
            GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[1]), &matrix);
        } else if (rand() % 4 == 2) {
            GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[2]), &matrix);
        } else {
            GsGetLwUnit(Model_GetSlotDataEntry(slot, work->config->parts[3]), &matrix);
        }
        setVector(&work->smoke[work->head], matrix.t[0], matrix.t[1], matrix.t[2]);
        work->smoke_ages[work->head] = 1;
        work->head = (work->head + 1) % 25;
    }
    if (work->phase >= 0) {
        setPolyFT4(&smoke_packet);
        smoke_packet.tpage = work->texture[1] >> 16;
        smoke_packet.clut = work->texture[1];
        setRGB0(&smoke_packet, 64, 64, 64);
        point = work->smoke;
        for (i = 0; i < 25; point++, i++) {
            if (work->smoke_ages[i] > 0 && work->smoke_ages[i] < 18) {
                GsSetLsMatrix(&base);
                setVector(&vertices[0], -work->config->smoke_size, -work->config->smoke_size, 0);
                setVector(&vertices[1], work->config->smoke_size, -work->config->smoke_size, 0);
                setVector(&vertices[2], -work->config->smoke_size, work->config->smoke_size, 0);
                setVector(&vertices[3], work->config->smoke_size, work->config->smoke_size, 0);
                copyVector(&position, point);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&smoke_packet.x0, (PSXLONG *)&smoke_packet.x1, (PSXLONG *)&smoke_packet.x2,
                    (PSXLONG *)&smoke_packet.x3, &interpolation, &flag);
                /* The s16 casts keep the sign extensions before and after the divide. */
                setUV4(&smoke_packet, (s16)((s16)work->smoke_ages[i] / 3) * 32 - 32, 192,
                    (s16)((s16)work->smoke_ages[i] / 3) * 32 - 1, 192,
                    (s16)((s16)work->smoke_ages[i] / 3) * 32 - 32, 223,
                    (s16)((s16)work->smoke_ages[i] / 3) * 32 - 1, 223);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&smoke_packet, ot, (u16)depth, 1);
                }
                work->smoke_ages[i]++;
            }
        }
    }
    if (work->config->glow_time < work->elapsed) {
        setPolyFT4(&glow_packet);
        glow_packet.tpage = work->texture[0] >> 16;
        glow_packet.clut = work->texture[0];
        setRGB0(&glow_packet, 128, 128, 128);
        for (i = 0; i < work->glow_count; i++) {
            if (work->glow_ages[i] < 8) {
                setVector(&vertices[0], -work->config->glow_size, -work->config->glow_size, 0);
                setVector(&vertices[1], work->config->glow_size, -work->config->glow_size, 0);
                setVector(&vertices[2], -work->config->glow_size, work->config->glow_size, 0);
                setVector(&vertices[3], work->config->glow_size, work->config->glow_size, 0);
                GsSetLsMatrix(&base);
                Model_CopySlotPosition(-slot + 1, (u16 *)&position);
                addVector(&position, &work->glows[i]);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&glow_packet.x0, (PSXLONG *)&glow_packet.x1, (PSXLONG *)&glow_packet.x2,
                    (PSXLONG *)&glow_packet.x3, &interpolation, &flag);
                /* "* 64" keeps the (s16) extension that "<< 6" drops for a byte store. */
                setUV4(&glow_packet, (s16)(work->glow_ages[i] % 4) * 64, work->glow_ages[i] / 4 << 6,
                    ((s16)(work->glow_ages[i] % 4) * 64) + 63, work->glow_ages[i] / 4 << 6,
                    (s16)(work->glow_ages[i] % 4) * 64, (work->glow_ages[i] / 4 << 6) + 63,
                    ((s16)(work->glow_ages[i] % 4) * 64) + 63, (work->glow_ages[i] / 4 << 6) + 63);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&glow_packet, ot, (u16)depth, 1);
                }
                work->glow_ages[i]++;
            }
        }
        if (work->phase < 2) {
            if (work->glow_count++ > 128) {
                work->glow_count = 128;
            }
        }
        setPolyFT4(&ring_packet);
        setPolyG3(&spoke_packet);
        ring_packet.tpage = work->texture[2] >> 16;
        ring_packet.clut = work->texture[2];
        setVector(&vertices[0], -work->config->ring_size, -work->config->ring_size, 0);
        setVector(&vertices[1], work->config->ring_size, -work->config->ring_size, 0);
        setVector(&vertices[2], -work->config->ring_size, work->config->ring_size, 0);
        setVector(&vertices[3], work->config->ring_size, work->config->ring_size, 0);
        for (i = 0; i < work->ring_count; i++) {
            if (work->alpha[i] != 0) {
                setRGB0(&ring_packet, work->alpha[i], work->alpha[i], work->alpha[i]);
                GsSetLsMatrix(&base);
                scale.vx = work->scales[i];
                scale.vy = work->scales[i];
                scale.vz = work->scales[i];
                Model_CopySlotPosition(-slot + 1, (u16 *)&position);
                addVector(&position, &work->rings[i]);
                RotTrans(&position, (VECTOR *)matrix.t, &flag);
                RotMatrix(&rotation, &matrix);
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&ring_packet.x0, (PSXLONG *)&ring_packet.x1, (PSXLONG *)&ring_packet.x2,
                    (PSXLONG *)&ring_packet.x3, &interpolation, &flag);
                setUV4(&ring_packet, 0, 128, 63, 128, 0, 191, 63, 191);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ring_packet, ot, (u16)depth, 1);
                }
                setRGB0(&spoke_packet, 0, 0, 0);
                setRGB1(&spoke_packet, work->alpha[i], work->alpha[i], work->alpha[i]);
                setRGB2(&spoke_packet, work->alpha[i], work->alpha[i], work->alpha[i]);
                /* The axis pointer is hoisted into a saved register, spilling the tip row. */
                point = work->axis;
                for (j = 0; j < work->config->segments; j++) {
                    depth = RotAverage3(&work->tips[i][j], point, point + 1,
                        (PSXLONG *)&spoke_packet.x0, (PSXLONG *)&spoke_packet.x1, (PSXLONG *)&spoke_packet.x2,
                        &interpolation, &flag);
                    if (depth >= 0 && flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&spoke_packet, ot, (u16)depth, 1);
                    }
                }
                scale.vx = work->scales[i] / 2;
                scale.vy = work->scales[i] / 2;
                scale.vz = work->scales[i] / 2;
                ScaleMatrix(&matrix, &scale);
                GsSetLsMatrix(&matrix);
                depth = RotAverage4(&vertices[0], &vertices[1], &vertices[2], &vertices[3],
                    (PSXLONG *)&ring_packet.x0, (PSXLONG *)&ring_packet.x1, (PSXLONG *)&ring_packet.x2,
                    (PSXLONG *)&ring_packet.x3, &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&ring_packet, ot, (u16)depth, 1);
                }
                if (work->scales[i] > 0x400C) {
                    work->scales[i] += 0x100;
                    if (work->alpha[i] > 15) {
                        work->alpha[i] -= 15;
                    } else {
                        work->alpha[i] = 0;
                    }
                } else {
                    work->scales[i] += 0x800;
                }
            }
        }
        if (!(work->frame & 7) && work->phase < 2) {
            if (++work->ring_count > 32) {
                work->ring_count = 32;
            }
        }
    }
    work->elapsed += step;
    work->frame++;
    PopMatrix();
    if (work->phase == 3) {
        return 2;
    }
    if (work->phase == 0 && work->elapsed >= work->config->end) {
        work->phase = 2;
        return 1;
    }
    return work->phase == 0 ? 4 : 0;
}
