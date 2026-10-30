#include "../../types.h"
#include "variant682us_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 682's entry: strip columns that spawn at a tracked model point and
 * fan out as sixteen Gouraud strips, a burst of 32 rays with Gouraud
 * triangles and a spinning sixteen-strip fan, and a grid of animated spark
 * sprites pinned to six model points. Only command 0 has a timetable; the
 * setup also prints the work-area size. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model682UsState *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    Model682UsViewSnapshot view;
    /* P1 is never used, but its slot keeps the retail stack layout. */
    SVECTOR p0, p1;
    SVECTOR pts[4][2];
    VECTOR scale;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    /* J doubles as every fade divisor and the strip-weight divisor 8; the
     * register allocation depends on that sharing. */
    s32 i, j, k, value;
    u16 red, green, blue;
    s32 d0, d1, angle;
    u16 red1, green1, blue1;
    s32 r, g, b;
    u32 tex;
    PSXLONG depth;

    work = (Model682UsState *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model682UsState));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model682UsState));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        /* The three texture pages are uploaded from the end of the table. */
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D0B0[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D0B0[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D0B0[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        if (command == 0) {
            work->column_count = 64;
            work->burst_count = 1;
            work->spark_columns = 6;
            work->spark_rows = 16;
        }
        work->radius = D_8013D104[command].radius;
        work->height = D_8013D104[command].height;
        for (i = 0; i < work->column_count; i++) {
            work->columns[i].active = 0;
            work->columns[i].elapsed = 0;
            work->columns[i].duration = 32;
            work->columns[i].mode = 1;
            work->columns[i].coord = Model_GetCameraCoordinateUnit();
            work->columns[i].color[0] = D_8013D104[command].color[0];
            work->columns[i].color[1] = D_8013D104[command].color[1];
            work->columns[i].color[2] = D_8013D104[command].color[2];
            work->columns[i].u = 0x40;
            work->columns[i].v = 0;
            value = 0x800;
            setVector(&work->columns[i].scale, value, value, value);
            setVector(&work->columns[i].unknown_3C, 0, 0, 0);
            setVector(&work->columns[i].rotation, 0, 0, 0);
            setVector(&work->columns[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].duration = 48;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].color[0] = D_8013D104[command].color[3];
            work->bursts[i].color[1] = D_8013D104[command].color[4];
            work->bursts[i].color[2] = D_8013D104[command].color[5];
            work->bursts[i].u = 0x40;
            work->bursts[i].v = 0;
            value = 0x800;
            setVector(&work->bursts[i].scale, value, value, value);
            setVector(&work->bursts[i].unknown_3C, 0, 0, 0);
            setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
            if (command == 0) {
                setVector(&work->bursts[i].position, 0, 100, 0);
            } else {
                setVector(&work->bursts[i].position, 0, 0, 0);
            }
            work->bursts[i].position.vy -= 350;
            work->bursts[i].position.vz += work->direction * 450;
        }
        for (k = 0; k < work->spark_columns; k++) {
            for (i = 0; i < work->spark_rows; i++) {
                work->sparks[i][k].active = 0;
                work->sparks[i][k].elapsed = 0;
                work->sparks[i][k].duration = 32;
                work->sparks[i][k].coord = Model_GetCameraCoordinateUnit();
                work->sparks[i][k].color[0] = D_8013D104[command].color[3];
                work->sparks[i][k].color[1] = D_8013D104[command].color[4];
                work->sparks[i][k].color[2] = D_8013D104[command].color[5];
                work->sparks[i][k].u = 0;
                setVector(&work->sparks[i][k].position, 0, 0, 0);
            }
        }
        work->delay = D_8013D104[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        if (command == 0) {
            for (i = 0; i < 17; i++) {
                setVector(&work->spokes[i][0], 0, work->radius * -rsin(i << 7) / 4096,
                    work->radius * -rcos(i << 7) / 4096);
                /* Negated as "* -1" so the sum is formed in the sine's register. */
                setVector(&work->spokes[i][1], 0,
                    (work->radius * rsin(i << 7) / 4096 + work->height) * -1,
                    work->radius * -rcos(i << 7) / 4096);
            }
        }
        for (i = 0; i < 33; i++) {
            setVector(&work->rays[i][0], work->radius * rcos(i << 7) / 4096,
                work->radius * rsin(i << 7) / 4096, 0);
            setVector(&work->rays[i][1], (work->radius * 2 + work->radius * (i % 2)) * rcos(i << 7) / 4096,
                (work->radius * 2 + work->radius * (i % 2)) * rsin(i << 7) / 4096, 0);
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->fan[i][0], (work->radius >> 1) * rsin(i << 8) / 4096,
                (work->radius >> 1) * rcos(i << 8) / 4096, 0);
            setVector(&work->fan[i][1], (work->radius * 3 >> 1) * rsin(i << 8) / 4096,
                (work->radius * 3 >> 1) * rcos(i << 8) / 4096, 0);
        }
        value = work->radius >> 2;
        setVector(&work->quad[0], -value, -value, 0);
        setVector(&work->quad[1], value, -value, 0);
        setVector(&work->quad[2], -value, value, 0);
        setVector(&work->quad[3], value, value, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        if (work->command == 0) {
            if (work->elapsed >= 7 && work->elapsed < 176) {
                work->columns[work->flags[0]].active = 1;
                work->columns[work->flags[0]].elapsed = 0;
                work->columns[work->flags[0]].duration = 32;
                work->columns[work->flags[0]].mode = 1;
                work->columns[work->flags[0]].coord = Model_GetCameraCoordinateUnit();
                work->columns[work->flags[0]].color[0] = D_8013D104[work->command].color[0];
                work->columns[work->flags[0]].color[1] = D_8013D104[work->command].color[1];
                work->columns[work->flags[0]].color[2] = D_8013D104[work->command].color[2];
                work->columns[work->flags[0]].u = 0x40;
                work->columns[work->flags[0]].v = 0;
                value = 0x800;
                setVector(&work->columns[work->flags[0]].scale, value, value, value);
                setVector(&work->columns[work->flags[0]].unknown_3C, 0, 0, 0);
                setVector(&work->columns[work->flags[0]].rotation, 0, 0, 0);
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 2);
                GsGetLwUnit(&unit, &ls);
                p0.vx = ls.t[0];
                p0.vy = ls.t[1];
                p0.vz = ls.t[2];
                work->matrices[work->flags[0]] = ls;
                setVector(&work->columns[work->flags[0]].position, p0.vx, p0.vy, p0.vz);
                work->flags[0]++;
                if (work->flags[0] >= work->column_count) {
                    work->flags[0] = 0;
                }
            }
            if (work->elapsed < 224) {
                for (i = 0; i < work->spark_columns; i++) {
                    work->sparks[work->flags[2]][i].active = 1;
                    work->sparks[work->flags[2]][i].elapsed = 0;
                    work->sparks[work->flags[2]][i].u = (i * 5 + work->flags[2] * 3) % 8 * 32;
                    work->sparks[work->flags[2]][i].color[0] = D_8013D104[work->command].color[3];
                    work->sparks[work->flags[2]][i].color[1] = D_8013D104[work->command].color[4];
                    work->sparks[work->flags[2]][i].color[2] = D_8013D104[work->command].color[5];
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, i + 34);
                    GsGetLwUnit(&unit, &ls);
                    setVector(&work->sparks[work->flags[2]][i].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                work->flags[2]++;
                if (work->flags[2] >= work->spark_rows) {
                    work->flags[2] = 0;
                }
            }
            if (work->elapsed >= 143 && work->flags[1] == 0) {
                work->bursts[0].active = 1;
                work->flags[1] = 1;
                result = 1;
            }
        }
        PushMatrix();
        view = *(Model682UsViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->column_count; i++) {
            if (work->columns[i].active == 1) {
                unit.flg = 0;
                unit.matrix = work->matrices[i];
                value = (work->columns[i].elapsed << 13) / work->columns[i].duration + 0x800;
                setVector(&work->columns[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->columns[i].scale);
                unit.matrix.t[0] = work->columns[i].position.vx;
                unit.matrix.t[1] = work->columns[i].position.vy;
                unit.matrix.t[2] = work->columns[i].position.vz;
                unit.super = work->columns[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->columns[i].duration / 2 < work->columns[i].elapsed) {
                    value = (work->columns[i].duration - work->columns[i].elapsed) * 2;
                    j = work->columns[i].duration;
                    work->columns[i].color[0] = D_8013D104[work->command].color[0] * value / j;
                    work->columns[i].color[1] = D_8013D104[work->command].color[1] * value / j;
                    work->columns[i].color[2] = D_8013D104[work->command].color[2] * value / j;
                }
                /* Each strip fades toward its ends: 8 - |8 - k| on the inner
                 * corners and 8 - |7 - k| on the outer ones, in eighths. */
                for (k = 0, j = 8; k < 16; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[0];
                    work->strip.clut = work->clut[0];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, work->columns[i].u, work->columns[i].v,
                        work->columns[i].u + 31, work->columns[i].v,
                        work->columns[i].u, work->columns[i].v + 127,
                        work->columns[i].u + 31, work->columns[i].v + 127);
                    d0 = 8 - k;
                    value = 8 - ABS(d0);
                    red = work->columns[i].color[0] * value / j;
                    green = work->columns[i].color[1] * value / j;
                    blue = work->columns[i].color[2] * value / j;
                    d1 = 7 - k;
                    value = j - ABS(d1);
                    setRGB0(&work->strip, red, green, blue);
                    setRGB2(&work->strip, red, green, blue);
                    red1 = work->columns[i].color[0] * value / j;
                    green1 = work->columns[i].color[1] * value / j;
                    blue1 = work->columns[i].color[2] * value / j;
                    setRGB1(&work->strip, red1, green1, blue1);
                    setRGB3(&work->strip, red1, green1, blue1);
                    depth = RotAverage4(&work->spokes[k][0], &work->spokes[k + 1][0],
                        &work->spokes[k][1], &work->spokes[k + 1][1],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
                work->columns[i].elapsed += step;
                if (work->columns[i].duration < work->columns[i].elapsed) {
                    work->columns[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            if (work->bursts[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->bursts[i].elapsed << 13) / work->bursts[i].duration + 0x800;
                setVector(&work->bursts[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->bursts[i].scale);
                RotMatrixZ(work->bursts[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->bursts[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->bursts[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->bursts[i].position.vx;
                unit.matrix.t[1] = work->bursts[i].position.vy;
                unit.matrix.t[2] = work->bursts[i].position.vz;
                unit.super = work->bursts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->bursts[i].duration / 2 < work->bursts[i].elapsed) {
                    value = (work->bursts[i].duration - work->bursts[i].elapsed) * 2;
                    j = work->bursts[i].duration;
                    work->bursts[i].color[0] = D_8013D104[work->command].color[3] * value / j;
                    work->bursts[i].color[1] = D_8013D104[work->command].color[4] * value / j;
                    work->bursts[i].color[2] = D_8013D104[work->command].color[5] * value / j;
                }
                /* P0 becomes the burst centre shared by every ray triangle. */
                setVector(&p0, 0, 0, 0);
                for (k = 0; k < 32; k++) {
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[0];
                    work->sprite.clut = work->clut[0];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->bursts[i].u, work->bursts[i].v,
                        work->bursts[i].u + 31, work->bursts[i].v,
                        work->bursts[i].u, work->bursts[i].v + 127,
                        work->bursts[i].u + 31, work->bursts[i].v + 127);
                    r = work->bursts[i].color[0];
                    g = work->bursts[i].color[1];
                    b = work->bursts[i].color[2];
                    setRGB0(&work->sprite, r, g, b);
                    depth = RotAverage4(&work->rays[k][0], &work->rays[k + 1][0],
                        &work->rays[k][1], &work->rays[k + 1][1],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                    setPolyG3(&work->tri);
                    r = work->bursts[i].color[0];
                    g = work->bursts[i].color[1];
                    b = work->bursts[i].color[2];
                    setRGB0(&work->tri, r, g, b);
                    setRGB1(&work->tri, 0, 0, 0);
                    setRGB2(&work->tri, 0, 0, 0);
                    depth = RotAverage3(&p0, &work->rays[k][0], &work->rays[k + 1][0],
                        (PSXLONG *)&work->tri.x0, (PSXLONG *)&work->tri.x1,
                        (PSXLONG *)&work->tri.x2, &interpolation, &flag);
                    if (flag >= 0) {
                        Graphics_SubmitDrawModePacket((u32 *)&work->tri, func_80058F10(), (u16)depth,
                            work->bursts[i].mode);
                    }
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[1];
                    work->strip.clut = work->clut[1];
                    setSemiTrans(&work->strip, 1);
                    if (k % 2 == 0) {
                        setUV4(&work->strip, 0, 0, 63, 0, 0, 127, 63, 127);
                    } else {
                        setUV4(&work->strip, 63, 0, 0, 0, 63, 127, 0, 127);
                    }
                    r = work->bursts[i].color[0];
                    g = work->bursts[i].color[1];
                    b = work->bursts[i].color[2];
                    setRGB0(&work->strip, r, g, b);
                    setRGB1(&work->strip, r, g, b);
                    setRGB2(&work->strip, 0, 0, 0);
                    setRGB3(&work->strip, 0, 0, 0);
                    PushMatrix();
                    m = D_800FE168;
                    /* Alternate strips spin the other way, faster outward. */
                    angle = ((k % 2) * 2 - 1) * 55;
                    RotMatrixZ(work->bursts[i].elapsed * (k * 5 + 1) * angle, &m);
                    pts[0][0] = work->fan[k][0];
                    pts[1][0] = work->fan[k + 1][0];
                    pts[2][0] = work->fan[k][1];
                    pts[3][0] = work->fan[k + 1][1];
                    for (j = 0; j < 4; j++) {
                        ApplyMatrixSV(&m, &pts[j][0], &pts[j][1]);
                    }
                    PopMatrix();
                    depth = RotAverage4(&pts[0][1], &pts[1][1], &pts[2][1], &pts[3][1],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
                work->bursts[i].elapsed += step;
                if (work->bursts[i].duration < work->bursts[i].elapsed) {
                    work->bursts[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (k = 0; k < work->spark_columns; k++) {
            for (i = 0; i < work->spark_rows; i++) {
                if (work->sparks[i][k].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    value = (work->sparks[i][k].elapsed << 12) / work->sparks[i][k].duration + 0x1000;
                    setVector(&scale, value, value, value);
                    ScaleMatrix(&unit.matrix, &scale);
                    RotMatrixZ(0, &unit.matrix);
                    RotMatrixX(-((view.pitch - 4096) * work->direction), &unit.matrix);
                    RotMatrixY(work->slot * 2048 + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][k].position.vx;
                    unit.matrix.t[1] = work->sparks[i][k].position.vy;
                    unit.matrix.t[2] = work->sparks[i][k].position.vz;
                    unit.super = work->sparks[i][k].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    value = work->sparks[i][k].duration - work->sparks[i][k].elapsed;
                    j = work->sparks[i][k].duration;
                    work->sparks[i][k].color[0] = D_8013D104[work->command].color[3] * value / j;
                    work->sparks[i][k].color[1] = D_8013D104[work->command].color[4] * value / j;
                    work->sparks[i][k].color[2] = D_8013D104[work->command].color[5] * value / j;
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage[2];
                    work->sprite.clut = work->clut[2];
                    setSemiTrans(&work->sprite, 1);
                    setUV4(&work->sprite, work->sparks[i][k].u, 0x80,
                        work->sparks[i][k].u + 31, 0x80,
                        work->sparks[i][k].u, 0x9F,
                        work->sparks[i][k].u + 31, 0x9F);
                    setRGB0(&work->sprite, work->sparks[i][k].color[0], work->sparks[i][k].color[1],
                        work->sparks[i][k].color[2]);
                    depth = RotAverage4(&work->quad[0], &work->quad[1], &work->quad[2], &work->quad[3],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                    work->sparks[i][k].u += 32;
                    if (work->sparks[i][k].u > 224) {
                        work->sparks[i][k].u = 0;
                    }
                    work->sparks[i][k].elapsed += step;
                    if (work->sparks[i][k].duration < work->sparks[i][k].elapsed) {
                        work->sparks[i][k].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->column_count + work->burst_count + work->spark_columns * work->spark_rows) {
            result = 2;
        }
    }
    return result;
}
