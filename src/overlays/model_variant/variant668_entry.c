#include "../../types.h"
#include "variant668_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 668's entry: a spinning shell of eight petals, each a strip of
 * Gouraud-shaded bands, with a sprite at its centre, swells from a model
 * point while a screen flash fades. The setup also prints the work-area size. */

/* The work-size format string; retail's padding byte after the terminator is 0x14. */
#ifndef VERSION_FRENCH
static const char D_8013C0C4[24] = "E_DATA size = %x byte\n\0\x14";
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model668State *work;
    MATRIX ls, m;
    GsCOORDUNIT unit;
    Model668ViewSnapshot view;
    SVECTOR points[2];
    u8 r[2], g[2], b[2];
    PSXLONG interpolation, flag;
    s32 result, i, j, k, n, step, inactive;
    s32 value, duration, offset, sign;
    u32 tex;
    PSXLONG depth;

    work = (Model668State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model668State));
#ifndef VERSION_FRENCH
        printf(D_8013C0C4, sizeof(Model668State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013C0DC);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        switch (command) {
        case 0:
        case 1:
            work->shell_count = 1;
            break;
        }
        work->radius = D_8013C0F8[command].radius;
        work->twist = D_8013C0F8[command].twist;
        for (i = 0; i < work->shell_count; i++) {
            work->shells[i].duration = 64;
            work->shells[i].active = 0;
            work->shells[i].elapsed = 0;
            work->shells[i].mode = 1;
            work->shells[i].coord = Model_GetCameraCoordinateUnit();
            work->shells[i].color[0] = D_8013C0F8[command].color[0];
            work->shells[i].color[1] = D_8013C0F8[command].color[1];
            work->shells[i].color[2] = D_8013C0F8[command].color[2];
            work->shells[i].u = 0;
            work->shells[i].v = 0;
            setVector(&work->shells[i].scale, 4096, 4096, 4096);
            setVector(&work->shells[i].spin, 0, 0, 0);
            setVector(&work->shells[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->shells[i].position, 0, 0, 0);
            work->shells[i].position.vy -= 350;
            work->shells[i].position.vz += work->direction * 450;
        }
        work->flash.duration = 64;
        work->flash.active = 0;
        work->flash.elapsed = 0;
        work->flash.mode = 1;
        work->flash.coord = Model_GetCameraCoordinateUnit();
        work->flash.color[0] = D_8013C0F8[command].color[3];
        work->flash.color[1] = D_8013C0F8[command].color[4];
        work->flash.color[2] = D_8013C0F8[command].color[5];
        work->delay = D_8013C0F8[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        /* value and duration double as the radius and twist so they take
         * retail's callee-saved registers. */
        value = work->radius;
        duration = work->twist;
        for (j = 0; j < 8; j++) {
            for (i = 0; i < 8; i++) {
                for (k = 0; k < 9; k++) {
                    offset = k * duration / 8;
                    setVector(&points[0], offset,
                        value * rcos(k << 7) / 4096 + (k % 2 * 2 - 1) * value * rcos(k << 7) / 4096, 0);
                    /* Recomputing vx leaves retail's dead reload of k * twist. */
                    setVector(&points[1], k * duration / 8,
                        value * -rcos(k << 7) / 4096 + (k % 2 * 2 - 1) * value * rcos(k << 7) / 4096, 0);
                    PushMatrix();
                    m = D_800FE168;
                    RotMatrixX(k << 9, &m);
                    sign = i % 2 * 2 - 1;
                    RotMatrixZ(sign * (i << 12) / 8, &m);
                    RotMatrixX(sign * (i << 12) / 8, &m);
                    RotMatrixY(sign * (i << 12) / 8, &m);
                    RotMatrixY(duration + (j << 9), &m);
                    RotMatrixZ(duration + sign * -((j << 9) + j * 568), &m);
                    RotMatrixX(duration + sign * (j << 12) / 8 + j * 682, &m);
                    for (n = 0; n < 2; n++) {
                        ApplyMatrixSV(&m, &points[n], &work->grid[n][k][i][j]);
                    }
                    PopMatrix();
                }
            }
        }
        value = work->radius * 8;
        setVector(&work->corners[0], -value, -value, 0);
        setVector(&work->corners[1], value, -value, 0);
        setVector(&work->corners[2], -value, value, 0);
        setVector(&work->corners[3], value, value, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
        case 1:
            if (work->elapsed == 0) {
                for (i = 0; i < work->shell_count; i++) {
                    work->shells[i].active = 1;
                }
                work->flash.active = 1;
            }
            break;
        }
        PushMatrix();
        view = *(Model668ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->shell_count; i++) {
            if (work->shells[i].active == 1) {
                if ((work->shells[i].frame & 1) == 0) {
                    value = 0x1000;
                } else {
                    value = 0xB33;
                }
                setVector(&work->shells[i].scale, value, value, value);
                if (work->shells[i].duration / 2 < work->shells[i].elapsed) {
                    value = (work->shells[i].duration - work->shells[i].elapsed) * 2;
                    duration = work->shells[i].duration;
                    work->shells[i].color[0] = D_8013C0F8[work->command].color[0] * value / duration;
                    work->shells[i].color[1] = D_8013C0F8[work->command].color[1] * value / duration;
                    work->shells[i].color[2] = D_8013C0F8[work->command].color[2] * value / duration;
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->shells[i].scale);
                duration = work->shells[i].elapsed * 300;
                RotMatrixZ(duration + work->shells[i].rotation.vz, &unit.matrix);
                RotMatrixX(duration + work->shells[i].rotation.vx, &unit.matrix);
                RotMatrixY(duration + work->shells[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->shells[i].position.vx;
                unit.matrix.t[1] = work->shells[i].position.vy;
                unit.matrix.t[2] = work->shells[i].position.vz;
                unit.super = work->shells[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                for (j = 0; j < 8; j++) {
                    for (k = 0; k < 8; k++) {
                        for (n = 0; n < 8; n++) {
                            setPolyGT4(&work->strip);
                            work->strip.tpage = work->tpage[0];
                            work->strip.clut = work->clut[0];
                            /* After the tpage and clut stores, so code | 2 is not folded. */
                            setSemiTrans(&work->strip, 1);
                            setUV4(&work->strip, work->shells[i].u + 64, work->shells[i].v,
                                work->shells[i].u + 127, work->shells[i].v,
                                work->shells[i].u + 64, work->shells[i].v + 63,
                                work->shells[i].u + 127, work->shells[i].v + 63);
                            r[0] = work->shells[i].color[0] * (8 - n) / 8;
                            g[0] = work->shells[i].color[1] * (8 - n) / 8;
                            b[0] = work->shells[i].color[2] * (8 - n) / 8;
                            r[1] = work->shells[i].color[0] * (7 - n) / 8;
                            g[1] = work->shells[i].color[1] * (7 - n) / 8;
                            b[1] = work->shells[i].color[2] * (7 - n) / 8;
                            setRGB0(&work->strip, r[0], g[0], b[0]);
                            setRGB1(&work->strip, r[0], g[0], b[0]);
                            setRGB2(&work->strip, r[1], g[1], b[1]);
                            setRGB3(&work->strip, r[1], g[1], b[1]);
                            depth = RotAverage4(&work->grid[0][n][k][j], &work->grid[1][n][k][j],
                                &work->grid[0][n + 1][k][j], &work->grid[1][n + 1][k][j],
                                (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                                (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                                &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->shells[i].scale);
                RotMatrixZ(work->shells[i].rotation.vz, &unit.matrix);
                RotMatrixX(work->shells[i].rotation.vx - (view.pitch - 4096) * work->direction, &unit.matrix);
                RotMatrixY(work->shells[i].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                unit.matrix.t[0] = work->shells[i].position.vx;
                unit.matrix.t[1] = work->shells[i].position.vy;
                unit.matrix.t[2] = work->shells[i].position.vz;
                unit.super = work->shells[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->sprite);
                work->sprite.tpage = work->tpage[0];
                work->sprite.clut = work->clut[0];
                setSemiTrans(&work->sprite, 1);
                setUV4(&work->sprite, work->shells[i].u, work->shells[i].v,
                    work->shells[i].u + 63, work->shells[i].v,
                    work->shells[i].u, work->shells[i].v + 63,
                    work->shells[i].u + 63, work->shells[i].v + 63);
                setRGB0(&work->sprite, work->shells[i].color[0], work->shells[i].color[1], work->shells[i].color[2]);
                depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                    (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                    (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                    &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                }
                work->shells[i].frame++;
                work->shells[i].elapsed += step;
                if (work->shells[i].duration < work->shells[i].elapsed) {
                    work->shells[i].active = 0;
                }
                result = 4;
            } else {
                inactive++;
            }
        }
        if (work->flash.active == 1) {
            if (work->flash.elapsed < work->flash.duration) {
                value = work->flash.duration - work->flash.elapsed;
                duration = work->flash.duration;
                work->flash.color[0] = D_8013C0F8[work->command].color[3] * value / duration;
                work->flash.color[1] = D_8013C0F8[work->command].color[4] * value / duration;
                work->flash.color[2] = D_8013C0F8[work->command].color[5] * value / duration;
            }
            setPolyF4(&work->flash_poly);
            setXY4(&work->flash_poly, 0, 0, 320, 0, 0, 240, 320, 240);
            setRGB0(&work->flash_poly, work->flash.color[0], work->flash.color[1], work->flash.color[2]);
            Graphics_SubmitDrawModePacket((u32 *)&work->flash_poly, func_80058F10(), 0, work->flash.mode);
            work->flash.elapsed += step;
            if (work->flash.duration < work->flash.elapsed) {
                work->flash.active = 0;
            }
        } else {
            inactive++;
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->shell_count + 1) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}
