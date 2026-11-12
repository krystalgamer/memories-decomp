#include "../../types.h"
#include "variant666_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 666's entry: three blooms hover in a circle above the model,
 * each a pulsing square that launches a spark on every pulse. A spark
 * draws a fading line from its bloom towards a target point and, a
 * quarter of the way through its life, opens a ring of eight textured
 * triangles round a centre square. The setup also prints the work-area
 * size. */

/* Fades COLOR[0..2] of element E from the command colour at OFFSET. */
#define MODEL666_FADE(e, offset, value, divisor)                                      \
    (e).head.color[0] = gModelVariant666_aConfigs[work->head.command].color[(offset) + 0] * (value) / (divisor); \
    (e).head.color[1] = gModelVariant666_aConfigs[work->head.command].color[(offset) + 1] * (value) / (divisor); \
    (e).head.color[2] = gModelVariant666_aConfigs[work->head.command].color[(offset) + 2] * (value) / (divisor)

s32 ModelVariant666_HandleRequest(u8 *context, s32 command)
{
    Model666State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshot view;
    SVECTOR tip, mid;
    long interpolation, flag, sxy;
    s32 result, j, step, inactive;
    s32 i, k;
    s32 value, sign, ended;
    u32 tex;
    long depth;

    work = (Model666State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model666State));
#ifndef VERSION_FRENCH
        printf(gModelVariant666_szWorkSizeFormat, sizeof(Model666State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        Model_CopySlotPosition(work->head.slot == 0, (u16 *)&work->origin);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant666_aImages[2]);
        work->textures.tpage[0] = tex >> 16;
        work->textures.clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant666_aImages[0]);
        work->textures.tpage[1] = tex >> 16;
        work->textures.clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant666_aImages[1]);
        work->textures.tpage[2] = tex >> 16;
        work->textures.clut[2] = tex;
        switch (command) {
        case 0:
            work->count = 3;
            work->sparks_per_bloom = 16;
            break;
        }
        work->place.radius = gModelVariant666_aConfigs[command].radius;
        work->place.height = gModelVariant666_aConfigs[command].height;
        for (i = 0; i < work->count; i++) {
            work->blooms[i].head.duration = 480;
            work->blooms[i].head.active = 0;
            work->blooms[i].head.elapsed = 0;
            work->blooms[i].head.mode = 1;
            work->blooms[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->blooms[i].frame = 0;
            work->blooms[i].head.color[0] = gModelVariant666_aConfigs[command].color[0];
            work->blooms[i].head.color[1] = gModelVariant666_aConfigs[command].color[1];
            work->blooms[i].head.color[2] = gModelVariant666_aConfigs[command].color[2];
            work->blooms[i].head.u = 0;
            work->blooms[i].head.v = 32;
            setVector(&work->blooms[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->blooms[i].target, 0, 0, 0);
            setVector(&work->blooms[i].xform.rotation, 0, work->head.slot * 2048, 0);
            value = work->place.radius;
            k = (i << 12) / 3 + 0x800;
            setVector(&work->blooms[i].xform.position,
                work->origin.vx + value * rsin(work->head.slot * 2048 + k) / 4096,
                work->origin.vy - work->place.height,
                work->origin.vz + value * rcos(work->head.slot * 2048 + k) / 4096);
            work->blooms[i].cursor = 0;
        }
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < work->sparks_per_bloom; i++) {
                work->sparks[i][j].head.duration = 32;
                work->sparks[i][j].head.active = 0;
                work->sparks[i][j].head.elapsed = 0;
                work->sparks[i][j].head.mode = 1;
                work->sparks[i][j].xform.coord = Model_GetCameraCoordinateUnit();
                work->sparks[i][j].head.color[0] = gModelVariant666_aConfigs[command].color[0];
                work->sparks[i][j].head.color[1] = gModelVariant666_aConfigs[command].color[1];
                work->sparks[i][j].head.color[2] = gModelVariant666_aConfigs[command].color[2];
                work->sparks[i][j].spawned = 0;
                setVector(&work->sparks[i][j].xform.scale, 4096, 4096, 4096);
                sign = i % 2 * 2 - 1;
                value = work->place.height;
                setVector(&work->sparks[i][j].target,
                    sign * value * rsin(i << 8) / 4096,
                    work->place.height * 3,
                    sign * value * rcos(i << 8) / 4096);
                setVector(&work->sparks[i][j].xform.rotation, 0, work->head.slot * 2048, 0);
                work->sparks[i][j].xform.position = work->blooms[j].xform.position;
            }
        }
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < work->sparks_per_bloom; i++) {
                work->rings[i][j].head.duration = 32;
                work->rings[i][j].head.active = 0;
                work->rings[i][j].head.elapsed = 0;
                work->rings[i][j].head.mode = 1;
                work->rings[i][j].xform.coord = Model_GetCameraCoordinateUnit();
                work->rings[i][j].head.color[0] = gModelVariant666_aConfigs[command].color[3];
                work->rings[i][j].head.color[1] = gModelVariant666_aConfigs[command].color[4];
                work->rings[i][j].head.color[2] = gModelVariant666_aConfigs[command].color[5];
                work->rings[i][j].head.u = 0;
                work->rings[i][j].head.v = 0;
                setVector(&work->rings[i][j].xform.scale, 4096, 4096, 4096);
                setVector(&work->rings[i][j].target, 0, 0, 0);
                setVector(&work->rings[i][j].xform.rotation, 0, work->head.slot * 2048, 0);
                sign = i % 2 * 2 - 1;
                value = work->place.height / 2;
                setVector(&work->rings[i][j].xform.position,
                    work->blooms[j].xform.position.vx + sign * value * rsin(i << 8) / 4096,
                    work->origin.vy + work->place.radius * rsin((i + j) % 8 << 9) / 4096,
                    work->blooms[j].xform.position.vz + sign * value * rcos(i << 8) / 4096);
            }
        }
        work->place.delay = gModelVariant666_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        value = work->place.radius;
        setVector(&work->flare[0], -value, -value, 0);
        setVector(&work->flare[1], value, -value, 0);
        setVector(&work->flare[2], -value, value, 0);
        setVector(&work->flare[3], value, value, 0);
        setVector(&work->root, 0, 0, 0);
        /* Reassigning the radius here keeps the original store order. */
        value = work->place.radius;
        for (i = 0; i < 9; i++) {
            setVector(&work->rim[i], value * rsin(i << 9) / 4096, value * rcos(i << 9) / 4096, 0);
        }
        value = work->place.radius / 2;
        setVector(&work->ring[0], -value, -value, 0);
        setVector(&work->ring[1], value, -value, 0);
        setVector(&work->ring[2], -value, value, 0);
        setVector(&work->ring[3], value, value, 0);
    }
    if (work->place.delay > 0) {
        work->place.delay -= step;
        return result;
    }
    switch (work->head.command) {
    case 0:
        if (work->head.elapsed == 0) {
            for (i = 0; i < work->count; i++) {
                work->blooms[i].head.active = 1;
            }
        }
        if (work->flags[4] == 0 && work->head.elapsed > 432) {
            result = 1;
            work->flags[4] = result;
        }
        break;
    }
    PushMatrix();
    view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->count; i++) {
        if (work->blooms[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            ended = 0;
            if (work->blooms[i].head.elapsed < (u16)(work->blooms[i].head.duration / 10)) {
                value = work->blooms[i].head.elapsed * 5 * 8192 / work->blooms[i].head.duration;
            } else {
                value = 4096;
                ended = 1;
            }
            if (!(work->blooms[i].frame & 1)) {
                value /= 2;
            }
            setVector(&work->blooms[i].xform.scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->blooms[i].xform.scale);
            RotMatrixZ(work->blooms[i].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->blooms[i].xform.rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
            RotMatrixY(work->blooms[i].xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
            unit.matrix.t[0] = work->blooms[i].xform.position.vx;
            unit.matrix.t[1] = work->blooms[i].xform.position.vy;
            unit.matrix.t[2] = work->blooms[i].xform.position.vz;
            unit.super = work->blooms[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->blooms[i].head.duration * 9 / 10 < work->blooms[i].head.elapsed) {
                value = (work->blooms[i].head.duration - work->blooms[i].head.elapsed) * 10;
                sign = work->blooms[i].head.duration;
                MODEL666_FADE(work->blooms[i], 0, value, sign);
                ended = 0;
            }
            setPolyFT4(&work->quad);
            work->quad.tpage = work->textures.tpage[0];
            work->quad.clut = work->textures.clut[0];
            setSemiTrans(&work->quad, 1);
            setUV4(&work->quad, work->blooms[i].head.u, work->blooms[i].head.v,
                work->blooms[i].head.u + 63, work->blooms[i].head.v,
                work->blooms[i].head.u, work->blooms[i].head.v + 63,
                work->blooms[i].head.u + 63, work->blooms[i].head.v + 63);
            setRGB0(&work->quad, work->blooms[i].head.color[0], work->blooms[i].head.color[1], work->blooms[i].head.color[2]);
            depth = RotAverage4(&work->flare[0], &work->flare[1], &work->flare[2], &work->flare[3],
                (long *)&work->quad.x0, (long *)&work->quad.x1,
                (long *)&work->quad.x2, (long *)&work->quad.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
            }
            if (ended == 1) {
                work->sparks[work->blooms[i].cursor][i].head.active = ended;
                work->sparks[work->blooms[i].cursor][i].head.elapsed = 0;
                work->sparks[work->blooms[i].cursor][i].head.color[0] = gModelVariant666_aConfigs[work->head.command].color[0];
                work->sparks[work->blooms[i].cursor][i].head.color[1] = gModelVariant666_aConfigs[work->head.command].color[1];
                work->sparks[work->blooms[i].cursor][i].head.color[2] = gModelVariant666_aConfigs[work->head.command].color[2];
                work->sparks[work->blooms[i].cursor][i].spawned = 0;
                if (++work->blooms[i].cursor >= work->sparks_per_bloom) {
                    work->blooms[i].cursor = 0;
                }
            }
            work->blooms[i].head.elapsed += step;
            work->blooms[i].frame++;
            if (work->blooms[i].head.duration < work->blooms[i].head.elapsed) {
                work->blooms[i].head.active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (j = 0; j < work->count; j++) {
        for (i = 0; i < work->sparks_per_bloom; i++) {
            if (work->sparks[i][j].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->sparks[i][j].xform.scale);
                RotMatrixZ(work->sparks[i][j].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->sparks[i][j].xform.rotation.vx, &unit.matrix);
                RotMatrixY(work->sparks[i][j].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->sparks[i][j].xform.position.vx;
                unit.matrix.t[1] = work->sparks[i][j].xform.position.vy;
                unit.matrix.t[2] = work->sparks[i][j].xform.position.vz;
                unit.super = work->sparks[i][j].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->sparks[i][j].head.elapsed > work->sparks[i][j].head.duration * 3 / 4) {
                    value = (work->sparks[i][j].head.duration - work->sparks[i][j].head.elapsed) * 4;
                    sign = work->sparks[i][j].head.duration;
                    MODEL666_FADE(work->sparks[i][j], 0, value, sign);
                }
                work->line.attribute = 0x50000000;
                work->line.r0 = 0;
                work->line.g0 = 0;
                work->line.b0 = 0;
                work->line.r1 = work->sparks[i][j].head.color[0];
                work->line.g1 = work->sparks[i][j].head.color[1];
                work->line.b1 = work->sparks[i][j].head.color[2];
                if (work->sparks[i][j].head.elapsed < work->sparks[i][j].head.duration * 3 / 4) {
                    value = work->sparks[i][j].head.elapsed * 4;
                    sign = work->sparks[i][j].head.duration * 3;
                    tip.vx = work->sparks[i][j].target.vx * value / sign;
                    tip.vy = work->sparks[i][j].target.vy * value / sign;
                    tip.vz = work->sparks[i][j].target.vz * value / sign;
                } else {
                    tip.vx = work->sparks[i][j].target.vx;
                    tip.vy = work->sparks[i][j].target.vy;
                    tip.vz = work->sparks[i][j].target.vz;
                }
                mid.vx = (work->root.vx + tip.vx) / 2;
                mid.vy = (work->root.vy + tip.vy) / 2;
                mid.vz = (work->root.vz + tip.vz) / 2;
                RotTransPers(&work->root, (long *)&work->line.x0, &interpolation, &flag);
                RotTransPers(&tip, (long *)&work->line.x1, &interpolation, &flag);
                depth = RotTransPers(&mid, &sxy, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortGLine(&work->line, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                if (work->sparks[i][j].head.duration / 4 < work->sparks[i][j].head.elapsed && work->sparks[i][j].spawned == 0) {
                    work->rings[i][j].head.active = 1;
                    work->rings[i][j].head.elapsed = 0;
                    work->rings[i][j].head.color[0] = gModelVariant666_aConfigs[work->head.command].color[3];
                    work->rings[i][j].head.color[1] = gModelVariant666_aConfigs[work->head.command].color[4];
                    work->rings[i][j].head.color[2] = gModelVariant666_aConfigs[work->head.command].color[5];
                    work->rings[i][j].head.u = 0;
                    work->sparks[i][j].spawned = 1;
                }
                work->sparks[i][j].head.elapsed += step;
                if (work->sparks[i][j].head.duration < work->sparks[i][j].head.elapsed) {
                    work->sparks[i][j].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
    }
    for (j = 0; j < work->count; j++) {
        for (i = 0; i < work->sparks_per_bloom; i++) {
            if (work->rings[i][j].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                value = (work->rings[i][j].head.elapsed << 13) / work->rings[i][j].head.duration + 0x800;
                setVector(&work->rings[i][j].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->rings[i][j].xform.scale);
                RotMatrixZ(work->rings[i][j].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->rings[i][j].xform.rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                RotMatrixY(work->rings[i][j].xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->rings[i][j].xform.position.vx;
                unit.matrix.t[1] = work->rings[i][j].xform.position.vy;
                unit.matrix.t[2] = work->rings[i][j].xform.position.vz;
                unit.super = work->rings[i][j].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->rings[i][j].head.duration / 2 < work->rings[i][j].head.elapsed) {
                    value = (work->rings[i][j].head.duration - work->rings[i][j].head.elapsed) * 2;
                    sign = work->rings[i][j].head.duration;
                    MODEL666_FADE(work->rings[i][j], 3, value, sign);
                }
                for (k = 0; k < 8; k++) {
                    setPolyFT3(&work->fan);
                    work->fan.tpage = work->textures.tpage[1];
                    work->fan.clut = work->textures.clut[1];
                    setSemiTrans(&work->fan, 1);
                    setUV3(&work->fan, 32, work->rings[i][j].head.v + 96,
                        0, work->rings[i][j].head.v + 159,
                        63, work->rings[i][j].head.v + 159);
                    setRGB0(&work->fan, work->rings[i][j].head.color[0], work->rings[i][j].head.color[1], work->rings[i][j].head.color[2]);
                    depth = RotAverage3(&work->root, &work->rim[k], &work->rim[k + 1],
                        (long *)&work->fan.x0, (long *)&work->fan.x1, (long *)&work->fan.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->fan, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                setPolyFT4(&work->quad);
                work->quad.tpage = work->textures.tpage[2];
                work->quad.clut = work->textures.clut[2];
                setSemiTrans(&work->quad, 1);
                setUV4(&work->quad, work->rings[i][j].head.u, work->rings[i][j].head.v,
                    work->rings[i][j].head.u + 31, work->rings[i][j].head.v,
                    work->rings[i][j].head.u, work->rings[i][j].head.v + 31,
                    work->rings[i][j].head.u + 31, work->rings[i][j].head.v + 31);
                setRGB0(&work->quad, work->rings[i][j].head.color[0], work->rings[i][j].head.color[1], work->rings[i][j].head.color[2]);
                depth = RotAverage4(&work->ring[0], &work->ring[1], &work->ring[2], &work->ring[3],
                    (long *)&work->quad.x0, (long *)&work->quad.x1,
                    (long *)&work->quad.x2, (long *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                if (work->rings[i][j].head.u < 224) {
                    work->rings[i][j].head.u += 32;
                }
                work->rings[i][j].head.elapsed += step;
                if (work->rings[i][j].head.duration < work->rings[i][j].head.elapsed) {
                    work->rings[i][j].head.active = 0;
                }
                if (work->flags[4] == 0) {
                    result = 4;
                }
            } else {
                inactive++;
            }
        }
    }
    PopMatrix();
    work->head.elapsed += step;
    if (inactive == work->count * (work->sparks_per_bloom * 2 + 1)) {
        result = 2;
    }
    return result;
}
