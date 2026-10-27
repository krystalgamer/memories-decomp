#include "../../types.h"
#include "variant673_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

/* Header 673's entry: textured quads rise from the slot one after another,
 * fade out and hand over to quads falling around the slot position, and
 * each falling quad leaves a spinning fan of triangles where it lands. The
 * setup also prints the work-area size. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model673State *work;
    MATRIX ls, lw;
    GsCOORDUNIT unit;
    Model673ViewSnapshot view;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, k, n, r, s, a;
    u32 tex;
    PSXLONG depth;
    s32 d;

    work = (Model673State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model673State));
#ifndef VERSION_FRENCH
        printf(D_8013CB30, sizeof(Model673State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        Model_CopySlotPosition(work->slot == 0, (u16 *)work->position);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB48[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB48[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB48[1]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
        case 3:
            work->count = 64;
            break;
        }
        work->radius = D_8013CB9C[command].radius;
        work->height = D_8013CB9C[command].height;
        for (i = 0; i < work->count; i++) {
            work->rising[i].duration = 0x20;
            work->rising[i].active = 0;
            work->rising[i].elapsed = 0;
            work->rising[i].mode = 1;
            work->rising[i].coord = Model_GetCameraCoordinateUnit();
            work->rising[i].red = D_8013CB9C[command].red;
            work->rising[i].green = D_8013CB9C[command].green;
            work->rising[i].blue = D_8013CB9C[command].blue;
            work->rising[i].u = 0;
            work->rising[i].v = 0;
            setVector(&work->rising[i].scale, 0x1000, 0x1000, 0x1000);
            n = work->radius * rsin((i << 12) / work->count) / 4096;
            d = i % 2 * 2 - 1;
            k = i % 8 << 9;
            setVector(&work->rising[i].growth, d * n * rsin(k) / 4096, -work->height,
                n * rcos(k) / 4096);
            setVector(&work->rising[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->rising[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->falling[i].duration = 0x20;
            work->falling[i].active = 0;
            work->falling[i].elapsed = 0;
            work->falling[i].mode = 1;
            work->falling[i].coord = Model_GetCameraCoordinateUnit();
            work->falling[i].red = D_8013CB9C[command].red;
            work->falling[i].green = D_8013CB9C[command].green;
            work->falling[i].blue = D_8013CB9C[command].blue;
            work->falling[i].u = 0;
            work->falling[i].v = 0;
            setVector(&work->falling[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->falling[i].rotation, 0, work->slot * 2048, 0);
            s = i % 16 << 8;
            n = rsin(s);
            n = work->radius * ((n << 1) + n) / 4096;
            d = i % 2 * 2 - 1;
            setVector(&work->falling[i].growth, 0, work->height * 2 + d * work->height * rsin(s) / 4096, 0);
            s = i % 8 << 9;
            setVector(&work->falling[i].position, work->position[0] + d * n * rsin(s) / 4096,
                work->position[1] - work->height * 2, work->position[2] + n * rcos(s) / 4096);
        }
        for (i = 0; i < work->count; i++) {
            work->fans[i].active = 0;
            work->fans[i].elapsed = 0;
            work->fans[i].duration = 0x20;
            work->fans[i].mode = 1;
            work->fans[i].coord = Model_GetCameraCoordinateUnit();
            work->fans[i].red = D_8013CB9C[command].fan_red;
            work->fans[i].green = D_8013CB9C[command].fan_green;
            work->fans[i].blue = D_8013CB9C[command].fan_blue;
            work->fans[i].u = 0;
            work->fans[i].v = 0x20;
            setVector(&work->fans[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->fans[i].growth, 0, 0, 0);
            setVector(&work->fans[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->fans[i].position, 0, 0, 0);
        }
        work->delay = D_8013CB9C[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        n = work->radius;
        setVector(&work->square[0], -n, -n, 0);
        setVector(&work->square[1], n, -n, 0);
        setVector(&work->square[2], -n, n, 0);
        setVector(&work->square[3], n, n, 0);
        n = work->radius * 3;
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i], n * rsin(i << 8) / 4096, n * rcos(i << 8) / 4096, 0);
        }
        n = work->radius;
        setVector(&work->centre, 0, 0, 0);
        setVector(&work->corners[0], -n, -n, 0);
        setVector(&work->corners[1], n, -n, 0);
        setVector(&work->corners[2], -n, n, 0);
        setVector(&work->corners[3], n, n, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed < 0x122 && work->flags[0] < work->count && work->elapsed >= work->flags[0] * 4) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 0x1A);
                GsGetLwUnit(&unit, &lw);
                setVector(&work->rising[work->flags[0]].position, lw.t[0], lw.t[1], lw.t[2]);
                work->rising[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            break;
        case 1:
            if (work->elapsed < 0xFA && work->flags[0] < work->count && work->elapsed >= work->flags[0] * 4) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 5);
                GsGetLwUnit(&unit, &lw);
                setVector(&work->rising[work->flags[0]].position, lw.t[0], lw.t[1], lw.t[2]);
                work->rising[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            break;
        case 2:
            if (work->elapsed < 0x76 && work->flags[0] < work->count && work->elapsed >= work->flags[0] * 2) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 0);
                GsGetLwUnit(&unit, &lw);
                setVector(&work->rising[work->flags[0]].position, lw.t[0], lw.t[1], lw.t[2]);
                work->rising[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            break;
        case 3:
            if (work->elapsed < 0x9E && work->flags[0] < work->count && work->elapsed >= work->flags[0] * 2) {
                work->falling[work->flags[0]].active = 1;
                work->flags[0]++;
            }
            break;
        }
        PushMatrix();
        inactive = 0;
        view = *(Model673ViewSnapshot *)Model_GetViewMetricsBuffer();
        for (i = 0; i < work->count; i++) {
            if (work->rising[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->rising[i].scale);
                RotMatrixZ(work->rising[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->rising[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->rising[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->rising[i].position.vx
                    + work->rising[i].growth.vx * work->rising[i].elapsed / work->rising[i].duration;
                unit.matrix.t[1] = work->rising[i].position.vy
                    + work->rising[i].growth.vy * work->rising[i].elapsed / work->rising[i].duration;
                unit.matrix.t[2] = work->rising[i].position.vz
                    + work->rising[i].growth.vz * work->rising[i].elapsed / work->rising[i].duration;
                unit.super = work->rising[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->rising[i].duration / 2 < work->rising[i].elapsed) {
                    n = (work->rising[i].duration - work->rising[i].elapsed) * 2;
                    d = work->rising[i].duration;
                    work->rising[i].red = D_8013CB9C[work->command].red * n / d;
                    work->rising[i].green = D_8013CB9C[work->command].green * n / d;
                    work->rising[i].blue = D_8013CB9C[work->command].blue * n / d;
                }
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[2];
                work->flat.clut = work->clut[2];
                setSemiTrans(&work->flat, 1);
                setUV4(&work->flat, work->rising[i].u, work->rising[i].v, work->rising[i].u + 31,
                    work->rising[i].v, work->rising[i].u, work->rising[i].v + 31,
                    work->rising[i].u + 31, work->rising[i].v + 31);
                setRGB0(&work->flat, work->rising[i].red, work->rising[i].green, work->rising[i].blue);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                    (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                }
                if (work->rising[i].u < 0xE0) {
                    work->rising[i].u += 32;
                }
                work->rising[i].elapsed += step;
                if (work->rising[i].duration < work->rising[i].elapsed) {
                    work->rising[i].active = 0;
                    work->falling[i].active = 1;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->falling[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = (work->falling[i].elapsed << 12) / work->falling[i].duration + 0x1000;
                setVector(&work->falling[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->falling[i].scale);
                RotMatrixZ(work->falling[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->falling[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->falling[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->falling[i].position.vx
                    + work->falling[i].growth.vx * work->falling[i].elapsed / work->falling[i].duration;
                unit.matrix.t[1] = work->falling[i].position.vy
                    + work->falling[i].growth.vy * work->falling[i].elapsed / work->falling[i].duration;
                unit.matrix.t[2] = work->falling[i].position.vz
                    + work->falling[i].growth.vz * work->falling[i].elapsed / work->falling[i].duration;
                unit.super = work->falling[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->falling[i].elapsed < work->falling[i].duration / 2) {
                    n = work->falling[i].elapsed * 2;
                    d = work->falling[i].duration;
                    work->falling[i].red = D_8013CB9C[work->command].red * n / d;
                    work->falling[i].green = D_8013CB9C[work->command].green * n / d;
                    work->falling[i].blue = D_8013CB9C[work->command].blue * n / d;
                }
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[2];
                work->flat.clut = work->clut[2];
                setSemiTrans(&work->flat, 1);
                setUV4(&work->flat, work->falling[i].u, work->falling[i].v, work->falling[i].u + 31,
                    work->falling[i].v, work->falling[i].u, work->falling[i].v + 31,
                    work->falling[i].u + 31, work->falling[i].v + 31);
                setRGB0(&work->flat, work->falling[i].red, work->falling[i].green, work->falling[i].blue);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                    (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                }
                if (work->falling[i].u < 0xE0) {
                    work->falling[i].u += 32;
                }
                work->falling[i].elapsed += step;
                if (work->falling[i].duration < work->falling[i].elapsed) {
                    work->falling[i].active = 0;
                    setVector(&work->fans[i].position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                    work->fans[i].active = 1;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->fans[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = work->fans[i].elapsed * 0x3000 / work->fans[i].duration + 0x1000;
                setVector(&work->fans[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->fans[i].scale);
                RotMatrixZ(work->fans[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->fans[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->fans[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->fans[i].position.vx;
                unit.matrix.t[1] = work->fans[i].position.vy;
                unit.matrix.t[2] = work->fans[i].position.vz;
                unit.super = work->fans[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->fans[i].duration / 2 < work->fans[i].elapsed) {
                    n = (work->fans[i].duration - work->fans[i].elapsed) * 2;
                    d = work->fans[i].duration;
                    work->fans[i].red = D_8013CB9C[work->command].fan_red * n / d;
                    work->fans[i].green = D_8013CB9C[work->command].fan_green * n / d;
                    work->fans[i].blue = D_8013CB9C[work->command].fan_blue * n / d;
                }
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[0];
                work->flat.clut = work->clut[0];
                setSemiTrans(&work->flat, 1);
                setUV4(&work->flat, work->fans[i].u, work->fans[i].v, work->fans[i].u + 63,
                    work->fans[i].v, work->fans[i].u, work->fans[i].v + 63,
                    work->fans[i].u + 63, work->fans[i].v + 63);
                setRGB0(&work->flat, work->fans[i].red, work->fans[i].green, work->fans[i].blue);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                    (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT3(&work->fan);
                    work->fan.tpage = work->tpage[1];
                    work->fan.clut = work->clut[1];
                    setSemiTrans(&work->fan, 1);
                    setUV3(&work->fan, work->fans[i].u + 32, work->fans[i].v + 64, work->fans[i].u,
                        work->fans[i].v + 127, work->fans[i].u + 63, work->fans[i].v + 127);
                    setRGB0(&work->fan, work->fans[i].red, work->fans[i].green, work->fans[i].blue);
                    setRGB1(&work->fan, 0, 0, 0);
                    setRGB2(&work->fan, 0, 0, 0);
                    depth = RotAverage3(&work->centre, &work->ring[k], &work->ring[k + 1],
                        (PSXLONG *)&work->fan.x0, (PSXLONG *)&work->fan.x1, (PSXLONG *)&work->fan.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->fan, func_80058F10(), (u16)depth);
                    }
                }
                work->fans[i].elapsed += step;
                if (work->fans[i].duration < work->fans[i].elapsed) {
                    work->fans[i].active = 0;
                }
                result = 4;
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
    }
    return result;
}
