#include "../../types.h"
#include "variant654_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

/* Header 654's entry: bursts of textured quads fly out of the slot one column
 * at a time, or start together in front of it, and each column's first burst
 * opens a ring of quads and a dotted sphere where it ends. The setup also
 * prints the work-area size. */

/* The quad corners' negations go through an int parameter, so the front end
 * does not narrow them to halfword arithmetic: that is retail's three loads
 * of the radius and the zero-extension before the third negation. */
static inline s32 Model654_Negate(s32 value)
{
    return -value;
}

s32 func_8013B004(u8 *context, s32 command)
{
    Model654State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    Model654ViewSnapshot view;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, n;
    u32 tex;
    PSXLONG depth;
    s32 d;

    work = (Model654State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model654State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model654State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D404);
        work->tpage = tex >> 16;
        work->clut = tex;
        switch (command) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 9:
            work->rows = 3;
            work->columns = 8;
            work->ring_count = 8;
            break;
        case 5:
            work->rows = 3;
            work->columns = 4;
            work->ring_count = 8;
            break;
        case 6:
        case 7:
        case 8:
        case 11:
            work->rows = 0;
            work->columns = 8;
            work->ring_count = 8;
            break;
        case 10:
        case 12:
        case 13:
            work->rows = 0;
            work->columns = 3;
            work->ring_count = 8;
            break;
        case 14:
            work->rows = 3;
            work->columns = 12;
            work->ring_count = 5;
            break;
        }
        work->radius = D_8013D420[command].radius;
        work->unknown_10 = D_8013D420[command].unknown_04;
        for (j = 0; j < work->columns; j++) {
            for (i = 0; i < work->rows; i++) {
                work->bursts[i][j].duration = 0x80;
                work->bursts[i][j].active = 0;
                work->bursts[i][j].elapsed = 0;
                work->bursts[i][j].mode = 1;
                work->bursts[i][j].coord = Model_GetCameraCoordinateUnit();
                work->bursts[i][j].red = D_8013D420[command].red;
                work->bursts[i][j].green = D_8013D420[command].green;
                work->bursts[i][j].blue = D_8013D420[command].blue;
                work->bursts[i][j].v = 0;
                work->bursts[i][j].u = (i + j) % 7 << 5;
                setVector(&work->bursts[i][j].scale, 0, 0, 0);
                setVector(&work->bursts[i][j].growth,
                    (work->radius >> 1) * rsin((i << 12) / work->rows + (j << 12) / work->columns) / 4096,
                    (work->radius >> 1) * rcos((i << 12) / work->rows + (j << 12) / work->columns) / 4096,
                    work->direction * 450);
                setVector(&work->bursts[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->bursts[i][j].position, 0, 0, 0);
            }
        }
        for (j = 0; j < work->columns; j++) {
            for (i = 0; i < work->ring_count; i++) {
                work->rings[i][j].duration = 0x40;
                work->rings[i][j].active = 0;
                work->rings[i][j].elapsed = 0;
                work->rings[i][j].mode = 1;
                work->rings[i][j].coord = Model_GetCameraCoordinateUnit();
                work->rings[i][j].red = D_8013D420[command].red;
                work->rings[i][j].green = D_8013D420[command].green;
                work->rings[i][j].blue = D_8013D420[command].blue;
                work->rings[i][j].v = 0;
                work->rings[i][j].u = (i + j) % 7 << 5;
                setVector(&work->rings[i][j].scale, 0x1000, 0x1000, 0x1000);
                setVector(&work->rings[i][j].growth,
                    (work->radius * 5) * rsin((i << 12) / work->ring_count + (j << 12) / work->columns) / 4096,
                    (work->radius * 5) * rcos((i << 12) / work->ring_count + (j << 12) / work->columns) / 4096,
                    work->direction * work->radius * 5);
                setVector(&work->rings[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->rings[i][j].position, 0, 0, 0);
            }
        }
        for (i = 0; i < work->columns; i++) {
            work->spheres[i].duration = 0x40;
            work->spheres[i].active = 0;
            work->spheres[i].elapsed = 0;
            work->spheres[i].mode = 1;
            work->spheres[i].coord = Model_GetCameraCoordinateUnit();
            work->spheres[i].red = D_8013D420[command].sphere_red;
            work->spheres[i].green = D_8013D420[command].sphere_green;
            work->spheres[i].blue = D_8013D420[command].sphere_blue;
            setVector(&work->spheres[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->spheres[i].growth, 0, 0, 0);
            setVector(&work->spheres[i].rotation, (i << 12) / work->columns,
                (i << 12) / work->columns + (work->slot * 2048), (i << 12) / work->columns);
            setVector(&work->spheres[i].position, 0, 0, 0);
        }
        work->delay = D_8013D420[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        n = 5;
        setVector(&work->square[0], Model654_Negate(work->radius), Model654_Negate(work->radius), 0);
        setVector(&work->square[1], work->radius, Model654_Negate(work->radius), 0);
        setVector(&work->square[2], Model654_Negate(work->radius), work->radius, 0);
        setVector(&work->square[3], work->radius, work->radius, 0);
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 9; j++) {
                setVector(&work->grid[j][i],
                    SquareRoot0(n * n * work->radius * work->radius
                        - n * work->radius * rcos(i << 8) / 4096 * (n * work->radius * rcos(i << 8) / 4096))
                        * rsin(j << 9) / 4096,
                    n * work->radius * rcos(i << 8) / 4096,
                    SquareRoot0(n * n * work->radius * work->radius
                        - n * work->radius * rcos(i << 8) / 4096 * (n * work->radius * rcos(i << 8) / 4096))
                        * rcos(j << 9) / 4096);
            }
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->flags[0] < work->columns && work->elapsed >= work->flags[0] * 4) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 8);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->rows; i++) {
                    work->bursts[i][work->flags[0]].active = 1;
                    applyVector(&work->bursts[i][work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2], +=);
                }
                work->flags[0]++;
            }
            break;
        case 2:
        case 3:
        case 7:
        case 8:
            if (work->elapsed == 0) {
                for (i = 0; i < work->ring_count; i++) {
                    work->rings[i][0].active = 1;
                    applyVector(&work->rings[i][0].position, 0, -350, work->direction * 450, +=);
                }
                for (i = 0; i < work->columns; i++) {
                    work->spheres[i].active = 1;
                    applyVector(&work->spheres[i].position, 0, -350, work->direction * 450, +=);
                }
            }
            break;
        case 1:
        case 4:
        case 5:
        case 10:
        case 12:
        case 13:
            if (work->flags[0] < work->columns && work->elapsed >= work->flags[0] * 10) {
                for (i = 0; i < work->ring_count; i++) {
                    work->rings[i][work->flags[0]].active = 1;
                    applyVector(&work->rings[i][work->flags[0]].position, 0, -350, work->direction * 450, +=);
                }
                work->spheres[work->flags[0]].active = 1;
                applyVector(&work->spheres[work->flags[0]].position, 0, -350, work->direction * 450, +=);
                work->flags[0]++;
            }
            break;
        case 6:
            if (work->elapsed == 0) {
                for (i = 0; i < work->ring_count; i++) {
                    work->rings[i][0].active = 1;
                    applyVector(&work->rings[i][0].position, 0, -150, work->direction * 450, +=);
                }
                for (i = 0; i < work->columns; i++) {
                    work->spheres[i].active = 1;
                    applyVector(&work->spheres[i].position, 0, -150, work->direction * 450, +=);
                }
            }
            break;
        case 9:
            if (work->flags[0] < work->columns && work->elapsed >= work->flags[0] * 4) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 8);
                GsGetLwUnit(&unit, &ls);
                ls.t[2] += work->direction * 50;
                for (i = 0; i < work->rows; i++) {
                    work->bursts[i][work->flags[0]].active = 1;
                    applyVector(&work->bursts[i][work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2], +=);
                }
                work->flags[0]++;
            }
            break;
        case 11:
            if (work->elapsed == 0) {
                for (i = 0; i < work->ring_count; i++) {
                    work->rings[i][0].active = 1;
                    applyVector(&work->rings[i][0].position, 0, -250, work->direction * 450, +=);
                }
                for (i = 0; i < work->columns; i++) {
                    work->spheres[i].active = 1;
                    applyVector(&work->spheres[i].position, 0, -250, work->direction * 450, +=);
                }
            }
            break;
        case 14:
            if (work->flags[0] < work->columns && work->elapsed >= work->flags[0] * 4) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 6);
                GsGetLwUnit(&unit, &ls);
                for (i = 0; i < work->rows; i++) {
                    work->bursts[i][work->flags[0]].active = 1;
                    applyVector(&work->bursts[i][work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2], +=);
                }
                work->flags[0]++;
            }
            break;
        }
        PushMatrix();
        view = *(Model654ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (j = 0; j < work->columns; j++) {
            for (i = 0; i < work->rows; i++) {
                if (work->bursts[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    n = (work->bursts[i][j].elapsed << 12) / work->bursts[i][j].duration;
                    setVector(&work->bursts[i][j].scale, n, n, n);
                    ScaleMatrix(&unit.matrix, &work->bursts[i][j].scale);
                    RotMatrixZ(work->bursts[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->bursts[i][j].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                        &unit.matrix);
                    RotMatrixY(work->bursts[i][j].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                    unit.matrix.t[0] = work->bursts[i][j].position.vx
                        + work->bursts[i][j].growth.vx * work->bursts[i][j].elapsed / work->bursts[i][j].duration;
                    unit.matrix.t[1] = work->bursts[i][j].position.vy
                        + work->bursts[i][j].growth.vy * work->bursts[i][j].elapsed / work->bursts[i][j].duration;
                    unit.matrix.t[2] = work->bursts[i][j].position.vz
                        + (work->bursts[i][j].growth.vz - work->bursts[i][j].position.vz) * work->bursts[i][j].elapsed
                        / work->bursts[i][j].duration;
                    unit.super = work->bursts[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage;
                    work->flat.clut = work->clut;
                    setSemiTrans(&work->flat, 1);
                    setUV4(&work->flat, work->bursts[i][j].u, work->bursts[i][j].v, work->bursts[i][j].u + 31,
                        work->bursts[i][j].v, work->bursts[i][j].u, work->bursts[i][j].v + 31,
                        work->bursts[i][j].u + 31, work->bursts[i][j].v + 31);
                    setRGB0(&work->flat, work->bursts[i][j].red, work->bursts[i][j].green, work->bursts[i][j].blue);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                    work->bursts[i][j].u += 32;
                    if (work->bursts[i][j].u >= 0xE0) {
                        work->bursts[i][j].u = 0;
                    }
                    work->bursts[i][j].elapsed += step;
                    if (work->bursts[i][j].duration < work->bursts[i][j].elapsed) {
                        work->bursts[i][j].active = 0;
                        if (i == 0) {
                            for (k = 0; k < work->ring_count; k++) {
                                work->rings[k][j].active = 1;
                                setVector(&work->rings[k][j].position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                            }
                            work->spheres[j].active = 1;
                            setVector(&work->spheres[j].position, unit.matrix.t[0], unit.matrix.t[1], unit.matrix.t[2]);
                        }
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->columns; j++) {
            for (i = 0; i < work->ring_count; i++) {
                if (work->rings[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    n = (work->rings[i][j].elapsed << 12) / work->rings[i][j].duration + 0x1000;
                    setVector(&work->rings[i][j].scale, n, n, n);
                    ScaleMatrix(&unit.matrix, &work->rings[i][j].scale);
                    RotMatrixZ(work->rings[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->rings[i][j].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * work->direction,
                        &unit.matrix);
                    RotMatrixY(work->rings[i][j].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                    unit.matrix.t[0] = work->rings[i][j].position.vx
                        + work->rings[i][j].growth.vx * work->rings[i][j].elapsed / work->rings[i][j].duration;
                    unit.matrix.t[1] = work->rings[i][j].position.vy
                        + work->rings[i][j].growth.vy * work->rings[i][j].elapsed / work->rings[i][j].duration;
                    unit.matrix.t[2] = work->rings[i][j].position.vz
                        + work->rings[i][j].growth.vz * work->rings[i][j].elapsed / work->rings[i][j].duration;
                    unit.super = work->rings[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->rings[i][j].duration / 2 < work->rings[i][j].elapsed) {
                        n = (work->rings[i][j].duration - work->rings[i][j].elapsed) * 2;
                        d = work->rings[i][j].duration;
                        work->rings[i][j].red = D_8013D420[work->command].red * n / d;
                        work->rings[i][j].green = D_8013D420[work->command].green * n / d;
                        work->rings[i][j].blue = D_8013D420[work->command].blue * n / d;
                    }
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage;
                    work->flat.clut = work->clut;
                    setSemiTrans(&work->flat, 1);
                    setUV4(&work->flat, work->rings[i][j].u, work->rings[i][j].v, work->rings[i][j].u + 31,
                        work->rings[i][j].v, work->rings[i][j].u, work->rings[i][j].v + 31,
                        work->rings[i][j].u + 31, work->rings[i][j].v + 31);
                    setRGB0(&work->flat, work->rings[i][j].red, work->rings[i][j].green, work->rings[i][j].blue);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                    work->rings[i][j].u += 32;
                    if (work->rings[i][j].u >= 0xE0) {
                        work->rings[i][j].u = 0;
                    }
                    result = 4;
                    work->rings[i][j].elapsed += step;
                    if (work->rings[i][j].duration < work->rings[i][j].elapsed) {
                        work->rings[i][j].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->columns; i++) {
            if (work->spheres[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = (work->spheres[i].elapsed << 12) / work->spheres[i].duration;
                setVector(&work->spheres[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->spheres[i].scale);
                RotMatrixZ(work->spheres[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->spheres[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->spheres[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->spheres[i].position.vx;
                unit.matrix.t[1] = work->spheres[i].position.vy;
                unit.matrix.t[2] = work->spheres[i].position.vz;
                unit.super = work->spheres[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->spheres[i].duration / 2 < work->spheres[i].elapsed) {
                    n = (work->spheres[i].duration - work->spheres[i].elapsed) * 2;
                    d = work->spheres[i].duration;
                    work->spheres[i].red = D_8013D420[work->command].sphere_red * n / d;
                    work->spheres[i].green = D_8013D420[work->command].sphere_green * n / d;
                    work->spheres[i].blue = D_8013D420[work->command].sphere_blue * n / d;
                }
                for (j = 1; j < 8; j++) {
                    for (k = 0; k < 8; k++) {
                        work->line.attribute = 0x50000000;
                        work->line.r0 = work->spheres[i].red;
                        work->line.g0 = work->spheres[i].green;
                        work->line.b0 = work->spheres[i].blue;
                        work->line.r1 = work->spheres[i].red;
                        work->line.g1 = work->spheres[i].green;
                        work->line.b1 = work->spheres[i].blue;
                        /* Both ends are the same point, as in retail: a dotted sphere. */
                        RotTransPers(&work->grid[k][j], (PSXLONG *)&work->line.x0, &interpolation, &flag);
                        depth = RotTransPers(&work->grid[k][j], (PSXLONG *)&work->line.x1, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortGLine(&work->line, func_80058F10(), (u16)depth);
                        }
                    }
                }
                work->spheres[i].elapsed += step;
                if (work->spheres[i].duration < work->spheres[i].elapsed) {
                    work->spheres[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == (work->rows + work->ring_count) * work->columns + work->columns) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}
