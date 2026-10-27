#include "../../types.h"
#include "variant643_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 643's entry: four nested cone rings and a flat band spread
 * from a model joint while a grid of sprites drifts outwards. The
 * setup also prints the work-area size. */

#define MODEL643_FADE(e, value, divisor)                     \
    (e).color[0] = (e).color[0] * (value) / (divisor);       \
    (e).color[1] = (e).color[1] * (value) / (divisor);       \
    (e).color[2] = (e).color[2] * (value) / (divisor)

s32 func_8013B004(u8 *context, s32 command)
{
    Model643State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    Model643ViewSnapshot view;
    SVECTOR inner, outer;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, m, n;
    s32 value, sign;
    u32 tex;
    u8 r, g, b;
    PSXLONG depth;

    work = (Model643State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model643State));
#ifndef VERSION_FRENCH
        printf(D_8013D298, sizeof(Model643State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D2B0[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D2B0[0]);
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
        work->radius = D_8013D2E8[command].radius;
        work->height = D_8013D2E8[command].height;
        for (i = 0; i < work->cone_count; i++) {
            work->cones[i].duration = 48;
            work->cones[i].active = 0;
            work->cones[i].elapsed = 0;
            work->cones[i].mode = 1;
            work->cones[i].coord = Model_GetCameraCoordinateUnit();
            work->cones[i].color[0] = D_8013D2E8[command].color[0];
            work->cones[i].color[1] = D_8013D2E8[command].color[1];
            work->cones[i].color[2] = D_8013D2E8[command].color[2];
            work->cones[i].u = 0;
            work->cones[i].v = 0;
            setVector(&work->cones[i].scale, 0, 0, 0);
            setVector(&work->cones[i].target, 0, 0, 0);
            if (command == 2) {
                setVector(&work->cones[i].rotation, -0x200, work->slot * 2048, 0);
            } else {
                setVector(&work->cones[i].rotation, 0, work->slot * 2048, 0);
            }
            setVector(&work->cones[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->band_count; i++) {
            work->bands[i].duration = 48;
            work->bands[i].active = 0;
            work->bands[i].elapsed = 0;
            work->bands[i].mode = 1;
            work->bands[i].coord = Model_GetCameraCoordinateUnit();
            work->bands[i].color[0] = D_8013D2E8[command].color[0];
            work->bands[i].color[1] = D_8013D2E8[command].color[1];
            work->bands[i].color[2] = D_8013D2E8[command].color[2];
            setVector(&work->bands[i].scale, 0, 0, 0);
            setVector(&work->bands[i].target, 0, 0, 0);
            if (command == 2) {
                setVector(&work->bands[i].rotation, -0x200, work->slot * 2048, 0);
            } else {
                setVector(&work->bands[i].rotation, 0, work->slot * 2048, 0);
            }
            setVector(&work->bands[i].position, 0, 0, 0);
        }
        for (k = 0; k < work->rows; k++) {
            for (j = 0; j < work->columns; j++) {
                work->grid[k][j].active = 0;
                work->grid[k][j].elapsed = 0;
                work->grid[k][j].duration = 32;
                work->grid[k][j].mode = 1;
                work->grid[k][j].coord = Model_GetCameraCoordinateUnit();
                work->grid[k][j].color[0] = D_8013D2E8[command].color[3];
                work->grid[k][j].color[1] = D_8013D2E8[command].color[4];
                work->grid[k][j].color[2] = D_8013D2E8[command].color[5];
                work->grid[k][j].u = 0;
                work->grid[k][j].v = 128;
                work->grid[k][j].cursor = 1;
                setVector(&work->grid[k][j].scale, 4096, 4096, 4096);
                setVector(&work->grid[k][j].target,
                    -(rsin((j % 4) << 10) * 500 / 4096 * (j % 2 * 2 - 1) * work->direction),
                    -(rcos(((j % 5) << 12) / 5) * 500 / 4096 * (j % 2 * 2 - 1)),
                    -(rcos(((j % 6) << 12) / 6) * 500 / 4096 * work->direction));
                setVector(&work->grid[k][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->grid[k][j].position, 0, 0, 0);
                work->grid[k][j].position.vy = -350;
                work->grid[k][j].position.vz = work->direction * 450;
            }
        }
        work->delay = D_8013D2E8[command].delay;
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
    switch (work->command) {
    case 0:
        if (work->elapsed == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 5);
            GsGetLwUnit(&unit, &ls);
            for (k = 0; k < work->cone_count; k++) {
                work->cones[k].active = 1;
                setVector(&work->cones[k].position, ls.t[0], ls.t[1], ls.t[2]);
            }
            for (i = 0; i < work->band_count; i++) {
                work->bands[i].active = 1;
                setVector(&work->bands[i].position, ls.t[0], ls.t[1], ls.t[2]);
            }
        }
        if (work->elapsed >= 41 && work->flags[0] == 0) {
            for (i = 0; i < work->columns; i++) {
                work->grid[0][i].active = 1;
            }
            work->flags[0] = 1;
        }
        break;
    case 1:
        if (work->elapsed == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 9);
            GsGetLwUnit(&unit, &ls);
            for (i = 0; i < work->cone_count; i++) {
                work->cones[i].active = 1;
                setVector(&work->cones[i].position, ls.t[0], ls.t[1], ls.t[2]);
            }
            for (i = 0; i < work->band_count; i++) {
                work->bands[i].active = 1;
                setVector(&work->bands[i].position, ls.t[0], ls.t[1], ls.t[2]);
            }
        }
        if (work->elapsed >= 8 && work->flags[0] == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 9);
            GsGetLwUnit(&unit, &ls);
            for (i = 0; i < work->columns; i++) {
                work->grid[0][i].active = 1;
                setVector(&work->grid[0][i].position, ls.t[0], ls.t[1], ls.t[2]);
            }
            work->flags[0] = 1;
        }
        break;
    case 2:
        if (work->elapsed == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 15);
            GsGetLwUnit(&unit, &ls);
            for (i = 0; i < work->cone_count; i++) {
                work->cones[i].active = 1;
                setVector(&work->cones[i].position, ls.t[0], ls.t[1], ls.t[2]);
            }
            for (i = 0; i < work->band_count; i++) {
                work->bands[i].active = 1;
                setVector(&work->bands[i].position, ls.t[0], ls.t[1], ls.t[2]);
            }
        }
        if (work->elapsed >= 41 && work->flags[0] == 0) {
            for (i = 0; i < work->columns; i++) {
                work->grid[0][i].active = 1;
            }
            work->flags[0] = 1;
        }
        break;
    case 3:
        if (work->elapsed == 0) {
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 6);
            GsGetLwUnit(&unit, &ls);
            for (i = 0; i < work->cone_count; i++) {
                work->cones[i].active = 1;
                setVector(&work->cones[i].position, ls.t[0], ls.t[1], ls.t[2]);
            }
            for (i = 0; i < work->band_count; i++) {
                work->bands[i].active = 1;
                setVector(&work->bands[i].position, ls.t[0], ls.t[1], ls.t[2]);
            }
        }
        if (work->elapsed >= 11 && work->flags[0] == 0) {
            for (i = 0; i < work->columns; i++) {
                work->grid[0][i].active = 1;
            }
            work->flags[0] = 1;
        }
        break;
    }
    PushMatrix();
    view = *(Model643ViewSnapshot *)Model_GetViewMetricsBuffer();
    inactive = 0;
    for (i = 0; i < work->cone_count; i++) {
        if (work->cones[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->cones[i].scale);
            RotMatrixX(work->cones[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->cones[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->cones[i].position.vx;
            unit.matrix.t[1] = work->cones[i].position.vy;
            unit.matrix.t[2] = work->cones[i].position.vz;
            unit.super = work->cones[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            value = (4096 - i * 1000) * work->cones[i].elapsed / work->cones[i].duration;
            setVector(&work->cones[i].scale, value, value, value);
            value = i + 1;
            sign = work->cone_count;
            work->cones[i].color[0] = D_8013D2E8[work->command].color[0] * value / sign;
            work->cones[i].color[1] = D_8013D2E8[work->command].color[1] * value / sign;
            work->cones[i].color[2] = D_8013D2E8[work->command].color[2] * value / sign;
            if (work->cones[i].elapsed > (u32)work->cones[i].duration / 2) {
                value = (work->cones[i].duration - work->cones[i].elapsed) * 2;
                sign = work->cones[i].duration;
                MODEL643_FADE(work->cones[i], value, sign);
            }
            for (j = 0; j < 16; j++) {
                setPolyGT4(&work->strip);
                work->strip.tpage = work->tpage[0];
                work->strip.clut = work->clut[0];
                setSemiTrans(&work->strip, 1);
                setUV4(&work->strip, work->cones[i].u, work->cones[i].v,
                    work->cones[i].u + 31, work->cones[i].v,
                    work->cones[i].u, work->cones[i].v + 127,
                    work->cones[i].u + 31, work->cones[i].v + 127);
                r = work->cones[i].color[0];
                g = work->cones[i].color[1];
                b = work->cones[i].color[2];
                setRGB0(&work->strip, 0, 0, 0);
                setRGB1(&work->strip, 0, 0, 0);
                setRGB2(&work->strip, r, g, b);
                setRGB3(&work->strip, r, g, b);
                depth = RotAverage4(&work->cone[j][0], &work->cone[j + 1][0], &work->cone[j][1], &work->cone[j + 1][1],
                    (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                    (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                }
            }
            work->cones[i].elapsed += step;
            if (work->cones[i].duration < work->cones[i].elapsed) {
                work->cones[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (i = 0; i < work->band_count; i++) {
        if (work->bands[i].active == 1) {
            unit.flg = 0;
            unit.matrix = D_800FE168;
            ScaleMatrix(&unit.matrix, &work->bands[i].scale);
            RotMatrixX(work->bands[i].rotation.vx, &unit.matrix);
            RotMatrixY(work->bands[i].rotation.vy, &unit.matrix);
            unit.matrix.t[0] = work->bands[i].position.vx;
            unit.matrix.t[1] = work->bands[i].position.vy;
            unit.matrix.t[2] = work->bands[i].position.vz;
            unit.super = work->bands[i].coord;
            GsGetLsUnit(&unit, &ls);
            GsSetLsMatrix(&ls);
            value = (4096 - i * 1000) * work->bands[i].elapsed / work->bands[i].duration;
            setVector(&work->bands[i].scale, value, value, value);
            if (work->bands[i].elapsed > (u32)work->bands[i].duration / 2) {
                value = (work->bands[i].duration - work->bands[i].elapsed) * 2;
                sign = work->bands[i].duration;
                work->bands[i].color[0] = D_8013D2E8[work->command].color[0] * value / sign;
                work->bands[i].color[1] = D_8013D2E8[work->command].color[1] * value / sign;
                work->bands[i].color[2] = D_8013D2E8[work->command].color[2] * value / sign;
            }
            for (j = 0; j < 16; j++) {
                setVector(&inner, (work->ring[j][0].vx + work->ring[j][1].vx) / 2,
                    (work->ring[j][0].vy + work->ring[j][1].vy) / 2,
                    (work->ring[j][0].vz + work->ring[j][1].vz) / 2);
                setVector(&outer, (work->ring[j + 1][0].vx + work->ring[j + 1][1].vx) / 2,
                    (work->ring[j + 1][0].vy + work->ring[j + 1][1].vy) / 2,
                    (work->ring[j + 1][0].vz + work->ring[j + 1][1].vz) / 2);
                setPolyG4(&work->band);
                r = work->bands[i].color[0];
                g = work->bands[i].color[1];
                b = work->bands[i].color[2];
                setRGB0(&work->band, 0, 0, 0);
                setRGB1(&work->band, 0, 0, 0);
                setRGB2(&work->band, r, g, b);
                setRGB3(&work->band, r, g, b);
                depth = RotAverage4(&work->ring[j][0], &work->ring[j + 1][0], &inner, &outer,
                    (PSXLONG *)&work->band.x0, (PSXLONG *)&work->band.x1,
                    (PSXLONG *)&work->band.x2, (PSXLONG *)&work->band.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->band, func_80058F10(), (u16)depth, work->bands[i].mode);
                }
                setPolyG4(&work->band);
                r = work->bands[i].color[0];
                g = work->bands[i].color[1];
                b = work->bands[i].color[2];
                setRGB2(&work->band, 0, 0, 0);
                setRGB3(&work->band, 0, 0, 0);
                setRGB0(&work->band, r, g, b);
                setRGB1(&work->band, r, g, b);
                depth = RotAverage4(&inner, &outer, &work->ring[j][1], &work->ring[j + 1][1],
                    (PSXLONG *)&work->band.x0, (PSXLONG *)&work->band.x1,
                    (PSXLONG *)&work->band.x2, (PSXLONG *)&work->band.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)&work->band, func_80058F10(), (u16)depth, work->bands[i].mode);
                }
            }
            work->bands[i].elapsed += step;
            if (work->bands[i].duration < work->bands[i].elapsed) {
                work->bands[i].active = 0;
            }
        } else {
            inactive++;
        }
    }
    for (j = 0; j < work->columns; j++) {
        for (i = 0; i < work->rows; i++) {
            if (work->grid[i][j].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->grid[i][j].scale);
                RotMatrixZ(work->grid[i][j].rotation.vz, &unit.matrix);
                RotMatrixX(work->grid[i][j].rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix);
                RotMatrixY(work->grid[i][j].rotation.vy - (view.yaw - 0xC06), &unit.matrix);
                unit.matrix.t[0] = work->grid[i][j].position.vx
                    + work->grid[i][j].target.vx * work->grid[i][j].elapsed / work->grid[i][j].duration;
                unit.matrix.t[1] = work->grid[i][j].position.vy
                    + work->grid[i][j].target.vy * work->grid[i][j].elapsed / work->grid[i][j].duration;
                unit.matrix.t[2] = work->grid[i][j].position.vz
                    + work->grid[i][j].target.vz * work->grid[i][j].elapsed / work->grid[i][j].duration;
                unit.super = work->grid[i][j].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->grid[i][j].elapsed > (u32)work->grid[i][j].duration / 2) {
                    value = (work->grid[i][j].duration - work->grid[i][j].elapsed) * 2;
                    sign = work->grid[i][j].duration;
                    work->grid[i][j].color[0] = D_8013D2E8[work->command].color[3] * value / sign;
                    work->grid[i][j].color[1] = D_8013D2E8[work->command].color[4] * value / sign;
                    work->grid[i][j].color[2] = D_8013D2E8[work->command].color[5] * value / sign;
                }
                setPolyFT4(&work->quad);
                work->quad.tpage = work->tpage[1];
                work->quad.clut = work->clut[1];
                setSemiTrans(&work->quad, 1);
                value = 1;
                setUV4(&work->quad, work->grid[i][j].u, work->grid[i][j].v,
                    work->grid[i][j].u + 31, work->grid[i][j].v,
                    work->grid[i][j].u, work->grid[i][j].v + 31,
                    work->grid[i][j].u + 31, work->grid[i][j].v + 31);
                r = work->grid[i][j].color[0];
                g = work->grid[i][j].color[1];
                b = work->grid[i][j].color[2];
                setRGB0(&work->quad, r, g, b);
                depth = RotAverage4(&work->corner[0], &work->corner[1], &work->corner[2], &work->corner[3],
                    (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                    (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                }
                work->grid[i][j].u += 32;
                if (work->grid[i][j].u == 224) {
                    work->grid[i][j].u = 0;
                }
                if (i == 0) {
                    work->grid[work->grid[0][j].cursor][j].active = value;
                    work->grid[work->grid[0][j].cursor][j].elapsed = 0;
                    work->grid[work->grid[0][j].cursor][j].duration = 32;
                    work->grid[work->grid[0][j].cursor][j].mode = value;
                    work->grid[work->grid[0][j].cursor][j].coord = Model_GetCameraCoordinateUnit();
                    work->grid[work->grid[0][j].cursor][j].color[0] = D_8013D2E8[work->command].color[3];
                    work->grid[work->grid[0][j].cursor][j].color[1] = D_8013D2E8[work->command].color[4];
                    work->grid[work->grid[0][j].cursor][j].color[2] = D_8013D2E8[work->command].color[5];
                    work->grid[work->grid[0][j].cursor][j].u = 0;
                    work->grid[work->grid[0][j].cursor][j].v = 128;
                    setVector(&work->grid[work->grid[0][j].cursor][j].scale, 4096, 4096, 4096);
                    setVector(&work->grid[work->grid[0][j].cursor][j].target, 0, 0, 0);
                    setVector(&work->grid[work->grid[0][j].cursor][j].rotation, 0, work->slot * 2048, 0);
                    (work->grid[work->grid[0][j].cursor] + j)->position.vx = unit.matrix.t[0];
                    (work->grid[work->grid[0][j].cursor] + j)->position.vy = unit.matrix.t[1];
                    (work->grid[work->grid[0][j].cursor] + j)->position.vz = unit.matrix.t[2];
                    work->grid[0][j].cursor++;
                    if (work->grid[0][j].cursor > work->rows - 1) {
                        work->grid[0][j].cursor = value;
                    }
                }
                work->grid[i][j].elapsed += step;
                if (work->grid[i][j].duration < work->grid[i][j].elapsed) {
                    work->grid[i][j].active = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
    }
    PopMatrix();
    work->elapsed += step;
    if (inactive == work->cone_count + work->band_count + work->rows * work->columns) {
        result = 1;
        if (work->flags[4] == 1) {
            result = 2;
        }
        work->flags[4] = 1;
    }
    return result;
}
