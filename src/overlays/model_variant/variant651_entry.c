#include "../../types.h"
#include "variant651_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../psyq/abs.h"

/* Header 651's entry: shaded strip rings spin out of a model point, a dome
 * of strips grows over the slot row by row, and each finished row sends up
 * a spinning cone of strips. The setup also prints the work-area size. */

#ifndef VERSION_FRENCH
#ifndef MODEL651_STRING_PAD
#define MODEL651_STRING_PAD "\x8C"
#endif
static const char D_8013CA34[24] = "E_DATA size = %x byte\n\0" MODEL651_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model651State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    SVECTOR v[2];
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, n, d, t;
    s32 x, y;
    u8 u0, u1;
    u32 tex;
    PSXLONG depth;

    work = (Model651State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model651State));
#ifndef VERSION_FRENCH
        printf(D_8013CA34, sizeof(Model651State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CA4C);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        switch (command) {
        case 0:
            work->ring_count = 4;
            work->dome_count = 1;
            break;
        case 1:
            work->ring_count = 0;
            work->dome_count = 0;
            break;
        }
        work->radius = D_8013CA68[command].radius;
        work->height = D_8013CA68[command].height;
        for (i = 0; i < work->ring_count; i++) {
            work->rings[i].duration = 0x40;
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].red = D_8013CA68[command].red;
            work->rings[i].green = D_8013CA68[command].green;
            work->rings[i].blue = D_8013CA68[command].blue;
            work->rings[i].u = 0;
            work->rings[i].v = 0;
            setVector(&work->rings[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->rings[i].growth, 0, 0, 0);
            setVector(&work->rings[i].rotation, 0, work->slot * 2048, (i << 12) / work->ring_count);
            setVector(&work->rings[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->dome_count; i++) {
            work->domes[i].duration = 0x80;
            work->domes[i].active = 0;
            work->domes[i].elapsed = 0;
            work->domes[i].mode = 1;
            work->domes[i].coord = Model_GetCameraCoordinateUnit();
            work->domes[i].red = D_8013CA68[command].red;
            work->domes[i].green = D_8013CA68[command].green;
            work->domes[i].blue = D_8013CA68[command].blue;
            work->domes[i].u = 0;
            work->domes[i].v = 0;
            setVector(&work->domes[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->domes[i].growth, 0, 0, (work->height / 2 + 450) * work->direction);
            setVector(&work->domes[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->domes[i].position, 0, 0, 0);
        }
        for (i = 0; i < 8; i++) {
            work->cones[i].duration = 0x30;
            work->cones[i].active = 0;
            work->cones[i].elapsed = 0;
            work->cones[i].mode = 1;
            work->cones[i].coord = Model_GetCameraCoordinateUnit();
            work->cones[i].red = D_8013CA68[command].red;
            work->cones[i].green = D_8013CA68[command].green;
            work->cones[i].blue = D_8013CA68[command].blue;
            work->cones[i].u = 0;
            work->cones[i].v = 0;
            setVector(&work->cones[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->cones[i].growth, 0, 0, 0);
            setVector(&work->cones[i].rotation, 0, work->slot * 2048, i << 9);
            setVector(&work->cones[i].position, 0, 0, 0);
            work->cones[i].position.vy -= 350;
            work->cones[i].position.vz += work->direction * 450;
        }
        work->delay = D_8013CA68[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i][0], work->radius * rsin(i << 8) / 4096,
                work->radius * rcos(i << 8) / 4096, 0);
            setVector(&work->ring[i][1], work->radius * (rsin(i << 8) << 3) / 4096,
                work->radius * (rcos(i << 8) << 3) / 4096, -work->height / 2);
        }
        /* The row, angle and half counters reuse k, j and t; the original
         * register allocation depends on that sharing. */
        for (k = 0; k < 8; k++) {
            for (i = 0; i < 17; i++) {
                v[0].vx = work->radius * rsin((k + 1) * 96 + 256) / 4096 * rsin(j = i << 8) / 4096;
                v[0].vy = work->radius * rsin((k + 1) * 96 + 256) / 4096 * rcos(j) / 4096;
                v[0].vz = work->height / 2 - work->height * k / 8;
                x = rsin((k + 1) * 96 + 256);
                v[1].vx = work->radius * ((x << 2) + x) / 4096 * rsin(j) / 4096;
                y = rsin((k + 1) * 96 + 256);
                v[1].vy = work->radius * ((y << 2) + y) / 4096 * rcos(j) / 4096;
                v[1].vz = work->height / 2 - work->height * k / 8 - work->height / 4;
                ls = D_800FE168;
                RotMatrixZ(k << 9, &ls);
                for (t = 0; t < 2; t++) {
                    PushMatrix();
                    ApplyMatrixSV(&ls, &v[t], &work->dome[i][t][k]);
                    PopMatrix();
                }
            }
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->outer[i][0], work->radius * (rsin(i << 8) << 1) / 4096,
                work->radius * (rcos(i << 8) << 1) / 4096, 0);
            setVector(&work->outer[i][1], work->radius * (rsin(i << 8) << 2) / 4096,
                work->radius * (rcos(i << 8) << 2) / 4096, work->height / 15);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->flags[0] / 4 < work->ring_count) {
                if ((work->flags[0] & 3) == 0) {
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 5);
                    GsGetLwUnit(&unit, &ls);
                    work->rings[work->flags[0] / 4].active = 1;
                    applyVector(&work->rings[work->flags[0] / 4].position, ls.t[0], ls.t[1], ls.t[2], +=);
                }
                work->flags[0]++;
            }
            if (work->elapsed >= 11 && work->flags[1] == 0) {
                unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 5);
                GsGetLwUnit(&unit, &ls);
                work->domes[0].active = 1;
                applyVector(&work->domes[0].position, ls.t[0], ls.t[1], ls.t[2], +=);
                n = work->height * work->domes[0].duration
                    / ((ABS(work->domes[0].growth.vz) + ABS(work->domes[0].position.vz)) * 8);
                for (i = 0; i < 8; i++) {
                    work->row_timer[i] = n * (i + 2);
                }
                work->flags[1] = 1;
            }
            break;
        case 1:
            if (work->flags[0] < 7) {
                if (work->elapsed >= work->flags[0] * 16) {
                    work->cones[work->flags[0]].active = 1;
                    /* The zero x/y adds keep the original's dead loads. */
                    applyVector(&work->cones[work->flags[0]].position, 0, 0, work->direction * 100, +=);
                    if (work->flags[0] == 6) {
                        work->flags[1] = result = 1;
                    }
                    work->flags[0]++;
                }
                if (work->flags[1] != 1) {
                    result = 4;
                }
            }
            break;
        }
        PushMatrix();
        inactive = 0;
        for (i = 0; i < work->ring_count; i++) {
            if (work->rings[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = (work->rings[i].elapsed << 12) / work->rings[i].duration;
                setVector(&work->rings[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->rings[i].scale);
                RotMatrixZ(work->rings[i].rotation.vz, &unit.matrix);
                RotMatrixY(work->rings[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->rings[i].position.vx;
                unit.matrix.t[1] = work->rings[i].position.vy;
                unit.matrix.t[2] = work->rings[i].position.vz;
                unit.super = work->rings[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->rings[i].rotation.vz -= step * 10;
                if (work->rings[i].duration / 2 < work->rings[i].elapsed) {
                    n = (work->rings[i].duration - work->rings[i].elapsed) * 2;
                    d = work->rings[i].duration;
                    work->rings[i].red = D_8013CA68[work->command].red * n / d;
                    work->rings[i].green = D_8013CA68[work->command].green * n / d;
                    work->rings[i].blue = D_8013CA68[work->command].blue * n / d;
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->quad);
                    work->quad.tpage = work->tpage[0];
                    work->quad.clut = work->clut[0];
                    setSemiTrans(&work->quad, 1);
                    u0 = work->rings[i].u + k * 127 / 16;
                    u1 = work->rings[i].u + (k + 1) * 127 / 16;
                    setUV4(&work->quad, u0, work->rings[i].v, u1, work->rings[i].v,
                        u0, work->rings[i].v + 63, u1, work->rings[i].v + 63);
                    setRGB0(&work->quad, 0, 0, 0);
                    setRGB1(&work->quad, 0, 0, 0);
                    setRGB2(&work->quad, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                    setRGB3(&work->quad, work->rings[i].red, work->rings[i].green, work->rings[i].blue);
                    depth = RotAverage4(&work->ring[k][0], &work->ring[k + 1][0], &work->ring[k][1],
                        &work->ring[k + 1][1], (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                    }
                }
                work->rings[i].elapsed += step;
                if (work->rings[i].duration < work->rings[i].elapsed) {
                    work->rings[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->dome_count; i++) {
            if (work->domes[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                if (work->domes[i].elapsed < (u16)(work->domes[i].duration / 3)) {
                    n = (work->domes[i].elapsed << 12) / (u16)(work->domes[i].duration / 3);
                } else {
                    n = 0x1000;
                }
                setVector(&work->domes[i].scale, n, n, n);
                work->domes[i].scale.vz += work->domes[i].scale.vz / 8 * rsin(work->domes[i].elapsed * 100) / 4096;
                ScaleMatrix(&unit.matrix, &work->domes[i].scale);
                RotMatrixZ(work->domes[i].rotation.vz, &unit.matrix);
                RotMatrixY(work->domes[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->domes[i].position.vx;
                unit.matrix.t[1] = work->domes[i].position.vy;
                unit.matrix.t[2] = work->domes[i].position.vz
                    + (work->domes[i].growth.vz - work->domes[i].position.vz) * work->domes[i].elapsed
                        / work->domes[i].duration;
                unit.super = work->domes[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                work->domes[i].rotation.vz += step * 20;
                for (t = 0; t < 8; t++) {
                    if (work->row_timer[t] < work->domes[i].duration) {
                        for (k = 0; k < 16; k++) {
                            setPolyGT4(&work->quad);
                            work->quad.tpage = work->tpage[0];
                            work->quad.clut = work->clut[0];
                            setSemiTrans(&work->quad, 1);
                            u0 = work->domes[i].u + k * 127 / 16;
                            u1 = work->domes[i].u + (k + 1) * 127 / 16;
                            setUV4(&work->quad, u0, work->domes[i].v, u1, work->domes[i].v,
                                u0, work->domes[i].v + 63, u1, work->domes[i].v + 63);
                            setRGB0(&work->quad, 0, 0, 0);
                            setRGB1(&work->quad, 0, 0, 0);
                            setRGB2(&work->quad, work->domes[i].red, work->domes[i].green, work->domes[i].blue);
                            setRGB3(&work->quad, work->domes[i].red, work->domes[i].green, work->domes[i].blue);
                            depth = RotAverage4(&work->dome[k][0][t], &work->dome[k + 1][0][t],
                                &work->dome[k][1][t], &work->dome[k + 1][1][t], (PSXLONG *)&work->quad.x0,
                                (PSXLONG *)&work->quad.x1, (PSXLONG *)&work->quad.x2,
                                (PSXLONG *)&work->quad.x3, &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                    if ((work->domes[i].duration < work->row_timer[t]
                            || work->domes[i].duration < (work->row_timer[t] += step))
                        && work->cones[t].growth.vx == 0) {
                        work->cones[t].active = 1;
                        work->cones[t].position.vy = unit.matrix.t[1];
                        work->cones[t].growth.vx = 1;
                        if (t == 0) {
                            result = 1;
                        } else if (result != 1) {
                            result = 4;
                        }
                    }
                }
                work->domes[i].elapsed += step;
                if (work->domes[i].duration < work->domes[i].elapsed) {
                    work->domes[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < 8; i++) {
            if (work->cones[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                n = (work->cones[i].elapsed * 5 << 12) / work->cones[i].duration + 0x1000;
                setVector(&work->cones[i].scale, n, n, n);
                ScaleMatrix(&unit.matrix, &work->cones[i].scale);
                RotMatrixZ(work->cones[i].rotation.vz, &unit.matrix);
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
                    work->cones[i].red = D_8013CA68[work->command].red * n / d;
                    work->cones[i].green = D_8013CA68[work->command].green * n / d;
                    work->cones[i].blue = D_8013CA68[work->command].blue * n / d;
                }
                for (k = 0; k < 16; k++) {
                    setPolyGT4(&work->quad);
                    work->quad.tpage = work->tpage[0];
                    work->quad.clut = work->clut[0];
                    setSemiTrans(&work->quad, 1);
                    u0 = work->cones[i].u + k % 4 * 127 / 4;
                    u1 = work->cones[i].u + (k % 4 + 1) * 127 / 4;
                    setUV4(&work->quad, u0, work->cones[i].v, u1, work->cones[i].v,
                        u0, work->cones[i].v + 63, u1, work->cones[i].v + 63);
                    setRGB0(&work->quad, 0, 0, 0);
                    setRGB1(&work->quad, 0, 0, 0);
                    setRGB2(&work->quad, work->cones[i].red, work->cones[i].green, work->cones[i].blue);
                    setRGB3(&work->quad, work->cones[i].red, work->cones[i].green, work->cones[i].blue);
                    depth = RotAverage4(&work->outer[k][0], &work->outer[k + 1][0], &work->outer[k][1],
                        &work->outer[k + 1][1], (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
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
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->ring_count + work->dome_count + 8) {
            result = 2;
        }
    }
    return result;
}
