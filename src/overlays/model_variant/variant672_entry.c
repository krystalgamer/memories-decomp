#include "../../types.h"
#include "variant672_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 672's entry: sparks streak from model joints towards points
 * scattered round the effect origin, each spawning a growing flare
 * (a square and a sixteen-slice fan) and three ring sprites that drift
 * outwards. The setup also prints the work-area size. */

/* Fades COLOR[0..2] of element E from the command colour at OFFSET. */
#define MODEL672_FADE(e, offset, value, divisor)                                      \
    (e).head.color[0] = gModelVariant672_aConfigs[work->head.command].color[(offset) + 0] * (value) / (divisor); \
    (e).head.color[1] = gModelVariant672_aConfigs[work->head.command].color[(offset) + 1] * (value) / (divisor); \
    (e).head.color[2] = gModelVariant672_aConfigs[work->head.command].color[(offset) + 2] * (value) / (divisor)

s32 ModelVariant672_HandleRequest(u8 *context, s32 command)
{
    Model672State *work;
    MATRIX ls, lw, lw2;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshot view;
    SVECTOR head, middle;
    long interpolation, flag, sxy;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 value, angle, sign, arc, turn;
    u32 tex;
    long depth;

    work = (Model672State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model672State));
#ifndef VERSION_FRENCH
        printf(gModelVariant672_szWorkSizeFormat, sizeof(Model672State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        Model_CopySlotPosition(work->head.slot == 0, (u16 *)&work->origin);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant672_aImages[2]);
        work->textures.tpage[0] = tex >> 16;
        work->textures.clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant672_aImages[0]);
        work->textures.tpage[1] = tex >> 16;
        work->textures.clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant672_aImages[1]);
        work->textures.tpage[2] = tex >> 16;
        work->textures.clut[2] = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
            work->count = 64;
            break;
        }
        work->place.radius = gModelVariant672_aConfigs[command].radius;
        work->place.height = gModelVariant672_aConfigs[command].height;
        for (i = 0; i < work->count; i++) {
            work->sparks[i].head.duration = 32;
            work->sparks[i].head.active = 0;
            work->sparks[i].head.elapsed = 0;
            work->sparks[i].head.mode = 1;
            work->sparks[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].head.color[0] = gModelVariant672_aConfigs[command].color[0];
            work->sparks[i].head.color[1] = gModelVariant672_aConfigs[command].color[1];
            work->sparks[i].head.color[2] = gModelVariant672_aConfigs[command].color[2];
            setVector(&work->sparks[i].xform.scale, 4096, 4096, 4096);
            value = work->place.radius * rsin((i << 12) / work->count) / 4096;
            sign = i % 2 * 2 - 1;
            setVector(&work->sparks[i].target,
                work->origin.vx + sign * value * rsin(angle = (i % 8) << 9) / 4096,
                work->origin.vy + value * rcos(angle) / 4096,
                work->origin.vz * 2);
            setVector(&work->sparks[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->sparks[i].xform.position, 0, 0, 0);
            work->sparks[i].spawned = 0;
        }
        for (k = 0; k < work->count; k++) {
            work->flares[k].head.active = 0;
            work->flares[k].head.elapsed = 0;
            work->flares[k].head.duration = 32;
            work->flares[k].head.mode = 1;
            work->flares[k].xform.coord = Model_GetCameraCoordinateUnit();
            work->flares[k].head.color[0] = gModelVariant672_aConfigs[command].color[3];
            work->flares[k].head.color[1] = gModelVariant672_aConfigs[command].color[4];
            work->flares[k].head.color[2] = gModelVariant672_aConfigs[command].color[5];
            work->flares[k].head.u = 0;
            work->flares[k].head.v = 32;
            setVector(&work->flares[k].xform.scale, 4096, 4096, 4096);
            setVector(&work->flares[k].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->flares[k].target, 0, 0, 0);
            value = work->place.radius * rsin((k << 12) / work->count) / 4096;
            sign = k % 2 * 2 - 1;
            setVector(&work->flares[k].xform.position,
                work->origin.vx + sign * value * rsin(turn = (k % 8) << 9) / 4096,
                work->origin.vy + value * rcos(turn) / 4096,
                work->origin.vz + sign * value * rsin((k << 12) / work->count) / 4096);
        }
        for (i = 0; i < work->count; i++) {
            work->rings[i].head.duration = 32;
            work->rings[i].head.active = 0;
            work->rings[i].head.elapsed = 0;
            work->rings[i].head.mode = 1;
            work->rings[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->rings[i].head.color[0] = gModelVariant672_aConfigs[command].color[0];
            work->rings[i].head.color[1] = gModelVariant672_aConfigs[command].color[1];
            work->rings[i].head.color[2] = gModelVariant672_aConfigs[command].color[2];
            work->rings[i].head.u = 0;
            work->rings[i].head.v = 0;
            setVector(&work->rings[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->rings[i].target, 0, 0, work->place.height * 2);
            setVector(&work->rings[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->rings[i].xform.position, 0, 0, 0);
        }
        work->place.delay = gModelVariant672_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        value = work->place.radius;
        setVector(&work->center, 0, 0, 0);
        for (i = 0; i < 17; i++) {
            setVector(&work->rim[i], value * rsin(i << 8) / 4096, value * rcos(i << 8) / 4096, 0);
        }
        value = work->place.radius >> 1;
        setVector(&work->flare[0], -value, -value, 0);
        setVector(&work->flare[1], value, -value, 0);
        setVector(&work->flare[2], -value, value, 0);
        setVector(&work->flare[3], value, value, 0);
        value = work->place.height;
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
        if (work->head.elapsed < 100 && work->flags[0] < work->count) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 4);
            GsGetLwUnit(&unit, &lw);
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 5);
            GsGetLwUnit(&unit, &lw2);
            (work->sparks + work->flags[0])->xform.position.vx = (lw.t[0] + lw2.t[0]) / 2;
            (work->sparks + work->flags[0])->xform.position.vy = (lw.t[1] + lw2.t[1]) / 2;
            (work->sparks + work->flags[0])->xform.position.vz = (lw.t[2] + lw2.t[2]) / 2;
            work->sparks[work->flags[0]].head.active = 1;
            work->rings[work->flags[0]].xform.position = work->sparks[work->flags[0]].xform.position;
            work->rings[work->flags[0]].head.active = 1;
            work->flags[0]++;
        }
        break;
    case 1:
        if (work->head.elapsed < 200 && work->flags[0] < work->count) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 22);
            GsGetLwUnit(&unit, &lw);
            work->sparks[work->flags[0]].xform.position.vx = lw.t[0];
            work->sparks[work->flags[0]].xform.position.vy = lw.t[1];
            work->sparks[work->flags[0]].xform.position.vz = lw.t[2];
            work->sparks[work->flags[0]].head.active = 1;
            work->rings[work->flags[0]].xform.position = work->sparks[work->flags[0]].xform.position;
            work->rings[work->flags[0]].head.active = 1;
            work->flags[0]++;
        }
        break;
    case 2:
        if (work->head.elapsed < 84 && work->flags[0] < work->count) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, (u8)(work->flags[0] % 5) * 3 + 9);
            GsGetLwUnit(&unit, &lw);
            (work->sparks + work->flags[0])->xform.position.vx = lw.t[0];
            (work->sparks + work->flags[0])->xform.position.vy = lw.t[1];
            (work->sparks + work->flags[0])->xform.position.vz = lw.t[2];
            work->sparks[work->flags[0]].head.active = 1;
            work->rings[work->flags[0]].xform.position = work->sparks[work->flags[0]].xform.position;
            work->rings[work->flags[0]].head.active = 1;
            work->flags[0]++;
        }
        break;
    }
    PushMatrix();
    view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->count; i++) {
        if (work->sparks[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            ScaleMatrix(&unit.matrix, &work->sparks[i].xform.scale);
            unit.matrix.t[0] = work->sparks[i].xform.position.vx;
            unit.matrix.t[1] = work->sparks[i].xform.position.vy;
            unit.matrix.t[2] = work->sparks[i].xform.position.vz;
            unit.super = work->sparks[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->sparks[i].head.elapsed > work->sparks[i].head.duration * 3 / 4) {
                value = (work->sparks[i].head.duration - work->sparks[i].head.elapsed) * 4;
                sign = work->sparks[i].head.duration;
                MODEL672_FADE(work->sparks[i], 0, value, sign);
            }
            work->line.attribute = 0x50000000;
            work->line.r0 = 0;
            work->line.g0 = 0;
            work->line.b0 = 0;
            work->line.r1 = work->sparks[i].head.color[0];
            work->line.g1 = work->sparks[i].head.color[1];
            work->line.b1 = work->sparks[i].head.color[2];
            setVector(&head,
                (work->sparks[i].target.vx - work->sparks[i].xform.position.vx) * work->sparks[i].head.elapsed / work->sparks[i].head.duration,
                (work->sparks[i].target.vy - work->sparks[i].xform.position.vy) * work->sparks[i].head.elapsed / work->sparks[i].head.duration,
                (work->sparks[i].target.vz - work->sparks[i].xform.position.vz) * work->sparks[i].head.elapsed / work->sparks[i].head.duration);
            setVector(&middle, (head.vx + work->center.vx) / 2, (head.vy + work->center.vy) / 2,
                (head.vz + work->center.vz) / 2);
            RotTransPers(&work->center, (long *)&work->line.x0, &interpolation, &flag);
            RotTransPers(&head, (long *)&work->line.x1, &interpolation, &flag);
            depth = RotTransPers(&middle, &sxy, &interpolation, &flag);
            if (flag >= 0) {
                GsSortGLine(&work->line, Graphics_GetDefaultOrderingTable(), (u16)depth);
            }
            work->sparks[i].head.elapsed += step;
            if (work->sparks[i].spawned == 0
                && work->sparks[i].head.duration * 2 / 3 < work->sparks[i].head.elapsed) {
                work->flares[i].head.active = 1;
                work->sparks[i].spawned = 1;
            }
            if (work->sparks[i].head.elapsed > work->sparks[i].head.duration) {
                work->sparks[i].head.active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (k = 0; k < work->count; k++) {
        if (work->flares[k].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            value = work->flares[k].head.elapsed * 0x3000 / work->flares[k].head.duration + 0x1000;
            setVector(&work->flares[k].xform.scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->flares[k].xform.scale);
            RotMatrixZ(work->flares[k].xform.rotation.vz, &unit.matrix);
            RotMatrixX(work->flares[k].xform.rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
            angle = 0xC06;
            RotMatrixY(work->flares[k].xform.rotation.vy + angle - view.yaw, &unit.matrix);
            unit.matrix.t[0] = work->flares[k].xform.position.vx;
            unit.matrix.t[1] = work->flares[k].xform.position.vy;
            unit.matrix.t[2] = work->flares[k].xform.position.vz;
            unit.super = work->flares[k].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->flares[k].head.elapsed > (u32)work->flares[k].head.duration / 2) {
                value = (work->flares[k].head.duration - work->flares[k].head.elapsed) * 2;
                sign = work->flares[k].head.duration;
                MODEL672_FADE(work->flares[k], 3, value, sign);
            }
            setPolyFT4(&work->quad);
            work->quad.tpage = work->textures.tpage[0];
            work->quad.clut = work->textures.clut[0];
            setSemiTrans(&work->quad, 1);
            setUV4(&work->quad, work->flares[k].head.u, work->flares[k].head.v,
                work->flares[k].head.u + 63, work->flares[k].head.v,
                work->flares[k].head.u, work->flares[k].head.v + 63,
                work->flares[k].head.u + 63, work->flares[k].head.v + 63);
            setRGB0(&work->quad, work->flares[k].head.color[0], work->flares[k].head.color[1], work->flares[k].head.color[2]);
            depth = RotAverage4(&work->flare[0], &work->flare[1], &work->flare[2], &work->flare[3],
                (long *)&work->quad.x0, (long *)&work->quad.x1,
                (long *)&work->quad.x2, (long *)&work->quad.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
            }
            for (j = 0; j < 16; j++) {
                setPolyGT3(&work->fan);
                work->fan.tpage = work->textures.tpage[1];
                work->fan.clut = work->textures.clut[1];
                setSemiTrans(&work->fan, 1);
                setUV3(&work->fan, work->flares[k].head.u + 32, work->flares[k].head.v + 64,
                    work->flares[k].head.u, work->flares[k].head.v + 127,
                    work->flares[k].head.u + 63, work->flares[k].head.v + 127);
                setRGB0(&work->fan, work->flares[k].head.color[0], work->flares[k].head.color[1], work->flares[k].head.color[2]);
                setRGB1(&work->fan, 0, 0, 0);
                setRGB2(&work->fan, 0, 0, 0);
                depth = RotAverage3(&work->center, &work->rim[j], &work->rim[j + 1],
                    (long *)&work->fan.x0, (long *)&work->fan.x1, (long *)&work->fan.x2,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->fan, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
            }
            work->flares[k].head.elapsed += step;
            if (work->flares[k].head.duration < work->flares[k].head.elapsed) {
                work->flares[k].head.active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->count; i++) {
        if (work->rings[i].head.active == 1) {
            if (work->rings[i].head.elapsed > (u32)work->rings[i].head.duration / 2) {
                value = (work->rings[i].head.duration - work->rings[i].head.elapsed) * 2;
                sign = work->rings[i].head.duration;
                MODEL672_FADE(work->rings[i], 0, value, sign);
            }
            for (j = 0; j < 3; j++) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->rings[i].xform.scale);
                RotMatrixZ(work->rings[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->rings[i].xform.rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                angle = 0xC06;
                RotMatrixY(work->rings[i].xform.rotation.vy + angle - view.yaw, &unit.matrix);
                arc = (j << 12) / 3 + i * 682;
                unit.matrix.t[0] = work->rings[i].xform.position.vx
                    + work->rings[i].target.vz * rsin(arc) / 4096 * work->rings[i].head.elapsed / work->rings[i].head.duration;
                unit.matrix.t[1] = work->rings[i].xform.position.vy
                    + work->rings[i].target.vz * rcos(arc) / 4096 * work->rings[i].head.elapsed / work->rings[i].head.duration;
                unit.matrix.t[2] = work->rings[i].xform.position.vz
                    + work->direction * work->rings[i].target.vz * work->rings[i].head.elapsed / work->rings[i].head.duration;
                unit.super = work->rings[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->quad);
                work->quad.tpage = work->textures.tpage[2];
                work->quad.clut = work->textures.clut[2];
                setSemiTrans(&work->quad, 1);
                setUV4(&work->quad, work->rings[i].head.u, work->rings[i].head.v,
                    work->rings[i].head.u + 31, work->rings[i].head.v,
                    work->rings[i].head.u, work->rings[i].head.v + 31,
                    work->rings[i].head.u + 31, work->rings[i].head.v + 31);
                setRGB0(&work->quad, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                depth = RotAverage4(&work->ring[0], &work->ring[1], &work->ring[2], &work->ring[3],
                    (long *)&work->quad.x0, (long *)&work->quad.x1,
                    (long *)&work->quad.x2, (long *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
            }
            if (work->rings[i].head.u < 224) {
                work->rings[i].head.u += 32;
            }
            work->rings[i].head.elapsed += step;
            if (work->rings[i].head.duration < work->rings[i].head.elapsed) {
                work->rings[i].head.active = 0;
            }
        } else {
            inactive++;
        }
    }
    PopMatrix();
    work->head.elapsed += step;
    if (inactive == work->count * 3) {
        result = 1;
        if (work->flags[4] == 1) {
            result = 2;
        }
        work->flags[4] = 1;
    }
    return result;
}
