#include "../../types.h"
#include "variant673_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/model_slot_support.h"

/* Header 673's entry: textured quads rise from the slot one after another,
 * fade out and hand over to quads falling around the slot position, and
 * each falling quad leaves a spinning fan of triangles where it lands. The
 * setup also prints the work-area size. */

s32 ModelVariant673_HandleRequest(u8 *context, s32 command)
{
    Model673State *work;
    MATRIX ls, lw;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshotBytes view;
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, k, n, r, s, a;
    u32 tex;
    long depth;
    s32 d;

    work = (Model673State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model673State));
#ifndef VERSION_FRENCH
        printf(gModelVariant673_szWorkSizeFormat, sizeof(Model673State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        Model_CopySlotPosition(work->head.slot == 0, (u16 *)work->position);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant673_aImages[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant673_aImages[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant673_aImages[1]);
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
        work->radius = gModelVariant673_aConfigs[command].radius;
        work->height = gModelVariant673_aConfigs[command].height;
        for (i = 0; i < work->count; i++) {
            work->rising[i].head.duration = 0x20;
            work->rising[i].head.active = 0;
            work->rising[i].head.elapsed = 0;
            work->rising[i].head.mode = 1;
            work->rising[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->rising[i].head.color[0] = gModelVariant673_aConfigs[command].red;
            work->rising[i].head.color[1] = gModelVariant673_aConfigs[command].green;
            work->rising[i].head.color[2] = gModelVariant673_aConfigs[command].blue;
            work->rising[i].head.u = 0;
            work->rising[i].head.v = 0;
            setVector(&work->rising[i].xform.scale, 0x1000, 0x1000, 0x1000);
            n = work->radius * rsin((i << 12) / work->count) / 4096;
            d = i % 2 * 2 - 1;
            k = i % 8 << 9;
            setVector(&work->rising[i].growth, d * n * rsin(k) / 4096, -work->height,
                n * rcos(k) / 4096);
            setVector(&work->rising[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->rising[i].xform.position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->falling[i].head.duration = 0x20;
            work->falling[i].head.active = 0;
            work->falling[i].head.elapsed = 0;
            work->falling[i].head.mode = 1;
            work->falling[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->falling[i].head.color[0] = gModelVariant673_aConfigs[command].red;
            work->falling[i].head.color[1] = gModelVariant673_aConfigs[command].green;
            work->falling[i].head.color[2] = gModelVariant673_aConfigs[command].blue;
            work->falling[i].head.u = 0;
            work->falling[i].head.v = 0;
            setVector(&work->falling[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->falling[i].xform.rotation, 0, work->head.slot * 2048, 0);
            s = i % 16 << 8;
            n = rsin(s);
            n = work->radius * ((n << 1) + n) / 4096;
            d = i % 2 * 2 - 1;
            setVector(&work->falling[i].growth, 0, work->height * 2 + d * work->height * rsin(s) / 4096, 0);
            s = i % 8 << 9;
            setVector(&work->falling[i].xform.position, work->position[0] + d * n * rsin(s) / 4096,
                work->position[1] - work->height * 2, work->position[2] + n * rcos(s) / 4096);
        }
        for (i = 0; i < work->count; i++) {
            work->fans[i].head.active = 0;
            work->fans[i].head.elapsed = 0;
            work->fans[i].head.duration = 0x20;
            work->fans[i].head.mode = 1;
            work->fans[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->fans[i].head.color[0] = gModelVariant673_aConfigs[command].fan_red;
            work->fans[i].head.color[1] = gModelVariant673_aConfigs[command].fan_green;
            work->fans[i].head.color[2] = gModelVariant673_aConfigs[command].fan_blue;
            work->fans[i].head.u = 0;
            work->fans[i].head.v = 0x20;
            setVector(&work->fans[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->fans[i].growth, 0, 0, 0);
            setVector(&work->fans[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->fans[i].xform.position, 0, 0, 0);
        }
        work->delay = gModelVariant673_aConfigs[command].delay;
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
        switch (work->head.command) {
        case 0:
            if (work->head.elapsed < 0x122 && work->flags[0] < work->count && work->head.elapsed >= work->flags[0] * 4) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 0x1A);
                GsGetLwUnit(&unit, &lw);
                setVector(&work->rising[work->flags[0]].xform.position, lw.t[0], lw.t[1], lw.t[2]);
                work->rising[work->flags[0]].head.active = 1;
                work->flags[0]++;
            }
            break;
        case 1:
            if (work->head.elapsed < 0xFA && work->flags[0] < work->count && work->head.elapsed >= work->flags[0] * 4) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 5);
                GsGetLwUnit(&unit, &lw);
                setVector(&work->rising[work->flags[0]].xform.position, lw.t[0], lw.t[1], lw.t[2]);
                work->rising[work->flags[0]].head.active = 1;
                work->flags[0]++;
            }
            break;
        case 2:
            if (work->head.elapsed < 0x76 && work->flags[0] < work->count && work->head.elapsed >= work->flags[0] * 2) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 0);
                GsGetLwUnit(&unit, &lw);
                setVector(&work->rising[work->flags[0]].xform.position, lw.t[0], lw.t[1], lw.t[2]);
                work->rising[work->flags[0]].head.active = 1;
                work->flags[0]++;
            }
            break;
        case 3:
            if (work->head.elapsed < 0x9E && work->flags[0] < work->count && work->head.elapsed >= work->flags[0] * 2) {
                work->falling[work->flags[0]].head.active = 1;
                work->flags[0]++;
            }
            break;
        }
        PushMatrix();
        inactive = 0;
        view = *(ModelVariantViewSnapshotBytes *)Model_GetViewMetricsBuffer();
        for (i = 0; i < work->count; i++) {
            if (work->rising[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->rising[i].xform.scale);
                RotMatrixZ(work->rising[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->rising[i].xform.rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->rising[i].xform.rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->rising[i].xform.position.vx
                    + work->rising[i].growth.vx * work->rising[i].head.elapsed / work->rising[i].head.duration;
                unit.matrix.t[1] = work->rising[i].xform.position.vy
                    + work->rising[i].growth.vy * work->rising[i].head.elapsed / work->rising[i].head.duration;
                unit.matrix.t[2] = work->rising[i].xform.position.vz
                    + work->rising[i].growth.vz * work->rising[i].head.elapsed / work->rising[i].head.duration;
                unit.super = work->rising[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->rising[i].head.duration / 2 < work->rising[i].head.elapsed) {
                    n = (work->rising[i].head.duration - work->rising[i].head.elapsed) * 2;
                    d = work->rising[i].head.duration;
                    work->rising[i].head.color[0] = gModelVariant673_aConfigs[work->head.command].red * n / d;
                    work->rising[i].head.color[1] = gModelVariant673_aConfigs[work->head.command].green * n / d;
                    work->rising[i].head.color[2] = gModelVariant673_aConfigs[work->head.command].blue * n / d;
                }
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[2];
                work->flat.clut = work->clut[2];
                setSemiTrans(&work->flat, 1);
                setUV4(&work->flat, work->rising[i].head.u, work->rising[i].head.v, work->rising[i].head.u + 31,
                    work->rising[i].head.v, work->rising[i].head.u, work->rising[i].head.v + 31,
                    work->rising[i].head.u + 31, work->rising[i].head.v + 31);
                setRGB0(&work->flat, work->rising[i].head.color[0], work->rising[i].head.color[1], work->rising[i].head.color[2]);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (long *)&work->flat.x0, (long *)&work->flat.x1,
                    (long *)&work->flat.x2, (long *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                if (work->rising[i].head.u < 0xE0) {
                    work->rising[i].head.u += 32;
                }
                work->rising[i].head.elapsed += step;
                if (work->rising[i].head.duration < work->rising[i].head.elapsed) {
                    work->rising[i].head.active = 0;
                    work->falling[i].head.active = 1;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->falling[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                n = (work->falling[i].head.elapsed << 12) / work->falling[i].head.duration + 0x1000;
                setVector(&work->falling[i].xform.scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->falling[i].xform.scale);
                RotMatrixZ(work->falling[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->falling[i].xform.rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->falling[i].xform.rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->falling[i].xform.position.vx
                    + work->falling[i].growth.vx * work->falling[i].head.elapsed / work->falling[i].head.duration;
                unit.matrix.t[1] = work->falling[i].xform.position.vy
                    + work->falling[i].growth.vy * work->falling[i].head.elapsed / work->falling[i].head.duration;
                unit.matrix.t[2] = work->falling[i].xform.position.vz
                    + work->falling[i].growth.vz * work->falling[i].head.elapsed / work->falling[i].head.duration;
                unit.super = work->falling[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->falling[i].head.elapsed < work->falling[i].head.duration / 2) {
                    n = work->falling[i].head.elapsed * 2;
                    d = work->falling[i].head.duration;
                    work->falling[i].head.color[0] = gModelVariant673_aConfigs[work->head.command].red * n / d;
                    work->falling[i].head.color[1] = gModelVariant673_aConfigs[work->head.command].green * n / d;
                    work->falling[i].head.color[2] = gModelVariant673_aConfigs[work->head.command].blue * n / d;
                }
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[2];
                work->flat.clut = work->clut[2];
                setSemiTrans(&work->flat, 1);
                setUV4(&work->flat, work->falling[i].head.u, work->falling[i].head.v, work->falling[i].head.u + 31,
                    work->falling[i].head.v, work->falling[i].head.u, work->falling[i].head.v + 31,
                    work->falling[i].head.u + 31, work->falling[i].head.v + 31);
                setRGB0(&work->flat, work->falling[i].head.color[0], work->falling[i].head.color[1], work->falling[i].head.color[2]);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (long *)&work->flat.x0, (long *)&work->flat.x1,
                    (long *)&work->flat.x2, (long *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                if (work->falling[i].head.u < 0xE0) {
                    work->falling[i].head.u += 32;
                }
                work->falling[i].head.elapsed += step;
                if (work->falling[i].head.duration < work->falling[i].head.elapsed) {
                    work->falling[i].head.active = 0;
                    setVector(&work->fans[i].xform.position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                    work->fans[i].head.active = 1;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->fans[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                n = work->fans[i].head.elapsed * 0x3000 / work->fans[i].head.duration + 0x1000;
                setVector(&work->fans[i].xform.scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->fans[i].xform.scale);
                RotMatrixZ(work->fans[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->fans[i].xform.rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->fans[i].xform.rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->fans[i].xform.position.vx;
                unit.matrix.t[1] = work->fans[i].xform.position.vy;
                unit.matrix.t[2] = work->fans[i].xform.position.vz;
                unit.super = work->fans[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->fans[i].head.duration / 2 < work->fans[i].head.elapsed) {
                    n = (work->fans[i].head.duration - work->fans[i].head.elapsed) * 2;
                    d = work->fans[i].head.duration;
                    work->fans[i].head.color[0] = gModelVariant673_aConfigs[work->head.command].fan_red * n / d;
                    work->fans[i].head.color[1] = gModelVariant673_aConfigs[work->head.command].fan_green * n / d;
                    work->fans[i].head.color[2] = gModelVariant673_aConfigs[work->head.command].fan_blue * n / d;
                }
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[0];
                work->flat.clut = work->clut[0];
                setSemiTrans(&work->flat, 1);
                setUV4(&work->flat, work->fans[i].head.u, work->fans[i].head.v, work->fans[i].head.u + 63,
                    work->fans[i].head.v, work->fans[i].head.u, work->fans[i].head.v + 63,
                    work->fans[i].head.u + 63, work->fans[i].head.v + 63);
                setRGB0(&work->flat, work->fans[i].head.color[0], work->fans[i].head.color[1], work->fans[i].head.color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (long *)&work->flat.x0, (long *)&work->flat.x1,
                    (long *)&work->flat.x2, (long *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT3(&work->fan);
                    work->fan.tpage = work->tpage[1];
                    work->fan.clut = work->clut[1];
                    setSemiTrans(&work->fan, 1);
                    setUV3(&work->fan, work->fans[i].head.u + 32, work->fans[i].head.v + 64, work->fans[i].head.u,
                        work->fans[i].head.v + 127, work->fans[i].head.u + 63, work->fans[i].head.v + 127);
                    setRGB0(&work->fan, work->fans[i].head.color[0], work->fans[i].head.color[1], work->fans[i].head.color[2]);
                    setRGB1(&work->fan, 0, 0, 0);
                    setRGB2(&work->fan, 0, 0, 0);
                    depth = RotAverage3(&work->centre, &work->ring[k], &work->ring[k + 1],
                        (long *)&work->fan.x0, (long *)&work->fan.x1, (long *)&work->fan.x2,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->fan, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                work->fans[i].head.elapsed += step;
                if (work->fans[i].head.duration < work->fans[i].head.elapsed) {
                    work->fans[i].head.active = 0;
                }
                result = 4;
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
    }
    return result;
}
