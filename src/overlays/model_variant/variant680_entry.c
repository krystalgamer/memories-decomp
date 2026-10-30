#include "../../types.h"
#include "variant680_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

/* Header 680's entry: command 0 spawns up to 32 shaded twin-layer cones at
 * model points in a fixed order; each cone grows, fades and hands over to a
 * flipbook spark, while a flash pulses outwards from the same point in five
 * directions. Columns of glows also flicker at a row of model points. The
 * setup also prints the work-area size. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model680State *work;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    Model680ViewSnapshot view;
    /* Arrays, so the colour bytes live in the frame like the original's. */
    u8 r[2], g[2], b[2], v[2];
    VECTOR offset;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 n, d, t, p;
    /* Single-use temporaries keep the original's register choices. */
    s32 tilt, reach, swing;
    s32 count;
    u32 tex;
    PSXLONG depth;

    result = 0;
    work = (Model680State *)context;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model680State));
#ifndef VERSION_FRENCH
        printf(D_8013DAAC, sizeof(Model680State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        if (command == 0) {
            work->order[1] = 1;
            work->order[2] = 2;
            work->order[0] = 0;
            work->order[3] = 3;
        }
        for (i = 0; i < 8; i++) {
            work->cell_u[i] = i % 4 * 64;
            work->cell_v[i] = i / 4 * 64 - 128;
        }
        /* Scheduling barrier: the empty loop after the early count
         * assignment keeps the D_8013DAC4 high half ahead of the first
         * call's constant argument.  The assignment itself is sunk into
         * the command check below before register allocation. */
        count = 32;
        do {
        } while (0);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAC4[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAC4[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAC4[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAC4[3]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAC4[4]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAC4[5]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAC4[6]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAC4[7]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAC4[8]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAC4[9]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013DAC4[10]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        if (command == 0) {
            work->count = count;
            work->rows = 6;
            work->columns = 8;
        }
        work->radius = D_8013DBF8[command].radius;
        work->height = D_8013DBF8[command].height;
        for (i = 0; i < work->count; i++) {
            work->cones[i].duration = 16;
            work->cones[i].active = 0;
            work->cones[i].elapsed = 0;
            work->cones[i].mode = 1;
            work->cones[i].coord = Model_GetCameraCoordinateUnit();
            work->cones[i].red = D_8013DBF8[command].red;
            work->cones[i].green = D_8013DBF8[command].green;
            work->cones[i].blue = D_8013DBF8[command].blue;
            work->cones[i].u = 0;
            work->cones[i].v = 0;
            setVector(&work->cones[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->cones[i].growth, 0, 0, 0);
            tilt = work->direction * 6;
            setVector(&work->cones[i].rotation, i * tilt, 0x2AA, 0);
            setVector(&work->cones[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->flashes[i].duration = 32;
            work->flashes[i].active = 0;
            work->flashes[i].elapsed = 0;
            work->flashes[i].mode = 1;
            work->flashes[i].coord = Model_GetCameraCoordinateUnit();
            work->flashes[i].red = D_8013DBF8[command].flash_red;
            work->flashes[i].green = D_8013DBF8[command].flash_green;
            work->flashes[i].blue = D_8013DBF8[command].flash_blue;
            work->flashes[i].u = 0;
            work->flashes[i].v = 64;
            setVector(&work->flashes[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->flashes[i].growth, 0, 0, 0);
            setVector(&work->flashes[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->flashes[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->sparks[i].active = 0;
            work->sparks[i].elapsed = 0;
            work->sparks[i].duration = 32;
            work->sparks[i].mode = 1;
            work->sparks[i].coord = Model_GetCameraCoordinateUnit();
            work->sparks[i].red = D_8013DBF8[command].flash_red;
            work->sparks[i].green = D_8013DBF8[command].flash_green;
            work->sparks[i].blue = D_8013DBF8[command].flash_blue;
            work->sparks[i].u = 0;
            work->sparks[i].v = 32;
            setVector(&work->sparks[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->sparks[i].growth, 0, 0, 0);
            setVector(&work->sparks[i].rotation, 0, work->slot * 2048, 0);
            swing = rsin(i * 500) * 20;
            n = work->radius * swing / 4096;
            d = i % 2 * 2 - 1;
            p = i % 16 << 8;
            reach = d * n;
            setVector(&work->sparks[i].position, reach * rsin(p) / 4096, reach * rcos(p) / 4096, n * rsin(p) / 4096);
            work->sparks[i].position.vy -= 350;
            work->sparks[i].position.vz += work->direction * 450;
        }
        for (j = 0; j < work->rows; j++) {
            for (i = 0; i < work->columns; i++) {
                work->glows[i][j].active = 0;
                work->glows[i][j].elapsed = 0;
                work->glows[i][j].duration = 16;
                work->glows[i][j].coord = Model_GetCameraCoordinateUnit();
                work->glows[i][j].red = D_8013DBF8[command].red;
                work->glows[i][j].green = D_8013DBF8[command].green;
                work->glows[i][j].blue = D_8013DBF8[command].blue;
                work->glows[i][j].u = 0;
                setVector(&work->glows[i][j].position, 0, 0, 0);
            }
        }
        setVector(&offset, work->radius * 6, 0, work->radius * -(work->direction * 2));
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < 5; i++) {
                PushMatrix();
                m = D_800FE168;
                RotMatrixZ((i << 12) / 5, &m);
                /* Without the assignment the product becomes an induction variable. */
                RotMatrixX(j * (tilt = work->direction * 6), &m);
                RotMatrixY(0x2AA, &m);
                ApplyMatrixLV(&m, &offset, &work->ring[i][j]);
                PopMatrix();
            }
        }
        setVector(&offset, 0, work->radius * 20, 0);
        for (i = 0; i < 5; i++) {
            PushMatrix();
            m = D_800FE168;
            RotMatrixZ((i << 11) / 5, &m);
            RotMatrixY((i % 3 << 12) / 3, &m);
            ApplyMatrixLV(&m, &offset, &work->burst[i]);
            PopMatrix();
        }
        d = work->height * work->direction;
        for (i = 0; i < 5; i++) {
            n = work->radius * rcos(i * 819 / 4) / 4096;
            for (j = 0; j < 5; j++) {
                setVector(&work->grid[j][i][0], n * rsin(j << 10) / 4096, n * rcos(j << 10) / 4096, i * d / 4);
                work->grid[j][i][1] = work->grid[j][i][0];
                work->grid[j][i][0].vy += 30;
                work->grid[j][i][1].vy -= 30;
            }
        }
        n = work->radius * 2;
        setVector(&work->square[0], -n, -n, 0);
        setVector(&work->square[1], n, -n, 0);
        setVector(&work->square[2], -n, n, 0);
        setVector(&work->square[3], n, n, 0);
        n = work->radius * 2;
        d = work->radius * 4;
        for (i = 0; i < 6; i++) {
            t = (i << 12) / 5;
            setVector(&work->band[0][i][0], n * rsin(t) / 4096, n * rcos(t) / 4096, 0);
            setVector(&work->band[1][i][0], d * rsin(t) / 4096, d * rcos(t) / 4096,
                work->height * 2 / 3 * work->direction);
            for (j = 0; j < 2; j++) {
                work->band[j][i][1] = work->band[j][i][0];
                work->band[j][i][0].vy += 30;
                work->band[j][i][1].vy -= 30;
            }
        }
        n = work->radius * 7;
        setVector(&work->flash[0], -n, -n, 0);
        setVector(&work->flash[1], n, -n, 0);
        setVector(&work->flash[2], -n, n, 0);
        setVector(&work->flash[3], n, n, 0);
        n = work->radius * 4;
        setVector(&work->spark[0], -n, -n, 0);
        setVector(&work->spark[1], n, -n, 0);
        setVector(&work->spark[2], -n, n, 0);
        setVector(&work->spark[3], n, n, 0);
        n = work->radius * 2;
        setVector(&work->glow[0], -n, -n, 0);
        setVector(&work->glow[1], n, -n, 0);
        setVector(&work->glow[2], -n, n, 0);
        setVector(&work->glow[3], n, n, 0);
        work->delay = D_8013DBF8[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        if (work->command == 0) {
            if (work->elapsed >= 98 && work->elapsed < 204) {
                if (work->elapsed >= work->flags[0] * 3 + 98 && work->flags[0] < work->count) {
                    work->cones[work->flags[0]].active = 1;
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot,
                        (work->order[work->flags[0] & 3] >> 1) + 0x1D);
                    GsGetLwUnit(&unit, &ls);
                    setVector(&work->cones[work->flags[0]].position, ls.t[0], ls.t[1], ls.t[2]);
                    work->flashes[work->flags[0]].active = 1;
                    work->flashes[work->flags[0]].position = work->cones[work->flags[0]].position;
                    work->flags[0]++;
                }
            }
            if (work->elapsed < 78 || (work->elapsed >= 257 && work->elapsed < 358)) {
                for (i = 0; i < work->rows; i++) {
                    work->glows[work->flags[1]][i].active = 1;
                    work->glows[work->flags[1]][i].elapsed = 0;
                    work->glows[work->flags[1]][i].u = (i * 5 + work->flags[1] * 3) % 8 << 5;
                    work->glows[work->flags[1]][i].red = D_8013DBF8[work->command].red;
                    work->glows[work->flags[1]][i].green = D_8013DBF8[work->command].green;
                    work->glows[work->flags[1]][i].blue = D_8013DBF8[work->command].blue;
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, i + 0x22);
                    GsGetLwUnit(&unit, &ls);
                    setVector(&work->glows[work->flags[1]][i].position, ls.t[0], ls.t[1], ls.t[2]);
                }
                work->flags[1]++;
                if (work->flags[1] >= work->columns) {
                    work->flags[1] = 0;
                }
            }
            if (work->elapsed >= 301) {
                work->flags[2] = 1;
            }
            if (work->elapsed >= 223 && work->flags[4] == 0) {
                result = 1;
                work->flags[4] = result;
            }
        }
        PushMatrix();
        view = *(Model680ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->cones[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = (work->cones[i].elapsed << 12) / work->cones[i].duration;
                setVector(&work->cones[i].scale, 0x1000, 0x1000, n);
                ScaleMatrix(&unit.matrix, &work->cones[i].scale);
                RotMatrixX(work->cones[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->cones[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->cones[i].position.vx;
                unit.matrix.t[1] = work->cones[i].position.vy;
                unit.matrix.t[2] = work->cones[i].position.vz;
                unit.super = work->cones[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->cones[i].duration / 2 < work->cones[i].elapsed) {
                    n = (work->cones[i].duration - work->cones[i].elapsed) * 2;
                    d = work->cones[i].duration;
                    work->cones[i].red = D_8013DBF8[work->command].red * n / d;
                    work->cones[i].green = D_8013DBF8[work->command].green * n / d;
                    work->cones[i].blue = D_8013DBF8[work->command].blue * n / d;
                }
                for (j = 0; j < 4; j++) {
                    if (j == 0) {
                        r[0] = 0;
                        g[0] = 0;
                        b[0] = 0;
                    } else {
                        r[0] = work->cones[i].red * (4 - j) / 4;
                        g[0] = work->cones[i].green * (4 - j) / 4;
                        b[0] = work->cones[i].blue * (4 - j) / 4;
                    }
                    r[1] = work->cones[i].red * (3 - j) / 4;
                    g[1] = work->cones[i].green * (3 - j) / 4;
                    b[1] = work->cones[i].blue * (3 - j) / 4;
                    v[0] = work->cones[i].v + (4 - j) * 12;
                    v[1] = work->cones[i].v + (3 - j) * 12;
                    for (k = 0; k < 4; k++) {
                        setPolyGT4(&work->shaded);
                        work->shaded.tpage = work->tpage[0];
                        work->shaded.clut = work->clut[0];
                        setSemiTrans(&work->shaded, 1);
                        setUV4(&work->shaded, work->cones[i].u, v[0], work->cones[i].u + 31, v[0],
                            work->cones[i].u, v[1], work->cones[i].u + 31, v[1]);
                        setRGB0(&work->shaded, r[0], g[0], b[0]);
                        setRGB1(&work->shaded, r[0], g[0], b[0]);
                        setRGB2(&work->shaded, r[1], g[1], b[1]);
                        setRGB3(&work->shaded, r[1], g[1], b[1]);
                        n = i % 2;
                        depth = RotAverage4(&work->grid[k][j][n], &work->grid[k + 1][j][n],
                            &work->grid[k][j + 1][n], &work->grid[k + 1][j + 1][n],
                            (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                            (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->shaded, func_80058F10(), (u16)depth);
                        }
                    }
                }
                for (j = 0; j < 5; j++) {
                    setPolyGT4(&work->shaded);
                    setUV4(&work->shaded, 32, 0, 95, 0, 32, 16, 95, 16);
                    work->shaded.tpage = work->tpage[2];
                    work->shaded.clut = work->clut[2];
                    setSemiTrans(&work->shaded, 1);
                    setRGB0(&work->shaded, work->cones[i].red, work->cones[i].green, work->cones[i].blue);
                    setRGB1(&work->shaded, work->cones[i].red, work->cones[i].green, work->cones[i].blue);
                    setRGB2(&work->shaded, 0, 0, 0);
                    setRGB3(&work->shaded, 0, 0, 0);
                    n = i % 2;
                    depth = RotAverage4(&work->band[0][j][n], &work->band[0][j + 1][n],
                        &work->band[1][j][n], &work->band[1][j + 1][n],
                        (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                        (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->shaded, func_80058F10(), (u16)depth);
                    }
                }
                work->cones[i].elapsed += step;
                if (work->cones[i].duration < work->cones[i].elapsed) {
                    work->cones[i].active = 0;
                    work->sparks[i].active = 1;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->flashes[i].active == 1) {
                n = (work->flashes[i].elapsed << 12) / work->flashes[i].duration + 0x1000;
                setVector(&work->flashes[i].scale, n, n, n);
                if (work->flashes[i].duration / 2 < work->flashes[i].elapsed) {
                    n = (work->flashes[i].duration - work->flashes[i].elapsed) * 2;
                    d = work->flashes[i].duration;
                    work->flashes[i].red = D_8013DBF8[work->command].flash_red * n / d;
                    work->flashes[i].green = D_8013DBF8[work->command].flash_green * n / d;
                    work->flashes[i].blue = D_8013DBF8[work->command].flash_blue * n / d;
                }
                for (k = 0; k < 5; k++) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->flashes[i].scale);
                    RotMatrixZ(work->flashes[i].rotation.vz, &unit.matrix);
                    RotMatrixX(work->flashes[i].rotation.vx - (view.pitch - 0x1000) * work->direction,
                        &unit.matrix);
                    RotMatrixY(work->flashes[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->flashes[i].position.vx
                        + work->ring[k][i].vx * work->flashes[i].elapsed / work->flashes[i].duration;
                    n = i % 2 * 2 - 1;
                    unit.matrix.t[1] = work->flashes[i].position.vy - n * 30
                        + work->ring[k][i].vy * work->flashes[i].elapsed / work->flashes[i].duration;
                    unit.matrix.t[2] = work->flashes[i].position.vz
                        + work->ring[k][i].vz * work->flashes[i].elapsed / work->flashes[i].duration;
                    unit.super = work->flashes[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage[1];
                    work->flat.clut = work->clut[1];
                    setSemiTrans(&work->flat, 1);
                    setUV4(&work->flat, work->flashes[i].u, work->flashes[i].v, work->flashes[i].u + 31,
                        work->flashes[i].v, work->flashes[i].u, work->flashes[i].v + 31,
                        work->flashes[i].u + 31, work->flashes[i].v + 31);
                    setRGB0(&work->flat, work->flashes[i].red, work->flashes[i].green, work->flashes[i].blue);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                }
                if (work->flashes[i].u < 0xE0) {
                    work->flashes[i].u += 32;
                }
                work->flashes[i].elapsed += step;
                if (work->flashes[i].duration < work->flashes[i].elapsed) {
                    work->flashes[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->sparks[i].active == 1) {
                n = (work->sparks[i].elapsed << 13) / work->sparks[i].duration + 0x1000;
                setVector(&work->sparks[i].scale, n, n, n);
                if (work->sparks[i].duration / 2 < work->sparks[i].elapsed) {
                    n = (work->sparks[i].duration - work->sparks[i].elapsed) * 2;
                    d = work->sparks[i].duration;
                    work->sparks[i].red = D_8013DBF8[work->command].flash_red * n / d;
                    work->sparks[i].green = D_8013DBF8[work->command].flash_green * n / d;
                    work->sparks[i].blue = D_8013DBF8[work->command].flash_blue * n / d;
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->sparks[i].scale);
                RotMatrixZ(work->sparks[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->sparks[i].rotation.vx - (view.pitch - 0x1000) * work->direction,
                    &unit.matrix);
                RotMatrixY(work->sparks[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->sparks[i].position.vx;
                unit.matrix.t[1] = work->sparks[i].position.vy;
                unit.matrix.t[2] = work->sparks[i].position.vz;
                unit.super = work->sparks[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->flat);
                work->flat.tpage = work->tpage[3];
                work->flat.clut = work->clut[3];
                setSemiTrans(&work->flat, 1);
                setUV4(&work->flat, work->cell_u[work->sparks[i].u], work->cell_v[work->sparks[i].u],
                    work->cell_u[work->sparks[i].u] + 63, work->cell_v[work->sparks[i].u],
                    work->cell_u[work->sparks[i].u], work->cell_v[work->sparks[i].u] + 63,
                    work->cell_u[work->sparks[i].u] + 63, work->cell_v[work->sparks[i].u] + 63);
                setRGB0(&work->flat, work->sparks[i].red, work->sparks[i].green, work->sparks[i].blue);
                depth = RotAverage4(&work->flash[0], &work->flash[1], &work->flash[2], &work->flash[3],
                    (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                    (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                }
                for (j = 0; j < 5; j++) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->sparks[i].scale);
                    RotMatrixZ(work->sparks[i].rotation.vz, &unit.matrix);
                    RotMatrixX(work->sparks[i].rotation.vx - (view.pitch - 0x1000) * work->direction,
                        &unit.matrix);
                    RotMatrixY(work->sparks[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i].position.vx
                        + work->burst[j].vx * work->sparks[i].elapsed / work->sparks[i].duration;
                    unit.matrix.t[1] = work->sparks[i].position.vy
                        + work->burst[j].vy * work->sparks[i].elapsed / work->sparks[i].duration;
                    unit.matrix.t[2] = work->sparks[i].position.vz
                        + work->burst[j].vz * work->sparks[i].elapsed / work->sparks[i].duration;
                    unit.super = work->sparks[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage[1];
                    work->flat.clut = work->clut[1];
                    setSemiTrans(&work->flat, 1);
                    setUV4(&work->flat, work->sparks[i].v, 64, work->sparks[i].v + 31, 64,
                        work->sparks[i].v, 95, work->sparks[i].v + 31, 95);
                    setRGB0(&work->flat, work->sparks[i].red, work->sparks[i].green, work->sparks[i].blue);
                    depth = RotAverage4(&work->spark[0], &work->spark[1], &work->spark[2], &work->spark[3],
                        (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                }
                if (work->sparks[i].u < 7) {
                    work->sparks[i].u++;
                }
                if (work->sparks[i].v < 0xE0) {
                    work->sparks[i].v += 32;
                }
                work->sparks[i].elapsed += step;
                if (work->sparks[i].duration < work->sparks[i].elapsed) {
                    work->sparks[i].active = 0;
                }
                if (work->flags[4] == 0) {
                    result = 4;
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->rows; j++) {
            for (i = 0; i < work->columns; i++) {
                if (work->glows[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    n = (work->glows[i][j].elapsed << 12) / work->glows[i][j].duration + 0x1000;
                    setVector(&offset, n, n, n);
                    ScaleMatrix(&unit.matrix, &offset);
                    RotMatrixZ(0, &unit.matrix);
                    RotMatrixX(-((view.pitch - 0x1000) * work->direction), &unit.matrix);
                    RotMatrixY((work->slot << 11) + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->glows[i][j].position.vx;
                    unit.matrix.t[1] = work->glows[i][j].position.vy;
                    unit.matrix.t[2] = work->glows[i][j].position.vz;
                    unit.super = work->glows[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    n = work->glows[i][j].duration - work->glows[i][j].elapsed;
                    d = work->glows[i][j].duration;
                    work->glows[i][j].red = D_8013DBF8[work->command].red * n / d;
                    work->glows[i][j].green = D_8013DBF8[work->command].green * n / d;
                    work->glows[i][j].blue = D_8013DBF8[work->command].blue * n / d;
                    setPolyFT4(&work->flat);
                    work->flat.tpage = work->tpage[1];
                    work->flat.clut = work->clut[1];
                    setSemiTrans(&work->flat, 1);
                    setUV4(&work->flat, work->glows[i][j].u, 64, work->glows[i][j].u + 31, 64,
                        work->glows[i][j].u, 95, work->glows[i][j].u + 31, 95);
                    setRGB0(&work->flat, work->glows[i][j].red, work->glows[i][j].green, work->glows[i][j].blue);
                    depth = RotAverage4(&work->glow[0], &work->glow[1], &work->glow[2], &work->glow[3],
                        (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                    work->glows[i][j].u += 32;
                    if (work->glows[i][j].u >= 0xE1) {
                        work->glows[i][j].u = 0;
                    }
                    work->glows[i][j].elapsed += step;
                    if (work->glows[i][j].duration < work->glows[i][j].elapsed) {
                        work->glows[i][j].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->count * 3 + work->rows * work->columns && work->flags[2] == 1) {
            result = 2;
        }
    }
    return result;
}
