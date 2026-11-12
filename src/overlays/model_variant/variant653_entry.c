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
static const char gModelVariant653_szWorkSizeFormat[24] = "E_DATA size = %x byte\n\0" MODEL653_STRING_PAD;
#endif

s32 ModelVariant653_HandleRequest(u8 *context, s32 command)
{
    Model653State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    SVECTOR pos;
    long interpolation, flag;
    s32 result, l, step, inactive;
    s32 i, j, k, n, d;
    long depth;
    u32 tex;
    u8 r, g, b;

    work = (Model653State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model653State));
#ifndef VERSION_FRENCH
        printf(gModelVariant653_szWorkSizeFormat, sizeof(Model653State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        work->other_slot = work->head.slot == 0;
        Model_CopySlotPosition(work->other_slot, (u16 *)&work->target);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant653_Image);
        work->textures.tpage[0] = tex >> 16;
        work->textures.clut[0] = tex;
        if (command == 0) {
            work->emitter_count = 1;
            work->shell_count = 32;
            work->cone_count = 8;
        }
        work->radius = gModelVariant653_aConfigs[command].radius;
        work->height = gModelVariant653_aConfigs[command].height;
        for (i = 0; i < work->emitter_count; i++) {
            work->emitters[i].head.duration = 500;
            work->emitters[i].head.active = 0;
            work->emitters[i].head.elapsed = 0;
            work->emitters[i].head.mode = 1;
            work->emitters[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->emitters[i].head.color[0] = gModelVariant653_aConfigs[command].red;
            work->emitters[i].head.color[1] = gModelVariant653_aConfigs[command].green;
            work->emitters[i].head.color[2] = gModelVariant653_aConfigs[command].blue;
            work->emitters[i].head.u = 0;
            work->emitters[i].head.v = 0;
            setVector(&work->emitters[i].xform.scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->emitters[i].growth, 200, 0, work->target.vz);
            setVector(&work->emitters[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->emitters[i].xform.position, 0, 0, 0);
        }
        /* Shells, cones and spheres are indexed by the emitter plus child index.
         * The vector fields go through (&array[child])[emitter], whose separate
         * address arithmetic gives the original's second strength-reduced pointer. */
        for (j = 0; j < work->emitter_count; j++) {
            for (i = 0; i < work->shell_count; i++) {
                work->shells[j + i].head.duration = 0x40;
                work->shells[j + i].head.active = 0;
                work->shells[j + i].head.elapsed = 0;
                work->shells[j + i].head.mode = 1;
                work->shells[j + i].xform.coord = Model_GetCameraCoordinateUnit();
                work->shells[j + i].head.color[0] = gModelVariant653_aConfigs[command].red;
                work->shells[j + i].head.color[1] = gModelVariant653_aConfigs[command].green;
                work->shells[j + i].head.color[2] = gModelVariant653_aConfigs[command].blue;
                work->shells[j + i].head.u = 0;
                work->shells[j + i].head.v = 0;
                setVector(&(&work->shells[i])[j].xform.scale, 0x1000, 0x1000, 0x1000);
                setVector(&(&work->shells[i])[j].growth, 0, 0, 0);
                setVector(&(&work->shells[i])[j].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&(&work->shells[i])[j].xform.position, 0, 0, 0);
            }
        }
        for (j = 0; j < work->emitter_count; j++) {
            for (i = 0; i < work->cone_count; i++) {
                work->cones[j + i].head.duration = 0x50;
                work->cones[j + i].head.active = 0;
                work->cones[j + i].head.elapsed = 0;
                work->cones[j + i].head.mode = 1;
                work->cones[j + i].xform.coord = Model_GetCameraCoordinateUnit();
                work->cones[j + i].head.color[0] = gModelVariant653_aConfigs[command].red;
                work->cones[j + i].head.color[1] = gModelVariant653_aConfigs[command].green;
                work->cones[j + i].head.color[2] = gModelVariant653_aConfigs[command].blue;
                setVector(&(&work->cones[i])[j].xform.scale, 0, 0, 0);
                setVector(&(&work->cones[i])[j].growth, 0, 0, 0);
                setVector(&(&work->cones[i])[j].xform.rotation, 0,
                    (i << 12) / work->cone_count + work->head.slot * 2048, 0);
                if (i == 0) {
                    setVector(&work->cones[j].xform.position, 0, 0, 0);
                } else {
                    setVector(&(&work->cones[i])[j].xform.position,
                        work->radius / 2 * rsin(((i - 1) << 12) / (work->cone_count - 1)) / 4096, 0,
                        work->radius / 2 * rcos(((i - 1) << 12) / (work->cone_count - 1)) / 4096);
                }
            }
        }
        for (j = 0; j < work->emitter_count; j++) {
            for (i = 0; i < work->cone_count; i++) {
                work->spheres[j + i].head.duration = 0x50;
                work->spheres[j + i].head.active = 0;
                work->spheres[j + i].head.elapsed = 0;
                work->spheres[j + i].head.mode = 1;
                work->spheres[j + i].xform.coord = Model_GetCameraCoordinateUnit();
                work->spheres[j + i].head.color[0] = gModelVariant653_aConfigs[command].line_red;
                work->spheres[j + i].head.color[1] = gModelVariant653_aConfigs[command].line_green;
                work->spheres[j + i].head.color[2] = gModelVariant653_aConfigs[command].line_blue;
                setVector(&(&work->spheres[i])[j].xform.scale, 0, 0, 0);
                setVector(&(&work->spheres[i])[j].growth, 0, 0, 0);
                setVector(&(&work->spheres[i])[j].xform.rotation, (i << 12) / work->cone_count,
                    (i << 12) / work->cone_count + work->head.slot * 2048, (i << 12) / work->cone_count);
                setVector(&(&work->spheres[i])[j].xform.position, 0, 0, 0);
            }
        }
        work->delay = gModelVariant653_aConfigs[command].delay;
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
        if (work->head.command == 0) {
            if (work->head.elapsed == 0) {
                Model_CopySlotPosition(work->head.slot, (u16 *)&pos);
                work->emitters[0].head.active = 1;
                work->emitters[0].xform.position.vx += pos.vx;
                work->emitters[0].xform.position.vy -= 140;
                work->emitters[0].xform.position.vz += pos.vz;
                work->shadow = Model_SetSlotShadowEnabled(work->head.slot, 0);
            }
        }
        PushMatrix();
        inactive = 0;
        for (i = 0; i < work->emitter_count; i++) {
            if (work->emitters[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                ScaleMatrix(&unit.matrix, &work->emitters[i].xform.scale);
                RotMatrixY(work->emitters[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->emitters[i].xform.position.vx;
                unit.matrix.t[1] = work->emitters[i].xform.position.vy;
                if (work->emitters[i].head.elapsed < 160) {
                    unit.matrix.t[2] = work->emitters[i].xform.position.vz;
                } else if (work->emitters[i].head.elapsed < 250) {
                    unit.matrix.t[0] = work->emitters[i].xform.position.vx
                        + work->emitters[i].growth.vx
                            * rsin(((work->emitters[i].head.elapsed - 160) << 13) / 90) / 4096;
                    unit.matrix.t[2] = work->emitters[i].xform.position.vz
                        + (work->emitters[i].growth.vz - work->emitters[i].xform.position.vz)
                            * (work->emitters[i].head.elapsed - 160) / 90;
                    if ((work->flags[1] & 3) == 3) {
                        work->shells[i + work->flags[0]].head.active = 1;
                        work->shells[i + work->flags[0]].head.elapsed = 0;
                        work->shells[i + work->flags[0]].head.v = work->emitters[i].head.v;
                        (&work->shells[work->flags[0]])[i].xform.position.vx = unit.matrix.t[0];
                        (&work->shells[work->flags[0]])[i].xform.position.vy = unit.matrix.t[1];
                        (&work->shells[work->flags[0]])[i].xform.position.vz = unit.matrix.t[2];
                        if (++work->flags[0] >= work->shell_count) {
                            work->flags[0] = 0;
                        }
                    }
                    work->flags[1]++;
                } else if (work->emitters[i].head.elapsed < 340) {
                    unit.matrix.t[2] = work->emitters[i].growth.vz;
                    work->flags[1] = 0;
                    if (work->flags[2] == 0) {
                        for (j = 0; j < work->cone_count; j++) {
                            work->cones[i + j].head.active = 1;
                            applyVector(&(&work->cones[j])[i].xform.position, unit.matrix.t[0], unit.matrix.t[1],
                                unit.matrix.t[2], +=);
                        }
                        work->flags[2] = 1;
                    }
                } else if (work->emitters[i].head.elapsed < 430) {
                    unit.matrix.t[0] = work->emitters[i].xform.position.vx
                        + work->emitters[i].growth.vx
                            * rsin(((work->emitters[i].head.elapsed - 340) << 13) / 90) / 4096;
                    unit.matrix.t[2] = work->emitters[i].growth.vz
                        + (work->emitters[i].xform.position.vz - work->emitters[i].growth.vz)
                            * (work->emitters[i].head.elapsed - 340) / 90;
                    if ((work->flags[1] & 3) == 3) {
                        work->shells[i + work->flags[0]].head.active = 1;
                        work->shells[i + work->flags[0]].head.elapsed = 0;
                        work->shells[i + work->flags[0]].head.v = work->emitters[i].head.v;
                        (&work->shells[work->flags[0]])[i].xform.position.vx = unit.matrix.t[0];
                        (&work->shells[work->flags[0]])[i].xform.position.vy = unit.matrix.t[1];
                        (&work->shells[work->flags[0]])[i].xform.position.vz = unit.matrix.t[2];
                        if (++work->flags[0] >= work->shell_count) {
                            work->flags[0] = 0;
                        }
                    }
                    work->flags[1]++;
                } else {
                    unit.matrix.t[2] = work->emitters[i].xform.position.vz;
                }
                unit.super = work->emitters[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->emitters[i].head.elapsed < (u16)(work->emitters[i].head.duration / 5)) {
                    n = work->emitters[i].head.elapsed * 5;
                    d = work->emitters[i].head.duration;
                    work->emitters[i].head.color[0] = gModelVariant653_aConfigs[work->head.command].red * n / d;
                    work->emitters[i].head.color[1] = gModelVariant653_aConfigs[work->head.command].green * n / d;
                    work->emitters[i].head.color[2] = gModelVariant653_aConfigs[work->head.command].blue * n / d;
                }
                if (work->emitters[i].head.duration * 9 / 10 < work->emitters[i].head.elapsed) {
                    n = (work->emitters[i].head.duration - work->emitters[i].head.elapsed) * 10;
                    d = work->emitters[i].head.duration;
                    work->emitters[i].head.color[0] = gModelVariant653_aConfigs[work->head.command].red * n / d;
                    work->emitters[i].head.color[1] = gModelVariant653_aConfigs[work->head.command].green * n / d;
                    work->emitters[i].head.color[2] = gModelVariant653_aConfigs[work->head.command].blue * n / d;
                }
                for (k = 0; k < 6; k++) {
                    for (j = 0; j < 16; j++) {
                        setPolyGT4(&work->quad);
                        work->quad.tpage = work->textures.tpage[0];
                        work->quad.clut = work->textures.clut[0];
                        setSemiTrans(&work->quad, 1);
                        setUV4(&work->quad, work->emitters[i].head.u, work->emitters[i].head.v + 32,
                            work->emitters[i].head.u + 255, work->emitters[i].head.v + 32,
                            work->emitters[i].head.u, work->emitters[i].head.v,
                            work->emitters[i].head.u + 255, work->emitters[i].head.v);
                        n = 6 - k;
                        r = work->emitters[i].head.color[0] * n / 6;
                        g = work->emitters[i].head.color[1] * n / 6;
                        b = work->emitters[i].head.color[2] * n / 6;
                        setRGB0(&work->quad, r, g, b);
                        setRGB1(&work->quad, r, g, b);
                        n = 5 - k;
                        r = work->emitters[i].head.color[0] * n / 6;
                        g = work->emitters[i].head.color[1] * n / 6;
                        b = work->emitters[i].head.color[2] * n / 6;
                        setRGB2(&work->quad, r, g, b);
                        setRGB3(&work->quad, r, g, b);
                        depth = RotAverage4(&work->shell[j][k], &work->shell[j + 1][k], &work->shell[j][k + 1],
                            &work->shell[j + 1][k + 1], (long *)&work->quad.x0, (long *)&work->quad.x1,
                            (long *)&work->quad.x2, (long *)&work->quad.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                work->emitters[i].head.v += step;
                if (work->emitters[i].head.v >= 32) {
                    work->emitters[i].head.v = 0;
                }
                work->emitters[i].head.elapsed += step;
                if (work->emitters[i].head.duration < work->emitters[i].head.elapsed) {
                    work->emitters[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (l = 0; l < work->emitter_count; l++) {
            for (i = 0; i < work->shell_count; i++) {
                if (work->shells[l + i].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &(&work->shells[i])[l].xform.scale);
                    RotMatrixY(work->shells[l + i].xform.rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->shells[l + i].xform.position.vx;
                    unit.matrix.t[1] = work->shells[l + i].xform.position.vy;
                    unit.matrix.t[2] = work->shells[l + i].xform.position.vz;
                    unit.super = work->shells[l + i].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->shells[l + i].head.duration / 2 < work->shells[l + i].head.elapsed) {
                        n = (work->shells[l + i].head.duration - work->shells[l + i].head.elapsed) * 2;
                        d = work->shells[l + i].head.duration;
                        work->shells[l + i].head.color[0] = gModelVariant653_aConfigs[work->head.command].red * n / d;
                        work->shells[l + i].head.color[1] = gModelVariant653_aConfigs[work->head.command].green * n / d;
                        work->shells[l + i].head.color[2] = gModelVariant653_aConfigs[work->head.command].blue * n / d;
                    }
                    for (k = 0; k < 6; k++) {
                        for (j = 0; j < 16; j++) {
                            setPolyGT4(&work->quad);
                            work->quad.tpage = work->textures.tpage[0];
                            work->quad.clut = work->textures.clut[0];
                            setSemiTrans(&work->quad, 1);
                            setUV4(&work->quad, work->shells[l + i].head.u, work->shells[l + i].head.v + 32,
                                work->shells[l + i].head.u + 255, work->shells[l + i].head.v + 32,
                                work->shells[l + i].head.u, work->shells[l + i].head.v,
                                work->shells[l + i].head.u + 255, work->shells[l + i].head.v);
                            n = 6 - k;
                            r = work->shells[l + i].head.color[0] * n / 6;
                            g = work->shells[l + i].head.color[1] * n / 6;
                            b = work->shells[l + i].head.color[2] * n / 6;
                            setRGB0(&work->quad, r, g, b);
                            setRGB1(&work->quad, r, g, b);
                            n = 5 - k;
                            r = work->shells[l + i].head.color[0] * n / 6;
                            g = work->shells[l + i].head.color[1] * n / 6;
                            b = work->shells[l + i].head.color[2] * n / 6;
                            setRGB2(&work->quad, r, g, b);
                            setRGB3(&work->quad, r, g, b);
                            depth = RotAverage4(&work->shell[j][k], &work->shell[j + 1][k],
                                &work->shell[j][k + 1], &work->shell[j + 1][k + 1], (long *)&work->quad.x0,
                                (long *)&work->quad.x1, (long *)&work->quad.x2,
                                (long *)&work->quad.x3, &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);
                            }
                        }
                    }
                    work->shells[l + i].head.elapsed += step;
                    if (work->shells[l + i].head.duration < work->shells[l + i].head.elapsed) {
                        work->shells[l + i].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (l = 0; l < work->emitter_count; l++) {
            for (i = 0; i < work->cone_count; i++) {
                if (work->cones[l + i].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    n = 0x1000;
                    if (work->cones[l + i].head.elapsed < (u16)(work->cones[l + i].head.duration / 10)) {
                        n = work->cones[l + i].head.elapsed * 0xA000 / work->cones[l + i].head.duration;
                    }
                    if (work->cones[l + i].head.elapsed > work->cones[l + i].head.duration * 9 / 10) {
                        n = (work->cones[l + i].head.duration - work->cones[l + i].head.elapsed) * 0xA000
                            / work->cones[l + i].head.duration;
                    }
                    setVector(&(&work->cones[i])[l].xform.scale, n, n, n);
                    ScaleMatrix(&unit.matrix, &(&work->cones[i])[l].xform.scale);
                    RotMatrixY(work->cones[l + i].xform.rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->cones[l + i].xform.position.vx;
                    unit.matrix.t[1] = work->cones[l + i].xform.position.vy;
                    unit.matrix.t[2] = work->cones[l + i].xform.position.vz;
                    unit.super = work->cones[l + i].xform.coord;
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
                        r = work->cones[l + i].head.color[0] * (n = ABS(n)) / 4096;
                        g = work->cones[l + i].head.color[1] * n / 4096;
                        b = work->cones[l + i].head.color[2] * n / 4096;
                        setRGB1(&work->tri, r, g, b);
                        setRGB0(&work->tri, r, g, b);
                        n = rsin((j + 1) << 8);
                        r = work->cones[l + i].head.color[0] * (n = ABS(n)) / 4096;
                        g = work->cones[l + i].head.color[1] * n / 4096;
                        b = work->cones[l + i].head.color[2] * n / 4096;
                        setRGB2(&work->tri, r, g, b);
                        depth = RotAverage3(&pos, &work->ring[j], &work->ring[j + 1], (long *)&work->tri.x0,
                            (long *)&work->tri.x1, (long *)&work->tri.x2, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->tri, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                    if ((u16)(work->cones[l + i].head.duration / 20) < work->cones[l + i].head.elapsed
                        && work->cones[l + i].growth.vx == 0) {
                        work->spheres[l + i].head.active = 1;
                        setVector(&(&work->spheres[i])[l].xform.position, unit.matrix.t[0], -350, unit.matrix.t[2]);
                        work->cones[l + i].growth.vx = 1;
                        if (i == 0 && l == 0) {
                            result = 1;
                        }
                    }
                    work->cones[l + i].head.elapsed += step;
                    if (work->cones[l + i].head.duration < work->cones[l + i].head.elapsed) {
                        work->cones[l + i].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (l = 0; l < work->emitter_count; l++) {
            for (i = 0; i < work->cone_count; i++) {
                if (work->spheres[l + i].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    n = (work->spheres[l + i].head.elapsed << 12) / work->spheres[l + i].head.duration;
                    setVector(&(&work->spheres[i])[l].xform.scale, n, n, n);
                    ScaleMatrix(&unit.matrix, &(&work->spheres[i])[l].xform.scale);
                    RotMatrixZ(work->spheres[l + i].xform.rotation.vz, &unit.matrix);
                    RotMatrixX(work->spheres[l + i].xform.rotation.vx, &unit.matrix);
                    RotMatrixY(work->spheres[l + i].xform.rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->spheres[l + i].xform.position.vx;
                    unit.matrix.t[1] = work->spheres[l + i].xform.position.vy;
                    unit.matrix.t[2] = work->spheres[l + i].xform.position.vz;
                    unit.super = work->spheres[l + i].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->spheres[l + i].head.duration / 2 < work->spheres[l + i].head.elapsed) {
                        n = (work->spheres[l + i].head.duration - work->spheres[l + i].head.elapsed) * 2;
                        d = work->spheres[l + i].head.duration;
                        work->spheres[l + i].head.color[0] = gModelVariant653_aConfigs[work->head.command].line_red * n / d;
                        work->spheres[l + i].head.color[1] = gModelVariant653_aConfigs[work->head.command].line_green * n / d;
                        work->spheres[l + i].head.color[2] = gModelVariant653_aConfigs[work->head.command].line_blue * n / d;
                    }
                    for (j = 1; j < 8; j++) {
                        for (k = 0; k < 8; k++) {
                            work->line.attribute = 0x50000000;
                            work->line.r0 = work->spheres[l + i].head.color[0];
                            work->line.g0 = work->spheres[l + i].head.color[1];
                            work->line.b0 = work->spheres[l + i].head.color[2];
                            work->line.r1 = work->spheres[l + i].head.color[0];
                            work->line.g1 = work->spheres[l + i].head.color[1];
                            work->line.b1 = work->spheres[l + i].head.color[2];
                            RotTransPers(&work->sphere[k][j], (long *)&work->line.x0, &interpolation, &flag);
                            depth = RotTransPers(&work->sphere[k][j], (long *)&work->line.x1,
                                &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortGLine(&work->line, Graphics_GetDefaultOrderingTable(), (u16)depth);
                            }
                        }
                    }
                    work->spheres[l + i].head.elapsed += step;
                    if (work->spheres[l + i].head.duration < work->spheres[l + i].head.elapsed) {
                        work->spheres[l + i].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        work->head.elapsed += step;
        if (inactive == work->emitter_count * (work->shell_count + 1 + work->cone_count * 2)) {
            result = 2;
            Model_SetSlotShadowEnabled(work->head.slot, work->shadow);
        }
    }
    return result;
}
