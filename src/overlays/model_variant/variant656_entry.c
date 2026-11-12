#include "../../types.h"
#include "variant656_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../psyq/abs.h"

/* Header 656's entry: spinning sprites rise from one model joint while a
 * textured cone and dome open at another, singly or one per spawn step. The
 * setup also prints the work-area size. */

s32 ModelVariant656_HandleRequest(u8 *context, s32 command)
{
    Model656State *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    Model656ViewSnapshot view;
    SVECTOR points[4];
    SVECTOR offset, moved;
    long interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 value, duration;
    u32 tex;
    long depth;

    work = (Model656State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model656State));
#ifndef VERSION_FRENCH
        printf(gModelVariant656_szWorkSizeFormat, sizeof(Model656State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 0, &gModelVariant656_aImages[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant656_aImages[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
            work->count = 1;
            work->dome_count = 8;
            break;
        case 1:
            work->count = 6;
            work->dome_count = 1;
            break;
        case 2:
            work->count = 14;
            work->dome_count = 1;
            break;
        }
        work->radius = gModelVariant656_aConfigs[command].radius;
        work->height = gModelVariant656_aConfigs[command].height;
        for (i = 0; i < work->count; i++) {
            work->sprites[i].head.active = 0;
            work->sprites[i].head.elapsed = 0;
            work->sprites[i].head.duration = 64;
            work->sprites[i].head.mode = 1;
            work->sprites[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->sprites[i].head.color[0] = gModelVariant656_aConfigs[command].color[3];
            work->sprites[i].head.color[1] = gModelVariant656_aConfigs[command].color[4];
            work->sprites[i].head.color[2] = gModelVariant656_aConfigs[command].color[5];
            work->sprites[i].head.u = 0;
            work->sprites[i].head.v = 0;
            setVector(&work->sprites[i].xform.scale, 4096, 4096, 4096);
            switch (i % 3) {
            case 0:
            default:
                value = 0;
                break;
            case 1:
                value = 100;
                break;
            case 2:
                value = -100;
                break;
            }
            setVector(&work->sprites[i].growth, value, 0, work->direction * 200);
            setVector(&work->sprites[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->sprites[i].xform.position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->cones[i].head.active = 0;
            work->cones[i].head.elapsed = 0;
            switch (command) {
            case 1:
            case 2:
                work->cones[i].head.duration = 1;
                break;
            default:
                work->cones[i].head.duration = 64;
                break;
            }
            work->cones[i].head.mode = 1;
            work->cones[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->cones[i].head.color[0] = gModelVariant656_aConfigs[command].color[0];
            work->cones[i].head.color[1] = gModelVariant656_aConfigs[command].color[1];
            work->cones[i].head.color[2] = gModelVariant656_aConfigs[command].color[2];
            work->cones[i].head.u = 0;
            work->cones[i].head.v = 16;
            setVector(&work->cones[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->cones[i].growth, 0, 0, 0);
            setVector(&work->cones[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->cones[i].xform.position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->domes[i].head.active = 0;
            work->domes[i].head.elapsed = 0;
            switch (command) {
            case 1:
            case 2:
                work->domes[i].head.duration = 64;
                break;
            default:
                work->domes[i].head.duration = 96;
                break;
            }
            work->domes[i].head.mode = 1;
            work->domes[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->domes[i].head.color[0] = gModelVariant656_aConfigs[command].color[0];
            work->domes[i].head.color[1] = gModelVariant656_aConfigs[command].color[1];
            work->domes[i].head.color[2] = gModelVariant656_aConfigs[command].color[2];
            work->domes[i].head.u = 0;
            work->domes[i].head.v = 16;
            setVector(&work->domes[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->domes[i].growth, 0, 0, 0);
            setVector(&work->domes[i].xform.rotation, 0, work->head.slot * 2048, (i << 12) / work->count);
            setVector(&work->domes[i].xform.position, 0, 0, 0);
        }
        work->delay = gModelVariant656_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        switch (command) {
        case 1:
        case 2:
            setVector(&work->corners[0], -work->radius / 3, -work->radius / 3, 0);
            setVector(&work->corners[1], work->radius / 3, -work->radius / 3, 0);
            setVector(&work->corners[2], -work->radius / 3, work->radius / 3, 0);
            setVector(&work->corners[3], work->radius / 3, work->radius / 3, 0);
            break;
        default:
            setVector(&work->corners[0], -work->radius / 2, -work->radius / 2, 0);
            setVector(&work->corners[1], work->radius / 2, -work->radius / 2, 0);
            setVector(&work->corners[2], -work->radius / 2, work->radius / 2, 0);
            setVector(&work->corners[3], work->radius / 2, work->radius / 2, 0);
            break;
        }
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 9; j++) {
                setVector(&work->cone[j][i],
                    work->radius * -(i * -5) / 8 * rsin(j << 9) / 4096,
                    work->radius * -(i * -5) / 8 * rcos(j << 9) / 4096,
                    work->height * -rcos(i << 7) / 4096);
            }
        }
        for (i = 0; i < 9; i++) {
            setVector(&work->rim[i], work->radius * rsin(i << 9) / 4096, work->radius * rcos(i << 9) / 4096, 0);
        }
        for (i = 0; i < 9; i++) {
            setVector(&work->inner[i], work->radius * -(rsin(i << 9) * -5) / 4096,
                work->radius * -(rcos(i << 9) * -5) / 4096, 0);
            setVector(&work->outer[i], work->radius * -(rsin(i << 9) * -10) / 4096,
                work->radius * -(rcos(i << 9) * -10) / 4096, work->height / -4);
        }
        for (j = 0; j < work->dome_count; j++) {
            for (i = 0; i < 9; i++) {
                setVector(&work->dome_inner[i][j],
                    work->radius * -(rcos((j << 10) / work->dome_count) * -6) / 4096 * rsin((j % 2 << 8) + (i << 9)) / 4096,
                    work->radius * -(rcos((j << 10) / work->dome_count) * -6) / 4096 * rcos((j % 2 << 8) + (i << 9)) / 4096,
                    0);
                setVector(&work->dome_outer[i][j],
                    work->radius * -(rcos((j << 10) / work->dome_count) * -10) / 4096 * rsin((j % 2 << 8) + (i << 9)) / 4096,
                    work->radius * -(rcos((j << 10) / work->dome_count) * -10) / 4096 * rcos((j % 2 << 8) + (i << 9)) / 4096,
                    (work->height >> 2) * rcos((j << 10) / work->dome_count) / 4096);
            }
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->head.command) {
        case 0:
            if (work->head.elapsed == 0) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 18);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->count; i++) {
                    work->sprites[i].head.active = 1;
                    setVector(&work->sprites[i].xform.position, ls.t[0], ls.t[1], ls.t[2]);
                }
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 21);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->count; i++) {
                    work->cones[i].head.active = 1;
                    setVector(&work->cones[i].xform.position, ls.t[0], ls.t[1], ls.t[2]);
                    work->joints[i] = ls;
                    work->domes[i].head.active = 1;
                    setVector(&offset, 0, 0, ABS(ls.t[2]) + 450);
                    PushMatrix();
                    ApplyMatrixSV(&ls, &offset, &moved);
                    PopMatrix();
                    setVector(&work->domes[i].xform.position, moved.vx + ls.t[0], moved.vy + ls.t[1], work->direction * 450);
                }
            }
            break;
        case 1:
            if (work->head.elapsed >= work->flags[0] * 8 && work->flags[0] < work->count) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 18);
                GsGetLwUnit(&unit, &ls);
                work->sprites[work->flags[0]].head.active = 1;
                setVector(&work->sprites[work->flags[0]].xform.position, ls.t[0], ls.t[1], ls.t[2]);
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 21);
                GsGetLwUnit(&unit, &ls);
                work->cones[work->flags[0]].head.active = 1;
                setVector(&work->cones[work->flags[0]].xform.position, ls.t[0], ls.t[1], ls.t[2]);
                work->joints[work->flags[0]] = ls;
                work->domes[work->flags[0]].head.active = 1;
                setVector(&offset, 0, 0, ABS(ls.t[2]) + 450);
                PushMatrix();
                ApplyMatrixSV(&ls, &offset, &moved);
                PopMatrix();
                setVector(&work->domes[work->flags[0]].xform.position, moved.vx + ls.t[0], moved.vy + ls.t[1],
                    work->direction * 450);
                work->flags[0]++;
            }
            break;
        case 2:
            if (work->head.elapsed >= work->flags[0] * 8 && work->flags[0] < work->count) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 21);
                GsGetLwUnit(&unit, &ls);
                work->sprites[work->flags[0]].head.active = 1;
                setVector(&work->sprites[work->flags[0]].xform.position, ls.t[0], ls.t[1], ls.t[2]);
                m = ls;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, 20);
                GsGetLwUnit(&unit, &ls);
                work->cones[work->flags[0]].head.active = 1;
                setVector(&work->cones[work->flags[0]].xform.position, ls.t[0], ls.t[1], ls.t[2]);
                work->joints[work->flags[0]] = ls;
                work->domes[work->flags[0]].head.active = 1;
                setVector(&offset, 0, 0, -(ABS(ls.t[2]) + 450));
                PushMatrix();
                RotMatrixY(0x400, &work->joints[work->flags[0]]);
                RotMatrixX(-(work->direction * 4096) / 32, &work->joints[work->flags[0]]);
                ApplyMatrixSV(&work->joints[work->flags[0]], &offset, &moved);
                PopMatrix();
                setVector(&work->domes[work->flags[0]].xform.position, moved.vx + ls.t[0], moved.vy + ls.t[1],
                    work->direction * 450);
                work->flags[0]++;
            }
            break;
        }
        PushMatrix();
        view = *(Model656ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->sprites[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->sprites[i].xform.scale);
                RotMatrixZ(work->sprites[i].xform.rotation.vz, &unit.matrix);
                RotMatrixX(work->sprites[i].xform.rotation.vx - (view.u.halves[2] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->sprites[i].xform.rotation.vy + (0xC06 - view.u.halves[1]), &unit.matrix);
                unit.matrix.t[0] = work->sprites[i].xform.position.vx
                    + work->sprites[i].growth.vx * work->sprites[i].head.elapsed / work->sprites[i].head.duration;
                {
                    s32 h, t, y;

                    /* The always-zero masked shifts keep the arc height and base out of
                       CSE's constant/register view, matching the retail register use. */
                    h = 200 + (((u32)i << 8) & 0xFF);
                    t = work->sprites[i].head.elapsed * 3;
                    y = work->sprites[i].xform.position.vy;
                    unit.matrix.t[1] = h * t * (t - work->sprites[i].head.duration * 2)
                        / (work->sprites[i].head.duration * work->sprites[i].head.duration) + y + (((u32)y << 8) & 0xFF);
                }
                unit.matrix.t[2] = work->sprites[i].xform.position.vz
                    + work->sprites[i].growth.vz * work->sprites[i].head.elapsed / work->sprites[i].head.duration;
                unit.super = work->sprites[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setUV4(&work->sprite, work->sprites[i].head.u, work->sprites[i].head.v,
                    work->sprites[i].head.u + 15, work->sprites[i].head.v,
                    work->sprites[i].head.u, work->sprites[i].head.v + 15,
                    work->sprites[i].head.u + 15, work->sprites[i].head.v + 15);
                setRGB0(&work->sprite, work->sprites[i].head.color[0], work->sprites[i].head.color[1], work->sprites[i].head.color[2]);
                PushMatrix();
                m = GsIDMATRIX;
                RotMatrixZ(work->sprites[i].head.elapsed * 100, &m);
                for (j = 0; j < 4; j++) {
                    ApplyMatrixSV(&m, &work->corners[j], &points[j]);
                }
                PopMatrix();
                depth = RotAverage4(&points[0], &points[1], &points[2], &points[3],
                    (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                    (long *)&work->sprite.x2, (long *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                }
                work->sprites[i].head.elapsed += step;
                if (work->sprites[i].head.duration < work->sprites[i].head.elapsed) {
                    work->sprites[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->cones[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = work->joints[i];
                value = (work->cones[i].head.elapsed << 11) / work->cones[i].head.duration + 0x800;
                setVector(&work->cones[i].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->cones[i].xform.scale);
                unit.matrix.t[0] = work->cones[i].xform.position.vx;
                unit.matrix.t[1] = work->cones[i].xform.position.vy;
                unit.matrix.t[2] = work->cones[i].xform.position.vz;
                unit.super = work->cones[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->cones[i].head.duration / 2 < work->cones[i].head.elapsed) {
                    value = (work->cones[i].head.duration - work->cones[i].head.elapsed) * 2;
                    duration = work->cones[i].head.duration;
                    work->cones[i].head.color[0] = gModelVariant656_aConfigs[work->head.command].color[0] * value / duration;
                    work->cones[i].head.color[1] = gModelVariant656_aConfigs[work->head.command].color[1] * value / duration;
                    work->cones[i].head.color[2] = gModelVariant656_aConfigs[work->head.command].color[2] * value / duration;
                }
                for (j = 0; j < 8; j++) {
                    for (k = 0; k < 8; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[1];
                        work->strip.clut = work->clut[1];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->cones[i].head.u, work->cones[i].head.v,
                            work->cones[i].head.u + 31, work->cones[i].head.v,
                            work->cones[i].head.u, work->cones[i].head.v + 127,
                            work->cones[i].head.u + 31, work->cones[i].head.v + 127);
                        setRGB0(&work->strip, work->cones[i].head.color[0], work->cones[i].head.color[1], work->cones[i].head.color[2]);
                        setRGB1(&work->strip, work->cones[i].head.color[0], work->cones[i].head.color[1], work->cones[i].head.color[2]);
                        setRGB2(&work->strip, 0, 0, 0);
                        setRGB3(&work->strip, 0, 0, 0);
                        depth = RotAverage4(&work->rim[k], &work->rim[k + 1],
                            &work->cone[k][j], &work->cone[k + 1][j],
                            (long *)&work->strip.x0, (long *)&work->strip.x1,
                            (long *)&work->strip.x2, (long *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                for (j = 0; j < 8; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->cones[i].head.u, work->cones[i].head.v,
                        work->cones[i].head.u + 31, work->cones[i].head.v,
                        work->cones[i].head.u, work->cones[i].head.v + 127,
                        work->cones[i].head.u + 31, work->cones[i].head.v + 127);
                    setRGB0(&work->strip, work->cones[i].head.color[0], work->cones[i].head.color[1], work->cones[i].head.color[2]);
                    setRGB1(&work->strip, work->cones[i].head.color[0], work->cones[i].head.color[1], work->cones[i].head.color[2]);
                    setRGB2(&work->strip, 0, 0, 0);
                    setRGB3(&work->strip, 0, 0, 0);
                    depth = RotAverage4(&work->inner[j], &work->inner[j + 1],
                        &work->outer[j], &work->outer[j + 1],
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
        for (i = 0; i < work->count; i++) {
            if (work->domes[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                value = (work->domes[i].head.elapsed << 13) / work->domes[i].head.duration;
                setVector(&work->domes[i].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->domes[i].xform.scale);
                RotMatrixZ(work->domes[i].xform.rotation.vz, &unit.matrix);
                RotMatrixY(work->domes[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->domes[i].xform.position.vx;
                unit.matrix.t[1] = work->domes[i].xform.position.vy;
                unit.matrix.t[2] = work->domes[i].xform.position.vz;
                unit.super = work->domes[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->domes[i].head.duration / 2 < work->domes[i].head.elapsed) {
                    value = (work->domes[i].head.duration - work->domes[i].head.elapsed) * 2;
                    duration = work->domes[i].head.duration;
                    work->domes[i].head.color[0] = gModelVariant656_aConfigs[work->head.command].color[0] * value / duration;
                    work->domes[i].head.color[1] = gModelVariant656_aConfigs[work->head.command].color[1] * value / duration;
                    work->domes[i].head.color[2] = gModelVariant656_aConfigs[work->head.command].color[2] * value / duration;
                }
                for (k = 0; k < work->dome_count; k++) {
                    for (j = 0; j < 8; j++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[1];
                        work->strip.clut = work->clut[1];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->domes[i].head.u, work->domes[i].head.v,
                            work->domes[i].head.u + 31, work->domes[i].head.v,
                            work->domes[i].head.u, work->domes[i].head.v + 127,
                            work->domes[i].head.u + 31, work->domes[i].head.v + 127);
                        setRGB2(&work->strip, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                        setRGB3(&work->strip, work->domes[i].head.color[0], work->domes[i].head.color[1], work->domes[i].head.color[2]);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        depth = RotAverage4(&work->dome_inner[j][k], &work->dome_inner[j + 1][k],
                            &work->dome_outer[j][k], &work->dome_outer[j + 1][k],
                            (long *)&work->strip.x0, (long *)&work->strip.x1,
                            (long *)&work->strip.x2, (long *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                work->domes[i].head.elapsed += step;
                if (work->domes[i].head.duration < work->domes[i].head.elapsed) {
                    work->domes[i].head.active = 0;
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
