#include "../../types.h"
#include "variant761_entry.h"

/* Header 761's entry: a half ring of light opens around the slot, grows in
 * three widening passes and a spinning cone rises in its middle, while a
 * beam climbs upward and fades. The parts are tinted during a window, the
 * ambient level dips until the burst and the camera shakes until then. */

static const VECTOR gModelVariant761_D11DC = {4096, 4096, 4096, 0};
static const ModelTintColor gModelVariant761_D11EC = {0x60, 0x60, 0x60, 0};

s32 ModelVariant761_HandleRequest(u8 *context, s32 command)
{
    Model761State *work;
    MATRIX base, matrix;
    POLY_FT4 beam;
    POLY_FT4 ring;
    POLY_FT4 cone;
    SVECTOR rotation, position;
    VECTOR scale;
    ModelTintColor tint_start;
    ModelTintColor tint_end;
    PSXLONG flag, interpolation;
    s32 slot, side, step;
    GsOT *ot;
    s32 i;
    SVECTOR *v, *w;
    s32 sx, sy, sz;
    PSXLONG depth;

    memset(&rotation, 0, 8);
    memset(&position, 0, 8);
    scale = gModelVariant761_D11DC;
    tint_start = gModelVariant761_D11EC;
    memset(&tint_end, 0, 4);
    work = (Model761State *)context;
    slot = Model_GetActiveSlotIndex();
    side = slot * 2 - 1;
    if (command >= 0) {
        work->config = &gModelVariant761_D1244[command];
        v = &work->beam[0];
        setVector(v, 0, -work->config->beam_height, side * work->config->beam_width);
        v = &work->beam[1];
        setVector(v, 0, -work->config->beam_height, -side * work->config->beam_width);
        v = &work->beam[2];
        setVector(v, 0, work->config->beam_height, side * work->config->beam_width);
        v = &work->beam[3];
        setVector(v, 0, work->config->beam_height, -side * work->config->beam_width);
        setVector(&work->offset, 0, 0, 0);
        for (i = 0; i < 3; i++) {
            work->texture[i] = ModelTexture_PackPageClutCopy(slot, 1, &gModelVariant761_D11F0[i]);
        }
        Model_GetSlotDimensionsWithMinimums(-slot + 1, (ModelEffectAdjustment *)&position);
        v = work->inner;
        w = work->outer;
        if (position.vx > position.vz) {
            for (i = 0; i < 9; i++) {
                setVector(v, position.vx * (ccos(i << 8) % 4096) / 4096, 0,
                    side * position.vx * (csin(i << 8) % 4096) / 4096);
                setVector(w, position.vx * 2 * (ccos(i << 8) % 4096) / 4096,
                    -work->config->ring_depth, side * (position.vx << 1) * (csin(i << 8) % 4096) / 4096);
                v++;
                w++;
            }
        } else {
            for (i = 0; i < 9; i++) {
                setVector(v, position.vz * (ccos(i << 8) % 4096) / 4096, 0,
                    side * position.vz * (csin(i << 8) % 4096) / 4096);
                setVector(w, position.vz * 2 * (ccos(i << 8) % 4096) / 4096,
                    -work->config->ring_depth, side * (position.vz << 1) * (csin(i << 8) % 4096) / 4096);
                v++;
                w++;
            }
        }
        v = &work->cone[0];
        setVector(v, 0, -work->config->cone_height, 0);
        v = &work->cone[1];
        setVector(v, 0, -work->config->cone_height, side * work->config->cone_length);
        v = &work->cone[2];
        setVector(v, 0, 0, 0);
        v = &work->cone[3];
        setVector(v, 0, 0, side * work->config->cone_length);
        work->level = 2048;
        work->elapsed = 0;
        work->tick = 0;
        work->phase = 0;
        work->color.rgb.r = work->config->r;
        work->color.rgb.g = work->config->g;
        work->color.rgb.b = work->config->b;
        work->frame = 1;
        work->beam_scale = 0;
        return 0;
    }
    ot = func_80058F10();
    step = Model_GetFrameStep();
    Model_SetFrameStepOverride(1);
    if (work->elapsed >= work->config->tint_start && work->elapsed < work->config->tint_stop
        && !(work->tick & 1)) {
        Model_QueueTintRequestForAllParts(slot, 2, tint_start, tint_end, 8);
    }
    work->tick++;
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
            work->level = 2048;
            work->phase = 2;
        }
    }
    Model_SetSlotAmbient(2, work->level, work->level, work->level);
    if (work->elapsed < work->config->delay) {
        work->elapsed += step;
        return 0;
    }
    PushMatrix();
    base = *(MATRIX *)Model_GetLightSourceMatrix();
    if (work->frame < 64) {
        SetPolyFT4(&ring);
        v = work->inner;
        w = work->outer;
        setRGB0(&ring, work->color.rgb.r, work->color.rgb.g, work->color.rgb.b);
        ring.tpage = work->texture[1] >> 16;
        ring.clut = work->texture[1];
        setUV4(&ring, 128, 0, 128, 127, 255, 0, 255, 128);
        setVector(&position, 0, 0, side * 450);
        sz = sx = sy = (work->frame << 11) + 0x1000;
        if (sx > 0x7FCD) sx = 0x7FCD;
        if (sy > 0x7FCD) sy = 0x7FCD;
        if (sz > 0x7FCD) sz = 0x7FCD;
        setVector(&scale, sx, sy, sz);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        for (i = 0; i < 8; i++) {
            depth = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)depth, 1);
            }
        }
        v = work->inner;
        setVector(&position, 0, 0, side * 450);
        w = work->outer;
        sx = (work->frame << 9) + 0x1000;
        sy = work->frame * 3 * 1024 + 0x1000;
        sz = (work->frame << 9) + 0x1000;
        if (sx > 0x7FCD) sx = 0x7FCD;
        if (sy > 0x7FCD) sy = 0x7FCD;
        if (sz > 0x7FCD) sz = 0x7FCD;
        setVector(&scale, sx, sy, sz);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        for (i = 0; i < 8; i++) {
            depth = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)depth, 1);
            }
        }
        v = work->inner;
        setVector(&position, 0, 0, side * 450);
        w = work->outer;
        sx = (work->frame << 10) + 0x1000;
        sy = work->frame * 9 * 256 + 0x1000;
        sz = (work->frame << 10) + 0x1000;
        if (sx > 0x7FCD) sx = 0x7FCD;
        if (sy > 0x7FCD) sy = 0x7FCD;
        if (sz > 0x7FCD) sz = 0x7FCD;
        setVector(&scale, sx, sy, sz);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        for (i = 0; i < 8; i++) {
            depth = RotAverage4(&v[i], &v[i + 1], &w[i], &w[i + 1],
                (PSXLONG *)&ring.x0, (PSXLONG *)&ring.x1, (PSXLONG *)&ring.x2, (PSXLONG *)&ring.x3,
                &interpolation, &flag);
            if (depth >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)&ring, ot, (u16)depth, 1);
            }
        }
        SetPolyFT4(&cone);
        setRGB0(&cone, work->color.rgb.r, work->color.rgb.g, work->color.rgb.b);
        cone.tpage = work->texture[2] >> 16;
        cone.clut = work->texture[2];
        setUV4(&cone, 0, 128, 31, 128, 0, 191, 31, 191);
        v = work->cone;
        sz = sx = sy = (work->frame << 10) + 0x1000;
        if (sx > 0x7FCD) sx = 0x7FCD;
        if (sy > 0x7FCD) sy = 0x7FCD;
        if (sz > 0x7FCD) sz = 0x7FCD;
        setVector(&scale, sx, sy, sz);
        setVector(&position, 0, 0, side * 450);
        addVector(&position, &work->offset);
        position.vz -= side * work->config->cone_offset;
        setVector(&rotation, 0, 0, 256);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&v[0], &v[1], &v[2], &v[3],
            (PSXLONG *)&cone.x0, (PSXLONG *)&cone.x1, (PSXLONG *)&cone.x2, (PSXLONG *)&cone.x3,
            &interpolation, &flag);
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&cone, ot, (u16)depth, 1);
        }
        setVector(&rotation, 0, 0, -256);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&v[0], &v[1], &v[2], &v[3],
            (PSXLONG *)&cone.x0, (PSXLONG *)&cone.x1, (PSXLONG *)&cone.x2, (PSXLONG *)&cone.x3,
            &interpolation, &flag);
        if (depth >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&cone, ot, (u16)depth, 1);
        }
    }
    v = work->beam;
    work->frame++;
    if (work->color.word & 0xFFFFFF) {
        setPolyFT4(&beam);
        beam.tpage = work->texture[0] >> 16;
        beam.clut = work->texture[0];
        setRGB0(&beam, work->color.rgb.r, work->color.rgb.g, work->color.rgb.b);
        setVector(&position, 0, -350, side * 450);
        addVector(&position, &work->offset);
        setVector(&scale, 4096, work->beam_scale, 4096);
        GsSetLsMatrix(&base);
        RotTrans(&position, (VECTOR *)matrix.t, &flag);
        RotMatrix(&rotation, &matrix);
        MulMatrix2(&base, &matrix);
        ScaleMatrix(&matrix, &scale);
        GsSetLsMatrix(&matrix);
        depth = RotAverage4(&work->beam[2], v, &work->beam[3], &work->beam[1],
            (PSXLONG *)&beam.x0, (PSXLONG *)&beam.x1, (PSXLONG *)&beam.x2, (PSXLONG *)&beam.x3,
            &interpolation, &flag);
        setUV4(&beam, 0, 0, 127, 0, 0, 127, 127, 127);
        if (depth >= 0 && flag >= 0) {
            Graphics_SubmitDrawModePacket((u32 *)&beam, ot, (u16)depth, 1);
        }
        work->offset.vz += side * work->config->rise;
        if (work->beam_scale < 0x2000) {
            work->beam_scale += 0x800;
        } else {
            work->beam_scale += 0x80;
        }
        if ((u8)(work->color.rgb.r -= 12) < 15) work->color.rgb.r = 0;
        if ((u8)(work->color.rgb.g -= 12) < 15) work->color.rgb.g = 0;
        if ((u8)(work->color.rgb.b -= 12) < 15) work->color.rgb.b = 0;
    }
    if (work->elapsed < work->config->burst_time) {
        work->elapsed += step;
        Model_QueueCameraShake(32, 8);
        PopMatrix();
        return 1;
    }
    work->phase = 1;
    if (work->elapsed < work->config->end_time) {
        work->elapsed += step;
        PopMatrix();
        return 0;
    }
    PopMatrix();
    return 2;
}
