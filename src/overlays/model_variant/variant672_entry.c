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
    (e).color[0] = D_8013CB6C[work->command].color[(offset) + 0] * (value) / (divisor); \
    (e).color[1] = D_8013CB6C[work->command].color[(offset) + 1] * (value) / (divisor); \
    (e).color[2] = D_8013CB6C[work->command].color[(offset) + 2] * (value) / (divisor)

s32 func_8013B004(u8 *context, s32 command)
{
    Model672State *work;
    MATRIX ls, lw, lw2;
    GsCOORDUNIT unit;
    Model672ViewSnapshot view;
    SVECTOR head, middle;
    PSXLONG interpolation, flag, sxy;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 value, angle, sign, arc, turn;
    u32 tex;
    PSXLONG depth;

    work = (Model672State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model672State));
#ifndef VERSION_FRENCH
        printf(D_8013CB00, sizeof(Model672State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        Model_CopySlotPosition(work->slot == 0, (u16 *)&work->origin);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB18[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB18[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB18[1]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
            work->count = 64;
            break;
        }
        work->radius = D_8013CB6C[command].radius;
        work->height = D_8013CB6C[command].height;
        for (i = 0; i < work->count; i++) {
            work->sparks[i].duration = 32;
            work->sparks[i].active = 0;
            work->sparks[i].elapsed = 0;
            work->sparks[i].mode = 1;
            work->sparks[i].coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].color[0] = D_8013CB6C[command].color[0];
            work->sparks[i].color[1] = D_8013CB6C[command].color[1];
            work->sparks[i].color[2] = D_8013CB6C[command].color[2];
            setVector(&work->sparks[i].scale, 4096, 4096, 4096);
            value = work->radius * rsin((i << 12) / work->count) / 4096;
            sign = i % 2 * 2 - 1;
            setVector(&work->sparks[i].target,
                work->origin.vx + sign * value * rsin(angle = (i % 8) << 9) / 4096,
                work->origin.vy + value * rcos(angle) / 4096,
                work->origin.vz * 2);
            setVector(&work->sparks[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->sparks[i].position, 0, 0, 0);
            work->sparks[i].spawned = 0;
        }
        for (k = 0; k < work->count; k++) {
            work->flares[k].active = 0;
            work->flares[k].elapsed = 0;
            work->flares[k].duration = 32;
            work->flares[k].mode = 1;
            work->flares[k].coord = Model_GetCameraCoordinateUnit();
            work->flares[k].color[0] = D_8013CB6C[command].color[3];
            work->flares[k].color[1] = D_8013CB6C[command].color[4];
            work->flares[k].color[2] = D_8013CB6C[command].color[5];
            work->flares[k].u = 0;
            work->flares[k].v = 32;
            setVector(&work->flares[k].scale, 4096, 4096, 4096);
            setVector(&work->flares[k].rotation, 0, work->slot * 2048, 0);
            setVector(&work->flares[k].target, 0, 0, 0);
            value = work->radius * rsin((k << 12) / work->count) / 4096;
            sign = k % 2 * 2 - 1;
            setVector(&work->flares[k].position,
                work->origin.vx + sign * value * rsin(turn = (k % 8) << 9) / 4096,
                work->origin.vy + value * rcos(turn) / 4096,
                work->origin.vz + sign * value * rsin((k << 12) / work->count) / 4096);
        }
        for (i = 0; i < work->count; i++) {
            work->rings[i].duration = 32;
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].color[0] = D_8013CB6C[command].color[0];
            work->rings[i].color[1] = D_8013CB6C[command].color[1];
            work->rings[i].color[2] = D_8013CB6C[command].color[2];
            work->rings[i].u = 0;
            work->rings[i].v = 0;
            setVector(&work->rings[i].scale, 4096, 4096, 4096);
            setVector(&work->rings[i].target, 0, 0, work->height * 2);
            setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->rings[i].position, 0, 0, 0);
        }
        work->delay = D_8013CB6C[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        value = work->radius;
        setVector(&work->center, 0, 0, 0);
        for (i = 0; i < 17; i++) {
            setVector(&work->rim[i], value * rsin(i << 8) / 4096, value * rcos(i << 8) / 4096, 0);
        }
        value = work->radius >> 1;
        setVector(&work->flare[0], -value, -value, 0);
        setVector(&work->flare[1], value, -value, 0);
        setVector(&work->flare[2], -value, value, 0);
        setVector(&work->flare[3], value, value, 0);
        value = work->height;
        setVector(&work->ring[0], -value, -value, 0);
        setVector(&work->ring[1], value, -value, 0);
        setVector(&work->ring[2], -value, value, 0);
        setVector(&work->ring[3], value, value, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->command) {
    case 0:
        if (work->elapsed < 100 && work->flags[0] < work->count) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 4);
            GsGetLwUnit(&unit, &lw);
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 5);
            GsGetLwUnit(&unit, &lw2);
            (work->sparks + work->flags[0])->position.vx = (lw.t[0] + lw2.t[0]) / 2;
            (work->sparks + work->flags[0])->position.vy = (lw.t[1] + lw2.t[1]) / 2;
            (work->sparks + work->flags[0])->position.vz = (lw.t[2] + lw2.t[2]) / 2;
            work->sparks[work->flags[0]].active = 1;
            work->rings[work->flags[0]].position = work->sparks[work->flags[0]].position;
            work->rings[work->flags[0]].active = 1;
            work->flags[0]++;
        }
        break;
    case 1:
        if (work->elapsed < 200 && work->flags[0] < work->count) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 22);
            GsGetLwUnit(&unit, &lw);
            work->sparks[work->flags[0]].position.vx = lw.t[0];
            work->sparks[work->flags[0]].position.vy = lw.t[1];
            work->sparks[work->flags[0]].position.vz = lw.t[2];
            work->sparks[work->flags[0]].active = 1;
            work->rings[work->flags[0]].position = work->sparks[work->flags[0]].position;
            work->rings[work->flags[0]].active = 1;
            work->flags[0]++;
        }
        break;
    case 2:
        if (work->elapsed < 84 && work->flags[0] < work->count) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, (u8)(work->flags[0] % 5) * 3 + 9);
            GsGetLwUnit(&unit, &lw);
            (work->sparks + work->flags[0])->position.vx = lw.t[0];
            (work->sparks + work->flags[0])->position.vy = lw.t[1];
            (work->sparks + work->flags[0])->position.vz = lw.t[2];
            work->sparks[work->flags[0]].active = 1;
            work->rings[work->flags[0]].position = work->sparks[work->flags[0]].position;
            work->rings[work->flags[0]].active = 1;
            work->flags[0]++;
        }
        break;
    }
    PushMatrix();
    view = *(Model672ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->count; i++) {
        if (work->sparks[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->sparks[i].scale);
            unit.matrix.t[0] = work->sparks[i].position.vx;
            unit.matrix.t[1] = work->sparks[i].position.vy;
            unit.matrix.t[2] = work->sparks[i].position.vz;
            unit.super = work->sparks[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->sparks[i].elapsed > work->sparks[i].duration * 3 / 4) {
                value = (work->sparks[i].duration - work->sparks[i].elapsed) * 4;
                sign = work->sparks[i].duration;
                MODEL672_FADE(work->sparks[i], 0, value, sign);
            }
            work->line.attribute = 0x50000000;
            work->line.r0 = 0;
            work->line.g0 = 0;
            work->line.b0 = 0;
            work->line.r1 = work->sparks[i].color[0];
            work->line.g1 = work->sparks[i].color[1];
            work->line.b1 = work->sparks[i].color[2];
            setVector(&head,
                (work->sparks[i].target.vx - work->sparks[i].position.vx) * work->sparks[i].elapsed / work->sparks[i].duration,
                (work->sparks[i].target.vy - work->sparks[i].position.vy) * work->sparks[i].elapsed / work->sparks[i].duration,
                (work->sparks[i].target.vz - work->sparks[i].position.vz) * work->sparks[i].elapsed / work->sparks[i].duration);
            setVector(&middle, (head.vx + work->center.vx) / 2, (head.vy + work->center.vy) / 2,
                (head.vz + work->center.vz) / 2);
            RotTransPers(&work->center, (PSXLONG *)&work->line.x0, &interpolation, &flag);
            RotTransPers(&head, (PSXLONG *)&work->line.x1, &interpolation, &flag);
            depth = RotTransPers(&middle, &sxy, &interpolation, &flag);
            if (flag >= 0) {
                GsSortGLine(&work->line, func_80058F10(), (u16)depth);
            }
            work->sparks[i].elapsed += step;
            if (work->sparks[i].spawned == 0
                && work->sparks[i].duration * 2 / 3 < work->sparks[i].elapsed) {
                work->flares[i].active = 1;
                work->sparks[i].spawned = 1;
            }
            if (work->sparks[i].elapsed > work->sparks[i].duration) {
                work->sparks[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (k = 0; k < work->count; k++) {
        if (work->flares[k].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            value = work->flares[k].elapsed * 0x3000 / work->flares[k].duration + 0x1000;
            setVector(&work->flares[k].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->flares[k].scale);
            RotMatrixZ(work->flares[k].rotation.vz, &unit.matrix);
            RotMatrixX(work->flares[k].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
            angle = 0xC06;
            RotMatrixY(work->flares[k].rotation.vy + angle - view.yaw, &unit.matrix);
            unit.matrix.t[0] = work->flares[k].position.vx;
            unit.matrix.t[1] = work->flares[k].position.vy;
            unit.matrix.t[2] = work->flares[k].position.vz;
            unit.super = work->flares[k].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->flares[k].elapsed > (u32)work->flares[k].duration / 2) {
                value = (work->flares[k].duration - work->flares[k].elapsed) * 2;
                sign = work->flares[k].duration;
                MODEL672_FADE(work->flares[k], 3, value, sign);
            }
            setPolyFT4(&work->quad);
            work->quad.tpage = work->tpage[0];
            work->quad.clut = work->clut[0];
            setSemiTrans(&work->quad, 1);
            setUV4(&work->quad, work->flares[k].u, work->flares[k].v,
                work->flares[k].u + 63, work->flares[k].v,
                work->flares[k].u, work->flares[k].v + 63,
                work->flares[k].u + 63, work->flares[k].v + 63);
            setRGB0(&work->quad, work->flares[k].color[0], work->flares[k].color[1], work->flares[k].color[2]);
            depth = RotAverage4(&work->flare[0], &work->flare[1], &work->flare[2], &work->flare[3],
                (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
            }
            for (j = 0; j < 16; j++) {
                setPolyGT3(&work->fan);
                work->fan.tpage = work->tpage[1];
                work->fan.clut = work->clut[1];
                setSemiTrans(&work->fan, 1);
                setUV3(&work->fan, work->flares[k].u + 32, work->flares[k].v + 64,
                    work->flares[k].u, work->flares[k].v + 127,
                    work->flares[k].u + 63, work->flares[k].v + 127);
                setRGB0(&work->fan, work->flares[k].color[0], work->flares[k].color[1], work->flares[k].color[2]);
                setRGB1(&work->fan, 0, 0, 0);
                setRGB2(&work->fan, 0, 0, 0);
                depth = RotAverage3(&work->center, &work->rim[j], &work->rim[j + 1],
                    (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1, (PSXLONG *)&work->fan.x2,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
                }
            }
            work->flares[k].elapsed += step;
            if (work->flares[k].duration < work->flares[k].elapsed) {
                work->flares[k].active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->count; i++) {
        if (work->rings[i].active == 1) {
            if (work->rings[i].elapsed > (u32)work->rings[i].duration / 2) {
                value = (work->rings[i].duration - work->rings[i].elapsed) * 2;
                sign = work->rings[i].duration;
                MODEL672_FADE(work->rings[i], 0, value, sign);
            }
            for (j = 0; j < 3; j++) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->rings[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                angle = 0xC06;
                RotMatrixY(work->rings[i].rotation.vy + angle - view.yaw, &unit.matrix);
                arc = (j << 12) / 3 + i * 682;
                unit.matrix.t[0] = work->rings[i].position.vx
                    + work->rings[i].target.vz * rsin(arc) / 4096 * work->rings[i].elapsed / work->rings[i].duration;
                unit.matrix.t[1] = work->rings[i].position.vy
                    + work->rings[i].target.vz * rcos(arc) / 4096 * work->rings[i].elapsed / work->rings[i].duration;
                unit.matrix.t[2] = work->rings[i].position.vz
                    + work->direction * work->rings[i].target.vz * work->rings[i].elapsed / work->rings[i].duration;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->quad);
                work->quad.tpage = work->tpage[2];
                work->quad.clut = work->clut[2];
                setSemiTrans(&work->quad, 1);
                setUV4(&work->quad, work->rings[i].u, work->rings[i].v,
                    work->rings[i].u + 31, work->rings[i].v,
                    work->rings[i].u, work->rings[i].v + 31,
                    work->rings[i].u + 31, work->rings[i].v + 31);
                setRGB0(&work->quad, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                depth = RotAverage4(&work->ring[0], &work->ring[1], &work->ring[2], &work->ring[3],
                    (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                    (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                }
            }
            if (work->rings[i].u < 224) {
                work->rings[i].u += 32;
            }
            work->rings[i].elapsed += step;
            if (work->rings[i].duration < work->rings[i].elapsed) {
                work->rings[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    PopMatrix();
    work->elapsed += step;
    if (inactive == work->count * 3) {
        result = 1;
        if (work->flags[4] == 1) {
            result = 2;
        }
        work->flags[4] = 1;
    }
    return result;
}
