#include "../../types.h"
#include "variant653_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../psyq/abs.h"

/* Header 653's entry: an emitter swings out from the slot and back, dropping
 * textured shells as it goes. At the far end it releases a ring of cones,
 * and each cone launches a wire sphere of lines. The setup also prints the
 * work-area size. */

#ifndef VERSION_FRENCH
#ifndef MODEL653_STRING_PAD
#define MODEL653_STRING_PAD "\x14"
#endif
static const char D_8013D4F4[24] = "E_DATA size = %x byte\n\0" MODEL653_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model653State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    SVECTOR pos;
    PSXLONG interpolation, flag;
    s32 result, l, step, inactive;
    s32 i, j, k, n, d;
    PSXLONG depth;
    u32 tex;
    u8 r, g, b;

    work = (Model653State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model653State));
#ifndef VERSION_FRENCH
        printf(D_8013D4F4, sizeof(Model653State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        work->other_slot = work->slot == 0;
        Model_CopySlotPosition(work->other_slot, (u16 *)&work->target);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D50C);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        if (command == 0) {
            work->emitter_count = 1;
            work->shell_count = 32;
            work->cone_count = 8;
        }
        work->radius = D_8013D528[command].radius;
        work->height = D_8013D528[command].height;
        for (i = 0; i < work->emitter_count; i++) {
            work->emitters[i].duration = 500;
            work->emitters[i].active = 0;
            work->emitters[i].elapsed = 0;
            work->emitters[i].mode = 1;
            work->emitters[i].coord = Model_GetCameraCoordinateUnit();
            work->emitters[i].red = D_8013D528[command].red;
            work->emitters[i].green = D_8013D528[command].green;
            work->emitters[i].blue = D_8013D528[command].blue;
            work->emitters[i].u = 0;
            work->emitters[i].v = 0;
            setVector(&work->emitters[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->emitters[i].growth, 200, 0, work->target.vz);
            setVector(&work->emitters[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->emitters[i].position, 0, 0, 0);
        }
        /* Shells, cones and spheres are indexed by the emitter plus child index.
         * The vector fields go through (&array[child])[emitter], whose separate
         * address arithmetic gives the original's second strength-reduced pointer. */
        for (j = 0; j < work->emitter_count; j++) {
            for (i = 0; i < work->shell_count; i++) {
                work->shells[j + i].duration = 0x40;
                work->shells[j + i].active = 0;
                work->shells[j + i].elapsed = 0;
                work->shells[j + i].mode = 1;
                work->shells[j + i].coord = Model_GetCameraCoordinateUnit();
                work->shells[j + i].red = D_8013D528[command].red;
                work->shells[j + i].green = D_8013D528[command].green;
                work->shells[j + i].blue = D_8013D528[command].blue;
                work->shells[j + i].u = 0;
                work->shells[j + i].v = 0;
                setVector(&(&work->shells[i])[j].scale, 0x1000, 0x1000, 0x1000);
                setVector(&(&work->shells[i])[j].growth, 0, 0, 0);
                setVector(&(&work->shells[i])[j].rotation, 0, work->slot * 2048, 0);
                setVector(&(&work->shells[i])[j].position, 0, 0, 0);
            }
        }
        for (j = 0; j < work->emitter_count; j++) {
            for (i = 0; i < work->cone_count; i++) {
                work->cones[j + i].duration = 0x50;
                work->cones[j + i].active = 0;
                work->cones[j + i].elapsed = 0;
                work->cones[j + i].mode = 1;
                work->cones[j + i].coord = Model_GetCameraCoordinateUnit();
                work->cones[j + i].red = D_8013D528[command].red;
                work->cones[j + i].green = D_8013D528[command].green;
                work->cones[j + i].blue = D_8013D528[command].blue;
                setVector(&(&work->cones[i])[j].scale, 0, 0, 0);
                setVector(&(&work->cones[i])[j].growth, 0, 0, 0);
                setVector(&(&work->cones[i])[j].rotation, 0,
                    (i << 12) / work->cone_count + work->slot * 2048, 0);
                if (i == 0) {
                    setVector(&work->cones[j].position, 0, 0, 0);
                } else {
                    setVector(&(&work->cones[i])[j].position,
                        work->radius / 2 * rsin(((i - 1) << 12) / (work->cone_count - 1)) / 4096, 0,
                        work->radius / 2 * rcos(((i - 1) << 12) / (work->cone_count - 1)) / 4096);
                }
            }
        }
        for (j = 0; j < work->emitter_count; j++) {
            for (i = 0; i < work->cone_count; i++) {
                work->spheres[j + i].duration = 0x50;
                work->spheres[j + i].active = 0;
                work->spheres[j + i].elapsed = 0;
                work->spheres[j + i].mode = 1;
                work->spheres[j + i].coord = Model_GetCameraCoordinateUnit();
                work->spheres[j + i].red = D_8013D528[command].line_red;
                work->spheres[j + i].green = D_8013D528[command].line_green;
                work->spheres[j + i].blue = D_8013D528[command].line_blue;
                setVector(&(&work->spheres[i])[j].scale, 0, 0, 0);
                setVector(&(&work->spheres[i])[j].growth, 0, 0, 0);
                setVector(&(&work->spheres[i])[j].rotation, (i << 12) / work->cone_count,
                    (i << 12) / work->cone_count + work->slot * 2048, (i << 12) / work->cone_count);
                setVector(&(&work->spheres[i])[j].position, 0, 0, 0);
            }
        }
        work->delay = D_8013D528[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 7; i++) {
            for (j = 0; j < 17; j++) {
                setVector(&work->shell[j][i], work->radius * i / 6 * rsin(j << 8) / 4096, 0,
                    work->radius * i / 6 * rcos(j << 8) / 4096);
            }
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i], (u16)(work->radius / 10) * rsin(i << 8) / 4096, 0,
                (u16)(work->radius / 10) * rcos(i << 8) / 4096);
        }
        /* The doubled radius multiplies by a variable, which keeps the mult
         * instructions and holds the 2 in the callee-saved register n uses. */
        n = 2;
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 9; j++) {
                setVector(&work->sphere[j][i],
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
        /* Nested tests: a combined && lets GCC merge the byte and halfword
         * loads into one masked word compare. */
        if (work->command == 0) {
            if (work->elapsed == 0) {
                Model_CopySlotPosition(work->slot, (u16 *)&pos);
                work->emitters[0].active = 1;
                work->emitters[0].position.vx += pos.vx;
                work->emitters[0].position.vy -= 140;
                work->emitters[0].position.vz += pos.vz;
                work->shadow = Model_SetSlotShadowEnabled(work->slot, 0);
            }
        }
        PushMatrix();
        inactive = 0;
        for (i = 0; i < work->emitter_count; i++) {
            if (work->emitters[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->emitters[i].scale);
                RotMatrixY(work->emitters[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->emitters[i].position.vx;
                unit.matrix.t[1] = work->emitters[i].position.vy;
                if (work->emitters[i].elapsed < 160) {
                    unit.matrix.t[2] = work->emitters[i].position.vz;
                } else if (work->emitters[i].elapsed < 250) {
                    unit.matrix.t[0] = work->emitters[i].position.vx
                        + work->emitters[i].growth.vx
                            * rsin(((work->emitters[i].elapsed - 160) << 13) / 90) / 4096;
                    unit.matrix.t[2] = work->emitters[i].position.vz
                        + (work->emitters[i].growth.vz - work->emitters[i].position.vz)
                            * (work->emitters[i].elapsed - 160) / 90;
                    if ((work->flags[1] & 3) == 3) {
                        work->shells[i + work->flags[0]].active = 1;
                        work->shells[i + work->flags[0]].elapsed = 0;
                        work->shells[i + work->flags[0]].v = work->emitters[i].v;
                        (&work->shells[work->flags[0]])[i].position.vx = unit.matrix.t[0];
                        (&work->shells[work->flags[0]])[i].position.vy = unit.matrix.t[1];
                        (&work->shells[work->flags[0]])[i].position.vz = unit.matrix.t[2];
                        if (++work->flags[0] >= work->shell_count) {
                            work->flags[0] = 0;
                        }
                    }
                    work->flags[1]++;
                } else if (work->emitters[i].elapsed < 340) {
                    unit.matrix.t[2] = work->emitters[i].growth.vz;
                    work->flags[1] = 0;
                    if (work->flags[2] == 0) {
                        for (j = 0; j < work->cone_count; j++) {
                            work->cones[i + j].active = 1;
                            applyVector(&(&work->cones[j])[i].position, unit.matrix.t[0], unit.matrix.t[1],
                                unit.matrix.t[2], +=);
                        }
                        work->flags[2] = 1;
                    }
                } else if (work->emitters[i].elapsed < 430) {
                    unit.matrix.t[0] = work->emitters[i].position.vx
                        + work->emitters[i].growth.vx
                            * rsin(((work->emitters[i].elapsed - 340) << 13) / 90) / 4096;
                    unit.matrix.t[2] = work->emitters[i].growth.vz
                        + (work->emitters[i].position.vz - work->emitters[i].growth.vz)
                            * (work->emitters[i].elapsed - 340) / 90;
                    if ((work->flags[1] & 3) == 3) {
                        work->shells[i + work->flags[0]].active = 1;
                        work->shells[i + work->flags[0]].elapsed = 0;
                        work->shells[i + work->flags[0]].v = work->emitters[i].v;
                        (&work->shells[work->flags[0]])[i].position.vx = unit.matrix.t[0];
                        (&work->shells[work->flags[0]])[i].position.vy = unit.matrix.t[1];
                        (&work->shells[work->flags[0]])[i].position.vz = unit.matrix.t[2];
                        if (++work->flags[0] >= work->shell_count) {
                            work->flags[0] = 0;
                        }
                    }
                    work->flags[1]++;
                } else {
                    unit.matrix.t[2] = work->emitters[i].position.vz;
                }
                unit.super = work->emitters[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->emitters[i].elapsed < (u16)(work->emitters[i].duration / 5)) {
                    n = work->emitters[i].elapsed * 5;
                    d = work->emitters[i].duration;
                    work->emitters[i].red = D_8013D528[work->command].red * n / d;
                    work->emitters[i].green = D_8013D528[work->command].green * n / d;
                    work->emitters[i].blue = D_8013D528[work->command].blue * n / d;
                }
                if (work->emitters[i].duration * 9 / 10 < work->emitters[i].elapsed) {
                    n = (work->emitters[i].duration - work->emitters[i].elapsed) * 10;
                    d = work->emitters[i].duration;
                    work->emitters[i].red = D_8013D528[work->command].red * n / d;
                    work->emitters[i].green = D_8013D528[work->command].green * n / d;
                    work->emitters[i].blue = D_8013D528[work->command].blue * n / d;
                }
                for (k = 0; k < 6; k++) {
                    for (j = 0; j < 16; j++) {
                        setPolyGT4(&work->quad);
                        work->quad.tpage = work->tpage[0];
                        work->quad.clut = work->clut[0];
                        setSemiTrans(&work->quad, 1);
                        setUV4(&work->quad, work->emitters[i].u, work->emitters[i].v + 32,
                            work->emitters[i].u + 255, work->emitters[i].v + 32,
                            work->emitters[i].u, work->emitters[i].v,
                            work->emitters[i].u + 255, work->emitters[i].v);
                        n = 6 - k;
                        r = work->emitters[i].red * n / 6;
                        g = work->emitters[i].green * n / 6;
                        b = work->emitters[i].blue * n / 6;
                        setRGB0(&work->quad, r, g, b);
                        setRGB1(&work->quad, r, g, b);
                        n = 5 - k;
                        r = work->emitters[i].red * n / 6;
                        g = work->emitters[i].green * n / 6;
                        b = work->emitters[i].blue * n / 6;
                        setRGB2(&work->quad, r, g, b);
                        setRGB3(&work->quad, r, g, b);
                        depth = RotAverage4(&work->shell[j][k], &work->shell[j + 1][k], &work->shell[j][k + 1],
                            &work->shell[j + 1][k + 1], (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,
                            (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                        }
                    }
                }
                work->emitters[i].v += step;
                if (work->emitters[i].v >= 32) {
                    work->emitters[i].v = 0;
                }
                work->emitters[i].elapsed += step;
                if (work->emitters[i].duration < work->emitters[i].elapsed) {
                    work->emitters[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (l = 0; l < work->emitter_count; l++) {
            for (i = 0; i < work->shell_count; i++) {
                if (work->shells[l + i].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &(&work->shells[i])[l].scale);
                    RotMatrixY(work->shells[l + i].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->shells[l + i].position.vx;
                    unit.matrix.t[1] = work->shells[l + i].position.vy;
                    unit.matrix.t[2] = work->shells[l + i].position.vz;
                    unit.super = work->shells[l + i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->shells[l + i].duration / 2 < work->shells[l + i].elapsed) {
                        n = (work->shells[l + i].duration - work->shells[l + i].elapsed) * 2;
                        d = work->shells[l + i].duration;
                        work->shells[l + i].red = D_8013D528[work->command].red * n / d;
                        work->shells[l + i].green = D_8013D528[work->command].green * n / d;
                        work->shells[l + i].blue = D_8013D528[work->command].blue * n / d;
                    }
                    for (k = 0; k < 6; k++) {
                        for (j = 0; j < 16; j++) {
                            setPolyGT4(&work->quad);
                            work->quad.tpage = work->tpage[0];
                            work->quad.clut = work->clut[0];
                            setSemiTrans(&work->quad, 1);
                            setUV4(&work->quad, work->shells[l + i].u, work->shells[l + i].v + 32,
                                work->shells[l + i].u + 255, work->shells[l + i].v + 32,
                                work->shells[l + i].u, work->shells[l + i].v,
                                work->shells[l + i].u + 255, work->shells[l + i].v);
                            n = 6 - k;
                            r = work->shells[l + i].red * n / 6;
                            g = work->shells[l + i].green * n / 6;
                            b = work->shells[l + i].blue * n / 6;
                            setRGB0(&work->quad, r, g, b);
                            setRGB1(&work->quad, r, g, b);
                            n = 5 - k;
                            r = work->shells[l + i].red * n / 6;
                            g = work->shells[l + i].green * n / 6;
                            b = work->shells[l + i].blue * n / 6;
                            setRGB2(&work->quad, r, g, b);
                            setRGB3(&work->quad, r, g, b);
                            depth = RotAverage4(&work->shell[j][k], &work->shell[j + 1][k],
                                &work->shell[j][k + 1], &work->shell[j + 1][k + 1], (PSXLONG *)&work->quad.x0,
                                (PSXLONG *)&work->quad.x1, (PSXLONG *)&work->quad.x2,
                                (PSXLONG *)&work->quad.x3, &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->quad, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                    work->shells[l + i].elapsed += step;
                    if (work->shells[l + i].duration < work->shells[l + i].elapsed) {
                        work->shells[l + i].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (l = 0; l < work->emitter_count; l++) {
            for (i = 0; i < work->cone_count; i++) {
                if (work->cones[l + i].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    n = 0x1000;
                    if (work->cones[l + i].elapsed < (u16)(work->cones[l + i].duration / 10)) {
                        n = work->cones[l + i].elapsed * 0xA000 / work->cones[l + i].duration;
                    }
                    if (work->cones[l + i].elapsed > work->cones[l + i].duration * 9 / 10) {
                        n = (work->cones[l + i].duration - work->cones[l + i].elapsed) * 0xA000
                            / work->cones[l + i].duration;
                    }
                    setVector(&(&work->cones[i])[l].scale, n, n, n);
                    ScaleMatrix(&unit.matrix, &(&work->cones[i])[l].scale);
                    RotMatrixY(work->cones[l + i].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->cones[l + i].position.vx;
                    unit.matrix.t[1] = work->cones[l + i].position.vy;
                    unit.matrix.t[2] = work->cones[l + i].position.vz;
                    unit.super = work->cones[l + i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    pos.vx = 0;
                    pos.vy = -work->height;
                    pos.vz = 0;
                    for (j = 0; j < 16; j++) {
                        setPolyG3(&work->tri);
                        n = rsin(j << 8);
                        /* Folding the absolute value into the red product loads red before
                         * the sign test, as the original does. */
                        r = work->cones[l + i].red * (n = ABS(n)) / 4096;
                        g = work->cones[l + i].green * n / 4096;
                        b = work->cones[l + i].blue * n / 4096;
                        setRGB1(&work->tri, r, g, b);
                        setRGB0(&work->tri, r, g, b);
                        n = rsin((j + 1) << 8);
                        r = work->cones[l + i].red * (n = ABS(n)) / 4096;
                        g = work->cones[l + i].green * n / 4096;
                        b = work->cones[l + i].blue * n / 4096;
                        setRGB2(&work->tri, r, g, b);
                        depth = RotAverage3(&pos, &work->ring[j], &work->ring[j + 1], (PSXLONG *)&work->tri.x0,
                            (PSXLONG *)&work->tri.x1, (PSXLONG *)&work->tri.x2, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->tri, func_80058F10(), (u16)depth);
                        }
                    }
                    if ((u16)(work->cones[l + i].duration / 20) < work->cones[l + i].elapsed
                        && work->cones[l + i].growth.vx == 0) {
                        work->spheres[l + i].active = 1;
                        setVector(&(&work->spheres[i])[l].position, unit.matrix.t[0], -350, unit.matrix.t[2]);
                        work->cones[l + i].growth.vx = 1;
                        if (i == 0 && l == 0) {
                            result = 1;
                        }
                    }
                    work->cones[l + i].elapsed += step;
                    if (work->cones[l + i].duration < work->cones[l + i].elapsed) {
                        work->cones[l + i].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (l = 0; l < work->emitter_count; l++) {
            for (i = 0; i < work->cone_count; i++) {
                if (work->spheres[l + i].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    n = (work->spheres[l + i].elapsed << 12) / work->spheres[l + i].duration;
                    setVector(&(&work->spheres[i])[l].scale, n, n, n);
                    ScaleMatrix(&unit.matrix, &(&work->spheres[i])[l].scale);
                    RotMatrixZ(work->spheres[l + i].rotation.vz, &unit.matrix);
                    RotMatrixX(work->spheres[l + i].rotation.vx, &unit.matrix);
                    RotMatrixY(work->spheres[l + i].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->spheres[l + i].position.vx;
                    unit.matrix.t[1] = work->spheres[l + i].position.vy;
                    unit.matrix.t[2] = work->spheres[l + i].position.vz;
                    unit.super = work->spheres[l + i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->spheres[l + i].duration / 2 < work->spheres[l + i].elapsed) {
                        n = (work->spheres[l + i].duration - work->spheres[l + i].elapsed) * 2;
                        d = work->spheres[l + i].duration;
                        work->spheres[l + i].red = D_8013D528[work->command].line_red * n / d;
                        work->spheres[l + i].green = D_8013D528[work->command].line_green * n / d;
                        work->spheres[l + i].blue = D_8013D528[work->command].line_blue * n / d;
                    }
                    for (j = 1; j < 8; j++) {
                        for (k = 0; k < 8; k++) {
                            work->line.attribute = 0x50000000;
                            work->line.r0 = work->spheres[l + i].red;
                            work->line.g0 = work->spheres[l + i].green;
                            work->line.b0 = work->spheres[l + i].blue;
                            work->line.r1 = work->spheres[l + i].red;
                            work->line.g1 = work->spheres[l + i].green;
                            work->line.b1 = work->spheres[l + i].blue;
                            RotTransPers(&work->sphere[k][j], (PSXLONG *)&work->line.x0, &interpolation, &flag);
                            depth = RotTransPers(&work->sphere[k][j], (PSXLONG *)&work->line.x1,
                                &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortGLine(&work->line, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                    work->spheres[l + i].elapsed += step;
                    if (work->spheres[l + i].duration < work->spheres[l + i].elapsed) {
                        work->spheres[l + i].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->emitter_count * (work->shell_count + 1 + work->cone_count * 2)) {
            result = 2;
            Model_SetSlotShadowEnabled(work->slot, work->shadow);
        }
    }
    return result;
}
