#include "../../types.h"
#include "variant656_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../psyq/abs.h"

/* Header 656's entry: spinning sprites rise from one model joint while a
 * textured cone and dome open at another, singly or one per spawn step. The
 * setup also prints the work-area size. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model656State *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    Model656ViewSnapshot view;
    SVECTOR points[4];
    SVECTOR offset, moved;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 value, duration;
    u32 tex;
    PSXLONG depth;

    work = (Model656State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model656State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model656State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 0, &D_8013D3E0[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D3E0[1]);
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
        work->radius = D_8013D418[command].radius;
        work->height = D_8013D418[command].height;
        for (i = 0; i < work->count; i++) {
            work->sprites[i].active = 0;
            work->sprites[i].elapsed = 0;
            work->sprites[i].duration = 64;
            work->sprites[i].mode = 1;
            work->sprites[i].coord = Model_GetCameraCoordinateUnit();
            work->sprites[i].color[0] = D_8013D418[command].color[3];
            work->sprites[i].color[1] = D_8013D418[command].color[4];
            work->sprites[i].color[2] = D_8013D418[command].color[5];
            work->sprites[i].u = 0;
            work->sprites[i].v = 0;
            setVector(&work->sprites[i].scale, 4096, 4096, 4096);
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
            setVector(&work->sprites[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->sprites[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->cones[i].active = 0;
            work->cones[i].elapsed = 0;
            switch (command) {
            case 1:
            case 2:
                work->cones[i].duration = 1;
                break;
            default:
                work->cones[i].duration = 64;
                break;
            }
            work->cones[i].mode = 1;
            work->cones[i].coord = Model_GetCameraCoordinateUnit();
            work->cones[i].color[0] = D_8013D418[command].color[0];
            work->cones[i].color[1] = D_8013D418[command].color[1];
            work->cones[i].color[2] = D_8013D418[command].color[2];
            work->cones[i].u = 0;
            work->cones[i].v = 16;
            setVector(&work->cones[i].scale, 4096, 4096, 4096);
            setVector(&work->cones[i].growth, 0, 0, 0);
            setVector(&work->cones[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->cones[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->domes[i].active = 0;
            work->domes[i].elapsed = 0;
            switch (command) {
            case 1:
            case 2:
                work->domes[i].duration = 64;
                break;
            default:
                work->domes[i].duration = 96;
                break;
            }
            work->domes[i].mode = 1;
            work->domes[i].coord = Model_GetCameraCoordinateUnit();
            work->domes[i].color[0] = D_8013D418[command].color[0];
            work->domes[i].color[1] = D_8013D418[command].color[1];
            work->domes[i].color[2] = D_8013D418[command].color[2];
            work->domes[i].u = 0;
            work->domes[i].v = 16;
            setVector(&work->domes[i].scale, 4096, 4096, 4096);
            setVector(&work->domes[i].growth, 0, 0, 0);
            setVector(&work->domes[i].rotation, 0, work->slot * 2048, (i << 12) / work->count);
            setVector(&work->domes[i].position, 0, 0, 0);
        }
        work->delay = D_8013D418[command].delay;
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
        switch (work->command) {
        case 0:
            if (work->elapsed == 0) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 18);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->count; i++) {
                    work->sprites[i].active = 1;
                    setVector(&work->sprites[i].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 21);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->count; i++) {
                    work->cones[i].active = 1;
                    setVector(&work->cones[i].position, ls.t[0], ls.t[1], ls.t[2]);
                    work->joints[i] = ls;
                    work->domes[i].active = 1;
                    setVector(&offset, 0, 0, ABS(ls.t[2]) + 450);
                    PushMatrix();
                    ApplyMatrixSV(&ls, &offset, &moved);
                    PopMatrix();
                    setVector(&work->domes[i].position, moved.vx + ls.t[0], moved.vy + ls.t[1], work->direction * 450);
                }
            }
            break;
        case 1:
            if (work->elapsed >= work->flags[0] * 8 && work->flags[0] < work->count) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 18);
                GsGetLwUnit(&unit, &ls);
                work->sprites[work->flags[0]].active = 1;
                setVector(&work->sprites[work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 21);
                GsGetLwUnit(&unit, &ls);
                work->cones[work->flags[0]].active = 1;
                setVector(&work->cones[work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
                work->joints[work->flags[0]] = ls;
                work->domes[work->flags[0]].active = 1;
                setVector(&offset, 0, 0, ABS(ls.t[2]) + 450);
                PushMatrix();
                ApplyMatrixSV(&ls, &offset, &moved);
                PopMatrix();
                setVector(&work->domes[work->flags[0]].position, moved.vx + ls.t[0], moved.vy + ls.t[1],
                    work->direction * 450);
                work->flags[0]++;
            }
            break;
        case 2:
            if (work->elapsed >= work->flags[0] * 8 && work->flags[0] < work->count) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 21);
                GsGetLwUnit(&unit, &ls);
                work->sprites[work->flags[0]].active = 1;
                setVector(&work->sprites[work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
                m = ls;
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 20);
                GsGetLwUnit(&unit, &ls);
                work->cones[work->flags[0]].active = 1;
                setVector(&work->cones[work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
                work->joints[work->flags[0]] = ls;
                work->domes[work->flags[0]].active = 1;
                setVector(&offset, 0, 0, -(ABS(ls.t[2]) + 450));
                PushMatrix();
                RotMatrixY(0x400, &work->joints[work->flags[0]]);
                RotMatrixX(-(work->direction * 4096) / 32, &work->joints[work->flags[0]]);
                ApplyMatrixSV(&work->joints[work->flags[0]], &offset, &moved);
                PopMatrix();
                setVector(&work->domes[work->flags[0]].position, moved.vx + ls.t[0], moved.vy + ls.t[1],
                    work->direction * 450);
                work->flags[0]++;
            }
            break;
        }
        PushMatrix();
        view = *(Model656ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->sprites[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->sprites[i].scale);
                RotMatrixZ(work->sprites[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->sprites[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->sprites[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->sprites[i].position.vx
                    + work->sprites[i].growth.vx * work->sprites[i].elapsed / work->sprites[i].duration;
                {
                    s32 h, t, y;

                    /* The always-zero masked shifts keep the arc height and base out of
                       CSE's constant/register view, matching the retail register use. */
                    h = 200 + (((u32)i << 8) & 0xFF);
                    t = work->sprites[i].elapsed * 3;
                    y = work->sprites[i].position.vy;
                    unit.matrix.t[1] = h * t * (t - work->sprites[i].duration * 2)
                        / (work->sprites[i].duration * work->sprites[i].duration) + y + (((u32)y << 8) & 0xFF);
                }
                unit.matrix.t[2] = work->sprites[i].position.vz
                    + work->sprites[i].growth.vz * work->sprites[i].elapsed / work->sprites[i].duration;
                unit.super = work->sprites[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setUV4(&work->sprite, work->sprites[i].u, work->sprites[i].v,
                    work->sprites[i].u + 15, work->sprites[i].v,
                    work->sprites[i].u, work->sprites[i].v + 15,
                    work->sprites[i].u + 15, work->sprites[i].v + 15);
                setRGB0(&work->sprite, work->sprites[i].color[0], work->sprites[i].color[1], work->sprites[i].color[2]);
                PushMatrix();
                m = D_800FE168;
                RotMatrixZ(work->sprites[i].elapsed * 100, &m);
                for (j = 0; j < 4; j++) {
                    ApplyMatrixSV(&m, &work->corners[j], &points[j]);
                }
                PopMatrix();
                depth = RotAverage4(&points[0], &points[1], &points[2], &points[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                work->sprites[i].elapsed += step;
                if (work->sprites[i].duration < work->sprites[i].elapsed) {
                    work->sprites[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->cones[i].active == 1) {
                unit.flg = 0;
                unit.matrix = work->joints[i];
                value = (work->cones[i].elapsed << 11) / work->cones[i].duration + 0x800;
                setVector(&work->cones[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->cones[i].scale);
                unit.matrix.t[0] = work->cones[i].position.vx;
                unit.matrix.t[1] = work->cones[i].position.vy;
                unit.matrix.t[2] = work->cones[i].position.vz;
                unit.super = work->cones[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->cones[i].duration / 2 < work->cones[i].elapsed) {
                    value = (work->cones[i].duration - work->cones[i].elapsed) * 2;
                    duration = work->cones[i].duration;
                    work->cones[i].color[0] = D_8013D418[work->command].color[0] * value / duration;
                    work->cones[i].color[1] = D_8013D418[work->command].color[1] * value / duration;
                    work->cones[i].color[2] = D_8013D418[work->command].color[2] * value / duration;
                }
                for (j = 0; j < 8; j++) {
                    for (k = 0; k < 8; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[1];
                        work->strip.clut = work->clut[1];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->cones[i].u, work->cones[i].v,
                            work->cones[i].u + 31, work->cones[i].v,
                            work->cones[i].u, work->cones[i].v + 127,
                            work->cones[i].u + 31, work->cones[i].v + 127);
                        setRGB0(&work->strip, work->cones[i].color[0], work->cones[i].color[1], work->cones[i].color[2]);
                        setRGB1(&work->strip, work->cones[i].color[0], work->cones[i].color[1], work->cones[i].color[2]);
                        setRGB2(&work->strip, 0, 0, 0);
                        setRGB3(&work->strip, 0, 0, 0);
                        depth = RotAverage4(&work->rim[k], &work->rim[k + 1],
                            &work->cone[k][j], &work->cone[k + 1][j],
                            (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                            (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                        }
                    }
                }
                for (j = 0; j < 8; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->cones[i].u, work->cones[i].v,
                        work->cones[i].u + 31, work->cones[i].v,
                        work->cones[i].u, work->cones[i].v + 127,
                        work->cones[i].u + 31, work->cones[i].v + 127);
                    setRGB0(&work->strip, work->cones[i].color[0], work->cones[i].color[1], work->cones[i].color[2]);
                    setRGB1(&work->strip, work->cones[i].color[0], work->cones[i].color[1], work->cones[i].color[2]);
                    setRGB2(&work->strip, 0, 0, 0);
                    setRGB3(&work->strip, 0, 0, 0);
                    depth = RotAverage4(&work->inner[j], &work->inner[j + 1],
                        &work->outer[j], &work->outer[j + 1],
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
        for (i = 0; i < work->count; i++) {
            if (work->domes[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->domes[i].elapsed << 13) / work->domes[i].duration;
                setVector(&work->domes[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->domes[i].scale);
                RotMatrixZ(work->domes[i].rotation.vz, &unit.matrix);
                RotMatrixY(work->domes[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->domes[i].position.vx;
                unit.matrix.t[1] = work->domes[i].position.vy;
                unit.matrix.t[2] = work->domes[i].position.vz;
                unit.super = work->domes[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->domes[i].duration / 2 < work->domes[i].elapsed) {
                    value = (work->domes[i].duration - work->domes[i].elapsed) * 2;
                    duration = work->domes[i].duration;
                    work->domes[i].color[0] = D_8013D418[work->command].color[0] * value / duration;
                    work->domes[i].color[1] = D_8013D418[work->command].color[1] * value / duration;
                    work->domes[i].color[2] = D_8013D418[work->command].color[2] * value / duration;
                }
                for (k = 0; k < work->dome_count; k++) {
                    for (j = 0; j < 8; j++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[1];
                        work->strip.clut = work->clut[1];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->domes[i].u, work->domes[i].v,
                            work->domes[i].u + 31, work->domes[i].v,
                            work->domes[i].u, work->domes[i].v + 127,
                            work->domes[i].u + 31, work->domes[i].v + 127);
                        setRGB2(&work->strip, work->domes[i].color[0], work->domes[i].color[1], work->domes[i].color[2]);
                        setRGB3(&work->strip, work->domes[i].color[0], work->domes[i].color[1], work->domes[i].color[2]);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        depth = RotAverage4(&work->dome_inner[j][k], &work->dome_inner[j + 1][k],
                            &work->dome_outer[j][k], &work->dome_outer[j + 1][k],
                            (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                            (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                        }
                    }
                }
                work->domes[i].elapsed += step;
                if (work->domes[i].duration < work->domes[i].elapsed) {
                    work->domes[i].active = 0;
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
