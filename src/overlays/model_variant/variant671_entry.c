#include "../../types.h"
#include "variant671_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 671's entry: blooms open at a model joint, each a spinning
 * square with a cone of Gouraud-shaded petals, and each spawns a ring
 * of sixteen fading bands round a centre sprite. The setup also prints
 * the work-area size. */

/* Fades COLOR[0..2] of element E from the command colour at OFFSET. */
#define MODEL671_FADE(e, offset, value, divisor)                                      \
    (e).color[0] = gModelVariant671_D16E0[work->command].color[(offset) + 0] * (value) / (divisor); \
    (e).color[1] = gModelVariant671_D16E0[work->command].color[(offset) + 1] * (value) / (divisor); \
    (e).color[2] = gModelVariant671_D16E0[work->command].color[(offset) + 2] * (value) / (divisor)

s32 ModelVariant671_HandleRequest(u8 *context, s32 command)
{
    Model671State *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    Model671ViewSnapshot view;
    SVECTOR pts[3];
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 value, sign;
    u32 tex;
    PSXLONG depth;

    work = (Model671State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model671State));
#ifndef VERSION_FRENCH
        printf(gModelVariant671_D1674, sizeof(Model671State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        Model_CopySlotPosition(work->slot == 0, (u16 *)&work->origin);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant671_D168C[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant671_D168C[2]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant671_D168C[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
            work->count = 32;
            break;
        }
        work->radius = gModelVariant671_D16E0[command].radius;
        work->height = gModelVariant671_D16E0[command].height;
        for (i = 0; i < work->count; i++) {
            work->blooms[i].duration = 48;
            work->blooms[i].active = 0;
            work->blooms[i].elapsed = 0;
            work->blooms[i].mode = 1;
            work->blooms[i].coord = Model_GetCameraCoordinateUnit();
            work->blooms[i].color[0] = gModelVariant671_D16E0[command].color[0];
            work->blooms[i].color[1] = gModelVariant671_D16E0[command].color[1];
            work->blooms[i].color[2] = gModelVariant671_D16E0[command].color[2];
            work->blooms[i].u = 0;
            work->blooms[i].v = 0;
            setVector(&work->blooms[i].scale, 4096, 4096, 4096);
            setVector(&work->blooms[i].target, 0, 0, 0);
            setVector(&work->blooms[i].rotation, 0, work->slot * 2048, (i % 2 * 2 - 1) * (i << 12) / work->count);
            setVector(&work->blooms[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->rings[i].duration = 64;
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].mode = 2;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].color[0] = gModelVariant671_D16E0[command].color[3];
            work->rings[i].color[1] = gModelVariant671_D16E0[command].color[4];
            work->rings[i].color[2] = gModelVariant671_D16E0[command].color[5];
            work->rings[i].u = 0;
            work->rings[i].v = 128;
            setVector(&work->rings[i].scale, 4096, 4096, 4096);
            setVector(&work->rings[i].target, 0, 0, 0);
            setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
            sign = work->radius;
            value = i % 2 * 2 - 1;
            setVector(&work->rings[i].position,
                work->origin.vx + value * sign * rsin((i << 12) / work->count) / 4096,
                work->origin.vy + value * -sign * rcos((i << 12) / work->count) / 4096,
                work->origin.vz + value * sign * rcos((i << 12) / work->count) / 4096);
        }
        work->delay = gModelVariant671_D16E0[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        value = work->radius;
        sign = work->height;
        setVector(&work->flare[0], -value, -value, 0);
        setVector(&work->flare[1], value, -value, 0);
        setVector(&work->flare[2], -value, value, 0);
        setVector(&work->flare[3], value, value, 0);
        value = work->radius / 4;
        setVector(&pts[0], value, 0, 0);
        setVector(&pts[1], -value, 0, 0);
        setVector(&pts[2], 0, sign, 0);
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                PushMatrix();
                m = D_800FE168;
                RotMatrixZ(i << 9, &m);
                RotMatrixX((i + j) % 8 << 9, &m);
                RotMatrixY((i + j) % 8 << 9, &m);
                for (k = 0; k < 3; k++) {
                    ApplyMatrixSV(&m, &pts[k], &work->grid[k][j][i]);
                }
                PopMatrix();
            }
        }
        value = work->radius;
        sign = value * 3;
        for (i = 0; i < 17; i++) {
            setVector(&work->rim[0][i], value * rsin(i << 8) / 4096, value * rcos(i << 8) / 4096, 0);
            setVector(&work->rim[1][i], sign * rsin(i << 8) / 4096, sign * rcos(i << 8) / 4096, 0);
        }
        value = work->radius * 2;
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
        if (work->elapsed < 328 && work->elapsed >= work->flags[0] * 30 && work->flags[0] < work->count) {
            work->blooms[work->flags[0]].active = 1;
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 13);
            GsGetLwUnit(&unit, &ls);
            (work->blooms + work->flags[0])->position.vx = ls.t[0];
            (work->blooms + work->flags[0])->position.vy = ls.t[1];
            (work->blooms + work->flags[0])->position.vz = ls.t[2];
            work->rings[work->flags[0]].active = 1;
            work->flags[0]++;
        }
        break;
    }
    PushMatrix();
    view = *(Model671ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->count; i++) {
        if (work->blooms[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            value = work->blooms[i].elapsed * 0x3000 / work->blooms[i].duration + 0x800;
            setVector(&work->blooms[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->blooms[i].scale);
            RotMatrixZ(0, &unit.matrix);
            RotMatrixX(work->blooms[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
            RotMatrixY(work->blooms[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
            unit.matrix.t[0] = work->blooms[i].position.vx;
            unit.matrix.t[1] = work->blooms[i].position.vy;
            unit.matrix.t[2] = work->blooms[i].position.vz;
            unit.super = work->blooms[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->blooms[i].elapsed > (u16)(work->blooms[i].duration / 3)) {
                value = (work->blooms[i].duration - work->blooms[i].elapsed) * 3;
                sign = work->blooms[i].duration * 2;
                MODEL671_FADE(work->blooms[i], 0, value, sign);
            }
            setPolyFT4(&work->quad);
            work->quad.tpage = work->tpage[0];
            work->quad.clut = work->clut[0];
            setSemiTrans(&work->quad, 1);
            setUV4(&work->quad, work->blooms[i].u, work->blooms[i].v,
                work->blooms[i].u + 63, work->blooms[i].v,
                work->blooms[i].u, work->blooms[i].v + 63,
                work->blooms[i].u + 63, work->blooms[i].v + 63);
            setRGB0(&work->quad, work->blooms[i].color[0], work->blooms[i].color[1], work->blooms[i].color[2]);
            depth = RotAverage4(&work->flare[0], &work->flare[1], &work->flare[2], &work->flare[3],
                (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
            }
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->blooms[i].scale);
            RotMatrixZ(work->blooms[i].rotation.vz, &unit.matrix);
            RotMatrixY(work->blooms[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->blooms[i].position.vx;
            unit.matrix.t[1] = work->blooms[i].position.vy;
            unit.matrix.t[2] = work->blooms[i].position.vz;
            unit.super = work->blooms[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            for (j = 0; j < 8; j++) {
                for (k = 0; k < 8; k++) {
                    setPolyGT3(&work->fan);
                    work->fan.tpage = work->tpage[0];
                    work->fan.clut = work->clut[0];
                    setSemiTrans(&work->fan, 1);
                    setUV3(&work->fan, work->blooms[i].u + 64, work->blooms[i].v,
                        work->blooms[i].u + 127, work->blooms[i].v,
                        work->blooms[i].u + 96, work->blooms[i].v + 63);
                    setRGB2(&work->fan, 0, 0, 0);
                    setRGB0(&work->fan, work->blooms[i].color[0], work->blooms[i].color[1], work->blooms[i].color[2]);
                    setRGB1(&work->fan, work->blooms[i].color[0], work->blooms[i].color[1], work->blooms[i].color[2]);
                    depth = RotAverage3(&work->grid[0][k][j], &work->grid[1][k][j], &work->grid[2][k][j],
                        (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1, (PSXLONG *)&work->fan.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
                    }
                }
            }
            work->blooms[i].elapsed += step;
            if (work->blooms[i].duration < work->blooms[i].elapsed) {
                work->blooms[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->count; i++) {
        if (work->rings[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            value = work->rings[i].elapsed * 0x3000 / work->rings[i].duration + 0x800;
            setVector(&work->rings[i].scale, value, value, value);
            ScaleMatrix(&unit.matrix, &work->rings[i].scale);
            RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
            RotMatrixX(work->rings[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
            RotMatrixY(work->rings[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
            unit.matrix.t[0] = work->rings[i].position.vx;
            unit.matrix.t[1] = work->rings[i].position.vy;
            unit.matrix.t[2] = work->rings[i].position.vz;
            unit.super = work->rings[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            if (work->rings[i].elapsed > (u16)(work->rings[i].duration / 3)) {
                value = (work->rings[i].duration - work->rings[i].elapsed) * 3;
                sign = work->rings[i].duration * 2;
                MODEL671_FADE(work->rings[i], 3, value, sign);
            }
            for (j = 0; j < 16; j++) {
                setPolyGT4(&work->strip);
                work->strip.tpage = work->tpage[1];
                work->strip.clut = work->clut[1];
                setSemiTrans(&work->strip, 1);
                setUV4(&work->strip, work->rings[i].u, work->rings[i].v,
                    work->rings[i].u + 31, work->rings[i].v,
                    work->rings[i].u, work->rings[i].v + 127,
                    work->rings[i].u + 31, work->rings[i].v + 127);
                setRGB2(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                setRGB3(&work->strip, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                setRGB0(&work->strip, 0, 0, 0);
                setRGB1(&work->strip, 0, 0, 0);
                depth = RotAverage4(&work->rim[0][j], &work->rim[0][j + 1], &work->rim[1][j], &work->rim[1][j + 1],
                    (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                    (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                }
            }
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->rings[i].scale);
            RotMatrixZ(work->rings[i].elapsed * 10, &unit.matrix);
            RotMatrixX(work->rings[i].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
            RotMatrixY(work->rings[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
            unit.matrix.t[0] = work->rings[i].position.vx;
            unit.matrix.t[1] = work->rings[i].position.vy;
            unit.matrix.t[2] = work->rings[i].position.vz;
            unit.super = work->rings[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            setPolyFT4(&work->quad);
            work->quad.tpage = work->tpage[2];
            work->quad.clut = work->clut[2];
            setSemiTrans(&work->quad, 1);
            setUV4(&work->quad, work->rings[i].u + 32, work->rings[i].v,
                work->rings[i].u + 159, work->rings[i].v,
                work->rings[i].u + 32, work->rings[i].v + 127,
                work->rings[i].u + 159, work->rings[i].v + 127);
            setRGB0(&work->quad, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
            depth = RotAverage4(&work->ring[0], &work->ring[1], &work->ring[2], &work->ring[3],
                (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                &interpolation, &flag);
            if (flag >= 0) {
                GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
            }
            work->rings[i].elapsed += step;
            if (work->rings[i].duration < work->rings[i].elapsed) {
                work->rings[i].active = 0;
            }
            result = 4;
        } else {
            inactive++;
        }
    }
    PopMatrix();
    switch (work->command) {
    case 0:
        if (work->elapsed > 150 && work->flags[4] == 0) {
            result = 3;
            work->flags[4] = 1;
        }
        break;
    }
    work->elapsed += step;
    if (inactive == work->count * 2) {
        result = 2;
    }
    return result;
}
