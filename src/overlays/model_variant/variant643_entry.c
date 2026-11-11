#include "../../types.h"
#include "variant643_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 643's entry: four nested cone rings and a flat band spread
 * from a model joint while a grid of sprites drifts outwards. The
 * setup also prints the work-area size. */

#define MODEL643_FADE(e, value, divisor)                     \
    (e).head.color[0] = (e).head.color[0] * (value) / (divisor);       \
    (e).head.color[1] = (e).head.color[1] * (value) / (divisor);       \
    (e).head.color[2] = (e).head.color[2] * (value) / (divisor)

s32 ModelVariant643_HandleRequest(u8 *context, s32 command)
{
    Model643State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshot view;
    SVECTOR inner, outer;
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, m, n;
    s32 value, sign;
    u32 tex;
    u8 r, g, b;
    long depth;

    work = (Model643State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model643State));
#ifndef VERSION_FRENCH
        printf(gModelVariant643_szWorkSizeFormat, sizeof(Model643State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant643_aImages[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant643_aImages[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
        case 3:
            work->cone_count = 4;
            work->band_count = 1;
            work->rows = 8;
            work->columns = 16;
            break;
        }
        work->radius = gModelVariant643_aConfigs[command].radius;
        work->height = gModelVariant643_aConfigs[command].height;
        for (i = 0; i < work->cone_count; i++) {
            work->cones[i].head.duration = 48;
            work->cones[i].head.active = 0;
            work->cones[i].head.elapsed = 0;
            work->cones[i].head.mode = 1;
            work->cones[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->cones[i].head.color[0] = gModelVariant643_aConfigs[command].color[0];
            work->cones[i].head.color[1] = gModelVariant643_aConfigs[command].color[1];
            work->cones[i].head.color[2] = gModelVariant643_aConfigs[command].color[2];
            work->cones[i].head.u = 0;
            work->cones[i].head.v = 0;
            setVector(&work->cones[i].xform.scale, 0, 0, 0);
            setVector(&work->cones[i].target, 0, 0, 0);
            if (command == 2) {
                setVector(&work->cones[i].xform.rotation, -0x200, work->head.slot * 2048, 0);
            } else {
                setVector(&work->cones[i].xform.rotation, 0, work->head.slot * 2048, 0);
            }
            setVector(&work->cones[i].xform.position, 0, 0, 0);
        }
        for (i = 0; i < work->band_count; i++) {
            work->bands[i].head.duration = 48;
            work->bands[i].head.active = 0;
            work->bands[i].head.elapsed = 0;
            work->bands[i].head.mode = 1;
            work->bands[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->bands[i].head.color[0] = gModelVariant643_aConfigs[command].color[0];
            work->bands[i].head.color[1] = gModelVariant643_aConfigs[command].color[1];
            work->bands[i].head.color[2] = gModelVariant643_aConfigs[command].color[2];
            setVector(&work->bands[i].xform.scale, 0, 0, 0);
            setVector(&work->bands[i].target, 0, 0, 0);
            if (command == 2) {
                setVector(&work->bands[i].xform.rotation, -0x200, work->head.slot * 2048, 0);
            } else {
                setVector(&work->bands[i].xform.rotation, 0, work->head.slot * 2048, 0);
            }
            setVector(&work->bands[i].xform.position, 0, 0, 0);
        }
        for (k = 0; k < work->rows; k++) {
            for (j = 0; j < work->columns; j++) {
                work->grid[k][j].head.active = 0;
                work->grid[k][j].head.elapsed = 0;
                work->grid[k][j].head.duration = 32;
                work->grid[k][j].head.mode = 1;
                work->grid[k][j].xform.coord = Model_GetCameraCoordinateUnit();
                work->grid[k][j].head.color[0] = gModelVariant643_aConfigs[command].color[3];
                work->grid[k][j].head.color[1] = gModelVariant643_aConfigs[command].color[4];
                work->grid[k][j].head.color[2] = gModelVariant643_aConfigs[command].color[5];
                work->grid[k][j].head.u = 0;
                work->grid[k][j].head.v = 128;
                work->grid[k][j].cursor = 1;
                setVector(&work->grid[k][j].xform.scale, 4096, 4096, 4096);
                setVector(&work->grid[k][j].target,
                    -(rsin((j % 4) << 10) * 500 / 4096 * (j % 2 * 2 - 1) * work->direction),
                    -(rcos(((j % 5) << 12) / 5) * 500 / 4096 * (j % 2 * 2 - 1)),
                    -(rcos(((j % 6) << 12) / 6) * 500 / 4096 * work->direction));
                setVector(&work->grid[k][j].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->grid[k][j].xform.position, 0, 0, 0);
                work->grid[k][j].xform.position.vy = -350;
                work->grid[k][j].xform.position.vz = work->direction * 450;
            }
        }
        work->delay = gModelVariant643_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->cone[i][0], work->radius * rsin(i << 8) / 4096, work->radius * rcos(i << 8) / 4096, 0);
            setVector(&work->cone[i][1], work->radius * 4 * rsin(i << 8) / 4096, work->radius * 4 * rcos(i << 8) / 4096, -work->height);
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i][0], work->radius * 2 * rsin(i << 8) / 4096, work->radius * 2 * rcos(i << 8) / 4096, 0);
            setVector(&work->ring[i][1], work->radius * 5 * rsin(i << 8) / 4096, work->radius * 5 * rcos(i << 8) / 4096, 0);
        }
        setVector(&work->corner[0], -work->radius / 2, -work->radius / 2, 0);
        setVector(&work->corner[1], work->radius / 2, -work->radius / 2, 0);
        setVector(&work->corner[2], -work->radius / 2, work->radius / 2, 0);
        setVector(&work->corner[3], work->radius / 2, work->radius / 2, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
        return result;
    }
    switch (work->head.command) {
    case 0:
        if (work->head.elapsed == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 5);
            GsGetLwUnit(&unit, &ls);
            for (k = 0; k < work->cone_count; k++) {
                work->cones[k].head.active = 1;
                setVector(&work->cones[k].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            }
            for (i = 0; i < work->band_count; i++) {
                work->bands[i].head.active = 1;
                setVector(&work->bands[i].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            }
        }
        if (work->head.elapsed >= 41 && work->flags[0] == 0) {
            for (i = 0; i < work->columns; i++) {
                work->grid[0][i].head.active = 1;
            }
            work->flags[0] = 1;
        }
        break;
    case 1:
        if (work->head.elapsed == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 9);
            GsGetLwUnit(&unit, &ls);
            for (i = 0; i < work->cone_count; i++) {
                work->cones[i].head.active = 1;
                setVector(&work->cones[i].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            }
            for (i = 0; i < work->band_count; i++) {
                work->bands[i].head.active = 1;
                setVector(&work->bands[i].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            }
        }
        if (work->head.elapsed >= 8 && work->flags[0] == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 9);
            GsGetLwUnit(&unit, &ls);
            for (i = 0; i < work->columns; i++) {
                work->grid[0][i].head.active = 1;
                setVector(&work->grid[0][i].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            }
            work->flags[0] = 1;
        }
        break;
    case 2:
        if (work->head.elapsed == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 15);
            GsGetLwUnit(&unit, &ls);
            for (i = 0; i < work->cone_count; i++) {
                work->cones[i].head.active = 1;
                setVector(&work->cones[i].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            }
            for (i = 0; i < work->band_count; i++) {
                work->bands[i].head.active = 1;
                setVector(&work->bands[i].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            }
        }
        if (work->head.elapsed >= 41 && work->flags[0] == 0) {
            for (i = 0; i < work->columns; i++) {
                work->grid[0][i].head.active = 1;
            }
            work->flags[0] = 1;
        }
        break;
    case 3:
        if (work->head.elapsed == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 6);
            GsGetLwUnit(&unit, &ls);
            for (i = 0; i < work->cone_count; i++) {
                work->cones[i].head.active = 1;
                setVector(&work->cones[i].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            }
            for (i = 0; i < work->band_count; i++) {
                work->bands[i].head.active = 1;
                setVector(&work->bands[i].xform.position, ls.t[0], ls.t[1], ls.t[2]);
            }
        }
        if (work->head.elapsed >= 11 && work->flags[0] == 0) {
            for (i = 0; i < work->columns; i++) {
                work->grid[0][i].head.active = 1;
            }
            work->flags[0] = 1;
        }
        break;
    }
    PushMatrix();
    view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->cone_count; i++) {
        if (work->cones[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            ScaleMatrix(&unit.matrix, &work->cones[i].xform.scale);
            RotMatrixX(work->cones[i].xform.rotation.vx, &unit.matrix);
            RotMatrixY(work->cones[i].xform.rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->cones[i].xform.position.vx;
            unit.matrix.t[1] = work->cones[i].xform.position.vy;
            unit.matrix.t[2] = work->cones[i].xform.position.vz;
            unit.super = work->cones[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            value = (4096 - i * 1000) * work->cones[i].head.elapsed / work->cones[i].head.duration;
            setVector(&work->cones[i].xform.scale, value, value, value);
            value = i + 1;
            sign = work->cone_count;
            work->cones[i].head.color[0] = gModelVariant643_aConfigs[work->head.command].color[0] * value / sign;
            work->cones[i].head.color[1] = gModelVariant643_aConfigs[work->head.command].color[1] * value / sign;
            work->cones[i].head.color[2] = gModelVariant643_aConfigs[work->head.command].color[2] * value / sign;
            if (work->cones[i].head.elapsed > (u32)work->cones[i].head.duration / 2) {
                value = (work->cones[i].head.duration - work->cones[i].head.elapsed) * 2;
                sign = work->cones[i].head.duration;
                MODEL643_FADE(work->cones[i], value, sign);
            }
            for (j = 0; j < 16; j++) {
                setPolyGT4(&work->strip);
                work->strip.tpage = work->tpage[0];
                work->strip.clut = work->clut[0];
                setSemiTrans(&work->strip, 1);
                setUV4(&work->strip, work->cones[i].head.u, work->cones[i].head.v,
                    work->cones[i].head.u + 31, work->cones[i].head.v,
                    work->cones[i].head.u, work->cones[i].head.v + 127,
                    work->cones[i].head.u + 31, work->cones[i].head.v + 127);
                r = work->cones[i].head.color[0];
                g = work->cones[i].head.color[1];
                b = work->cones[i].head.color[2];
                setRGB0(&work->strip, 0, 0, 0);
                setRGB1(&work->strip, 0, 0, 0);
                setRGB2(&work->strip, r, g, b);
                setRGB3(&work->strip, r, g, b);
                depth = RotAverage4(&work->cone[j][0], &work->cone[j + 1][0], &work->cone[j][1], &work->cone[j + 1][1],
                    (long *)&work->strip.x0, (long *)&work->strip.x1,
                    (long *)&work->strip.x2, (long *)&work->strip.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
            }
            work->cones[i].head.elapsed += step;
            if (work->cones[i].head.duration < work->cones[i].head.elapsed) {
                work->cones[i].head.active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->band_count; i++) {
        if (work->bands[i].head.active == 1) {
            unit.flg = 0;
            unit.matrix = GsIDMATRIX;
            ScaleMatrix(&unit.matrix, &work->bands[i].xform.scale);
            RotMatrixX(work->bands[i].xform.rotation.vx, &unit.matrix);
            RotMatrixY(work->bands[i].xform.rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->bands[i].xform.position.vx;
            unit.matrix.t[1] = work->bands[i].xform.position.vy;
            unit.matrix.t[2] = work->bands[i].xform.position.vz;
            unit.super = work->bands[i].xform.coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            value = (4096 - i * 1000) * work->bands[i].head.elapsed / work->bands[i].head.duration;
            setVector(&work->bands[i].xform.scale, value, value, value);
            if (work->bands[i].head.elapsed > (u32)work->bands[i].head.duration / 2) {
                value = (work->bands[i].head.duration - work->bands[i].head.elapsed) * 2;
                sign = work->bands[i].head.duration;
                work->bands[i].head.color[0] = gModelVariant643_aConfigs[work->head.command].color[0] * value / sign;
                work->bands[i].head.color[1] = gModelVariant643_aConfigs[work->head.command].color[1] * value / sign;
                work->bands[i].head.color[2] = gModelVariant643_aConfigs[work->head.command].color[2] * value / sign;
            }
            for (j = 0; j < 16; j++) {
                setVector(&inner, (work->ring[j][0].vx + work->ring[j][1].vx) / 2,
                    (work->ring[j][0].vy + work->ring[j][1].vy) / 2,
                    (work->ring[j][0].vz + work->ring[j][1].vz) / 2);
                setVector(&outer, (work->ring[j + 1][0].vx + work->ring[j + 1][1].vx) / 2,
                    (work->ring[j + 1][0].vy + work->ring[j + 1][1].vy) / 2,
                    (work->ring[j + 1][0].vz + work->ring[j + 1][1].vz) / 2);
                setPolyG4(&work->band);
                r = work->bands[i].head.color[0];
                g = work->bands[i].head.color[1];
                b = work->bands[i].head.color[2];
                setRGB0(&work->band, 0, 0, 0);
                setRGB1(&work->band, 0, 0, 0);
                setRGB2(&work->band, r, g, b);
                setRGB3(&work->band, r, g, b);
                depth = RotAverage4(&work->ring[j][0], &work->ring[j + 1][0], &inner, &outer,
                    (long *)&work->band.x0, (long *)&work->band.x1,
                    (long *)&work->band.x2, (long *)&work->band.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->band, Graphics_GetDefaultOrderingTable(), (u16)depth, work->bands[i].head.mode);
                }
                setPolyG4(&work->band);
                r = work->bands[i].head.color[0];
                g = work->bands[i].head.color[1];
                b = work->bands[i].head.color[2];
                setRGB2(&work->band, 0, 0, 0);
                setRGB3(&work->band, 0, 0, 0);
                setRGB0(&work->band, r, g, b);
                setRGB1(&work->band, r, g, b);
                depth = RotAverage4(&inner, &outer, &work->ring[j][1], &work->ring[j + 1][1],
                    (long *)&work->band.x0, (long *)&work->band.x1,
                    (long *)&work->band.x2, (long *)&work->band.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->band, Graphics_GetDefaultOrderingTable(), (u16)depth, work->bands[i].head.mode);
                }
            }
            work->bands[i].head.elapsed += step;
            if (work->bands[i].head.duration < work->bands[i].head.elapsed) {
                work->bands[i].head.active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (j = 0; j < work->columns; j++) {
        for (i = 0; i < work->rows; i++) {
            if (work->grid[i][j].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->grid[i][j].xform.scale);
                RotMatrixZ(work->grid[i][j].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->grid[i][j].xform.rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                RotMatrixY(work->grid[i][j].xform.rotation.vy - (view.yaw - 0xC06), &unit.matrix);
                unit.matrix.t[0] = work->grid[i][j].xform.position.vx
                    + work->grid[i][j].target.vx * work->grid[i][j].head.elapsed / work->grid[i][j].head.duration;
                unit.matrix.t[1] = work->grid[i][j].xform.position.vy
                    + work->grid[i][j].target.vy * work->grid[i][j].head.elapsed / work->grid[i][j].head.duration;
                unit.matrix.t[2] = work->grid[i][j].xform.position.vz
                    + work->grid[i][j].target.vz * work->grid[i][j].head.elapsed / work->grid[i][j].head.duration;
                unit.super = work->grid[i][j].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->grid[i][j].head.elapsed > (u32)work->grid[i][j].head.duration / 2) {
                    value = (work->grid[i][j].head.duration - work->grid[i][j].head.elapsed) * 2;
                    sign = work->grid[i][j].head.duration;
                    work->grid[i][j].head.color[0] = gModelVariant643_aConfigs[work->head.command].color[3] * value / sign;
                    work->grid[i][j].head.color[1] = gModelVariant643_aConfigs[work->head.command].color[4] * value / sign;
                    work->grid[i][j].head.color[2] = gModelVariant643_aConfigs[work->head.command].color[5] * value / sign;
                }
                setPolyFT4(&work->quad);
                work->quad.tpage = work->tpage[1];
                work->quad.clut = work->clut[1];
                setSemiTrans(&work->quad, 1);
                value = 1;
                setUV4(&work->quad, work->grid[i][j].head.u, work->grid[i][j].head.v,
                    work->grid[i][j].head.u + 31, work->grid[i][j].head.v,
                    work->grid[i][j].head.u, work->grid[i][j].head.v + 31,
                    work->grid[i][j].head.u + 31, work->grid[i][j].head.v + 31);
                r = work->grid[i][j].head.color[0];
                g = work->grid[i][j].head.color[1];
                b = work->grid[i][j].head.color[2];
                setRGB0(&work->quad, r, g, b);
                depth = RotAverage4(&work->corner[0], &work->corner[1], &work->corner[2], &work->corner[3],
                    (long *)&work->quad.x0, (long *)&work->quad.x1,
                    (long *)&work->quad.x2, (long *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                work->grid[i][j].head.u += 32;
                if (work->grid[i][j].head.u == 224) {
                    work->grid[i][j].head.u = 0;
                }
                if (i == 0) {
                    work->grid[work->grid[0][j].cursor][j].head.active = value;
                    work->grid[work->grid[0][j].cursor][j].head.elapsed = 0;
                    work->grid[work->grid[0][j].cursor][j].head.duration = 32;
                    work->grid[work->grid[0][j].cursor][j].head.mode = value;
                    work->grid[work->grid[0][j].cursor][j].xform.coord = Model_GetCameraCoordinateUnit();
                    work->grid[work->grid[0][j].cursor][j].head.color[0] = gModelVariant643_aConfigs[work->head.command].color[3];
                    work->grid[work->grid[0][j].cursor][j].head.color[1] = gModelVariant643_aConfigs[work->head.command].color[4];
                    work->grid[work->grid[0][j].cursor][j].head.color[2] = gModelVariant643_aConfigs[work->head.command].color[5];
                    work->grid[work->grid[0][j].cursor][j].head.u = 0;
                    work->grid[work->grid[0][j].cursor][j].head.v = 128;
                    setVector(&work->grid[work->grid[0][j].cursor][j].xform.scale, 4096, 4096, 4096);
                    setVector(&work->grid[work->grid[0][j].cursor][j].target, 0, 0, 0);
                    setVector(&work->grid[work->grid[0][j].cursor][j].xform.rotation, 0, work->head.slot * 2048, 0);
                    (work->grid[work->grid[0][j].cursor] + j)->xform.position.vx = unit.matrix.t[0];
                    (work->grid[work->grid[0][j].cursor] + j)->xform.position.vy = unit.matrix.t[1];
                    (work->grid[work->grid[0][j].cursor] + j)->xform.position.vz = unit.matrix.t[2];
                    work->grid[0][j].cursor++;
                    if (work->grid[0][j].cursor > work->rows - 1) {
                        work->grid[0][j].cursor = value;
                    }
                }
                work->grid[i][j].head.elapsed += step;
                if (work->grid[i][j].head.duration < work->grid[i][j].head.elapsed) {
                    work->grid[i][j].head.active = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
    }
    PopMatrix();
    work->head.elapsed += step;
    if (inactive == work->cone_count + work->band_count + work->rows * work->columns) {
        result = 1;
        if (work->flags[4] == 1) {
            result = 2;
        }
        work->flags[4] = 1;
    }
    return result;
}
