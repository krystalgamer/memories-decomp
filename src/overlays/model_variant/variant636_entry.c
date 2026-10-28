#include "../../types.h"
#include "variant636_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 636's entry: up to six Gouraud streaks leave a model joint in
 * pairs or one by one and fly along arcs, each trailing 16 fading lines.
 * Where a streak lands, a dome of radial lines grows and a ring of
 * camera-facing animated sprites drifts outwards. The setup also prints
 * the work-area size. */

/* The corners' negations go through an int parameter, so the front end
 * does not narrow them to halfword arithmetic: that is retail's three loads
 * of the half-size and the zero-extension before the third negation. */
static inline s32 Model636_Negate(s32 value)
{
    return -value;
}

s32 func_8013B004(u8 *context, s32 command)
{
    Model636State *work;
    MATRIX ls;
    GsCOORDUNIT unit;
    Model636ViewSnapshot view;
    SVECTOR mid;
    PSXLONG interpolation, flag, sxy;
    s32 result, i, j, k, step, inactive;
    /* One temporary carries every fade; keeping it shared keeps the line
     * fades inside the inner loops. */
    s32 value;
    /* The sprites' lifetime, held in a register over the three fades. */
    s32 duration;
    u32 tex;
    PSXLONG depth;

    work = (Model636State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model636State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model636State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CF80);
        work->tpage = tex >> 16;
        work->clut = tex;
        /* Two tests rather than one &&, which the front end would fold into
         * one unsigned range check. */
        if (command < 3) {
            if (command >= 0) {
                work->rows = 6;
                work->cols = 8;
            }
        }
        work->radius = D_8013CF9C[command].radius;
        work->height = D_8013CF9C[command].height;
        for (i = 0; i < work->rows; i++) {
            work->streaks[i].active = 0;
            work->streaks[i].elapsed = 0;
            work->streaks[i].duration = 80;
            work->streaks[i].mode = 1;
            work->streaks[i].coord = Model_GetCameraCoordinateUnit();
            work->streaks[i].color[0] = D_8013CF9C[command].color[0];
            work->streaks[i].color[1] = D_8013CF9C[command].color[1];
            work->streaks[i].color[2] = D_8013CF9C[command].color[2];
            work->streaks[i].stage = 0;
            setVector(&work->streaks[i].scale, 4096, 4096, 4096);
            setVector(&work->streaks[i].target, 0, 0, 0);
            if (command < 3) {
                if (command >= 0) {
                    setVector(&work->streaks[i].rotation, 0, work->slot * 2048, 0);
                    setVector(&work->streaks[i].position, 0, 0, 0);
                }
            }
            switch (i) {
            case 0:
                work->paths[i].reach = 450;
                work->paths[i].apex = 160;
                work->paths[i].rise_scale = 1;
                work->paths[i].rise_divisor = 16;
                work->paths[i].side_scale = 1;
                work->paths[i].side_divisor = 32;
                break;
            case 1:
                work->paths[i].reach = 650;
                work->paths[i].apex = 180;
                work->paths[i].rise_scale = 1;
                work->paths[i].rise_divisor = 16;
                work->paths[i].side_scale = -1;
                work->paths[i].side_divisor = 16;
                break;
            case 2:
                work->paths[i].reach = 650;
                work->paths[i].apex = 200;
                work->paths[i].rise_scale = 1;
                work->paths[i].rise_divisor = 16;
                work->paths[i].side_scale = 1;
                work->paths[i].side_divisor = 16;
                break;
            case 3:
                work->paths[i].reach = 450;
                work->paths[i].apex = 160;
                work->paths[i].rise_scale = 1;
                work->paths[i].rise_divisor = 16;
                work->paths[i].side_scale = -1;
                work->paths[i].side_divisor = 32;
                break;
            case 4:
                work->paths[i].reach = 250;
                work->paths[i].apex = 180;
                work->paths[i].rise_scale = 1;
                work->paths[i].rise_divisor = 16;
                work->paths[i].side_scale = 1;
                work->paths[i].side_divisor = 16;
                break;
            case 5:
                work->paths[i].reach = 250;
                work->paths[i].apex = 200;
                work->paths[i].rise_scale = 1;
                work->paths[i].rise_divisor = 16;
                work->paths[i].side_scale = -1;
                work->paths[i].side_divisor = 16;
                break;
            case 6:
                work->paths[i].reach = 650;
                work->paths[i].apex = 160;
                work->paths[i].rise_scale = 1;
                work->paths[i].rise_divisor = 16;
                work->paths[i].side_scale = 1;
                work->paths[i].side_divisor = 16;
                break;
            case 7:
                work->paths[i].reach = 650;
                work->paths[i].apex = 180;
                work->paths[i].rise_scale = 1;
                work->paths[i].rise_divisor = 16;
                work->paths[i].side_scale = -1;
                work->paths[i].side_divisor = 16;
                break;
            }
        }
        for (i = 0; i < work->rows; i++) {
            work->domes[i].active = 0;
            work->domes[i].elapsed = 0;
            work->domes[i].duration = 64;
            work->domes[i].mode = 1;
            work->domes[i].coord = Model_GetCameraCoordinateUnit();
            work->domes[i].color[0] = D_8013CF9C[command].color[0];
            work->domes[i].color[1] = D_8013CF9C[command].color[1];
            work->domes[i].color[2] = D_8013CF9C[command].color[2];
            work->domes[i].stage = 0;
            setVector(&work->domes[i].scale, 4096, 4096, 4096);
            setVector(&work->domes[i].target, 0, 0, 0);
            if (command < 3) {
                if (command >= 0) {
                    setVector(&work->domes[i].rotation, 0, work->slot * 2048, 0);
                    setVector(&work->domes[i].position, 0, 0, 0);
                }
            }
        }
        for (j = 0; j < work->rows; j++) {
            for (i = 0; i < work->cols; i++) {
                work->sprites[i][j].active = 0;
                work->sprites[i][j].elapsed = 0;
                work->sprites[i][j].duration = 64;
                work->sprites[i][j].mode = 1;
                work->sprites[i][j].coord = Model_GetCameraCoordinateUnit();
                work->sprites[i][j].color[0] = D_8013CF9C[command].color[3];
                work->sprites[i][j].color[1] = D_8013CF9C[command].color[4];
                work->sprites[i][j].color[2] = D_8013CF9C[command].color[5];
                setVector(&work->sprites[i][j].scale, 4096, 4096, 4096);
                setVector(&work->sprites[i][j].target, work->radius * rsin((i << 12) / work->cols) / 4096,
                    work->radius / 2 * (i % 3 + 1), work->radius * rcos((i << 12) / work->cols) / 4096);
                if (command < 3) {
                    if (command >= 0) {
                        setVector(&work->sprites[i][j].rotation, 0, work->slot * 2048, 0);
                        setVector(&work->sprites[i][j].position, 0, 0, 0);
                    }
                }
            }
        }
        work->delay = D_8013CF9C[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 8; j++) {
                setVector(&work->dome[j][i],
                    SquareRoot0(work->radius * work->radius
                        - work->radius * rcos(i << 7) / 4096 * (work->radius * rcos(i << 7) / 4096))
                        * rsin(j << 9) / 4096,
                    work->radius + work->radius * -rcos(i << 7) / 4096,
                    SquareRoot0(work->radius * work->radius
                        - work->radius * rcos(i << 7) / 4096 * (work->radius * rcos(i << 7) / 4096))
                        * rcos(j << 9) / 4096);
            }
        }
        setVector(&work->corners[0], Model636_Negate(work->height), Model636_Negate(work->height), 0);
        setVector(&work->corners[1], work->height, Model636_Negate(work->height), 0);
        setVector(&work->corners[2], Model636_Negate(work->height), work->height, 0);
        setVector(&work->corners[3], work->height, work->height, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 6);
            GsGetLwUnit(&unit, &ls);
            if (work->elapsed == 0 && work->flags[0] == 0) {
                for (j = 0; j < 2; j++) {
                    work->streaks[j].active = 1;
                    setVector(&work->streaks[j].target, (work->slot * 2 - 1) * -ls.t[0], ls.t[1],
                        (work->slot * 2 - 1) * -ls.t[2]);
                    for (i = 0; i < 17; i++) {
                        setVector(&work->trail[i][j], work->streaks[j].target.vx, work->streaks[j].target.vy,
                            work->streaks[j].target.vz);
                    }
                }
                work->flags[0] = 1;
            }
            if (work->elapsed > 10 && work->flags[1] == 0) {
                for (j = 2; j < 4; j++) {
                    work->streaks[j].active = 1;
                    setVector(&work->streaks[j].target, (work->slot * 2 - 1) * -ls.t[0], ls.t[1],
                        (work->slot * 2 - 1) * -ls.t[2]);
                    for (i = 0; i < 17; i++) {
                        setVector(&work->trail[i][j], work->streaks[j].target.vx, work->streaks[j].target.vy,
                            work->streaks[j].target.vz);
                    }
                }
                work->flags[1] = 1;
            }
            if (work->elapsed > 20 && work->flags[2] == 0) {
                for (j = 4; j < 6; j++) {
                    work->streaks[j].active = 1;
                    setVector(&work->streaks[j].target, (work->slot * 2 - 1) * -ls.t[0], ls.t[1],
                        (work->slot * 2 - 1) * -ls.t[2]);
                    for (i = 0; i < 17; i++) {
                        setVector(&work->trail[i][j], work->streaks[j].target.vx, work->streaks[j].target.vy,
                            work->streaks[j].target.vz);
                    }
                }
                work->flags[2] = 1;
            }
            break;
        case 1:
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 6);
            GsGetLwUnit(&unit, &ls);
            if (work->flags[0] < work->rows && work->elapsed >= work->flags[0] * 15) {
                work->streaks[work->flags[0]].active = 1;
                setVector(&work->streaks[work->flags[0]].target, (work->slot * 2 - 1) * -ls.t[0], ls.t[1],
                    (work->slot * 2 - 1) * -ls.t[2]);
                for (i = 0; i < 17; i++) {
                    setVector(&work->trail[i][work->flags[0]], work->streaks[work->flags[0]].target.vx,
                        work->streaks[work->flags[0]].target.vy, work->streaks[work->flags[0]].target.vz);
                }
                work->flags[0]++;
            }
            break;
        case 2:
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 2);
            GsGetLwUnit(&unit, &ls);
            if (work->flags[0] < work->rows && work->elapsed >= work->flags[0] * 30) {
                work->streaks[work->flags[0]].active = 1;
                setVector(&work->streaks[work->flags[0]].target, (work->slot * 2 - 1) * -ls.t[0], ls.t[1],
                    (work->slot * 2 - 1) * -ls.t[2]);
                for (i = 0; i < 17; i++) {
                    setVector(&work->trail[i][work->flags[0]], work->streaks[work->flags[0]].target.vx,
                        work->streaks[work->flags[0]].target.vy, work->streaks[work->flags[0]].target.vz);
                }
                work->flags[0]++;
            }
            break;
        }
        PushMatrix();
        view = *(Model636ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->rows; i++) {
            if (work->streaks[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->streaks[i].scale);
                RotMatrixY(work->streaks[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->streaks[i].position.vx;
                unit.matrix.t[1] = work->streaks[i].position.vy;
                unit.matrix.t[2] = work->streaks[i].position.vz;
                unit.super = work->streaks[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                for (j = 16; j > 0; j--) {
                    work->trail[j][i] = work->trail[j - 1][i];
                }
                if (work->streaks[i].elapsed < work->streaks[i].duration - 32) {
                    setVector(&work->trail[0][i],
                        work->streaks[i].target.vx
                            + work->paths[i].side_scale * work->streaks[i].elapsed
                                * (work->streaks[i].elapsed - work->paths[i].apex) / work->paths[i].side_divisor,
                        work->streaks[i].target.vy
                            + work->paths[i].rise_scale * work->streaks[i].elapsed
                                * (work->streaks[i].elapsed - work->paths[i].apex) / work->paths[i].rise_divisor,
                        work->streaks[i].target.vz
                            + (work->paths[i].reach + work->streaks[i].target.vz) * -work->streaks[i].elapsed
                                / (work->streaks[i].duration - 32));
                } else if (work->streaks[i].stage == 0) {
                    /* The mirrored landing point; the shift keeps the
                     * doubled slot as the multiplier. */
                    work->domes[i].active = 1;
                    setVector(&work->domes[i].position, work->trail[0][i].vx - (work->slot << 1) * work->trail[0][i].vx,
                        work->trail[0][i].vy, work->trail[0][i].vz - (work->slot << 1) * work->trail[0][i].vz);
                    for (j = 0; j < work->cols; j++) {
                        work->sprites[j][i].active = 1;
                        setVector(&work->sprites[j][i].position,
                            work->trail[0][i].vx - (work->slot << 1) * work->trail[0][i].vx, work->trail[0][i].vy,
                            work->trail[0][i].vz - (work->slot << 1) * work->trail[0][i].vz);
                    }
                    work->streaks[i].stage = 1;
                }
                for (j = 0; j < 16; j++) {
                    work->line.attribute = 0x50000000;
                    value = 16 - j;
                    work->line.r0 = work->streaks[i].color[0] * value / 16;
                    work->line.g0 = work->streaks[i].color[1] * value / 16;
                    work->line.b0 = work->streaks[i].color[2] * value / 16;
                    value = 15 - j;
                    work->line.r1 = work->streaks[i].color[0] * value / 16;
                    work->line.g1 = work->streaks[i].color[1] * value / 16;
                    work->line.b1 = work->streaks[i].color[2] * value / 16;
                    RotTransPers(&work->trail[j][i], (PSXLONG *)&work->line.x0, &interpolation, &flag);
                    RotTransPers(&work->trail[j + 1][i], (PSXLONG *)&work->line.x1, &interpolation, &flag);
                    setVector(&mid, (work->trail[j][i].vx + work->trail[j + 1][i].vx) / 2,
                        (work->trail[j][i].vy + work->trail[j + 1][i].vy) / 2,
                        (work->trail[j][i].vz + work->trail[j + 1][i].vz) / 2);
                    depth = RotTransPers(&mid, &sxy, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortGLine(&work->line, func_80058F10(), (u16)depth);
                    }
                }
                work->streaks[i].elapsed += step;
                if (work->streaks[i].duration < work->streaks[i].elapsed) {
                    work->streaks[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->rows; i++) {
            if (work->domes[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->domes[i].scale);
                RotMatrixY(work->domes[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->domes[i].position.vx;
                unit.matrix.t[1] = work->domes[i].position.vy;
                unit.matrix.t[2] = work->domes[i].position.vz;
                unit.super = work->domes[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->domes[i].duration / 3 < work->domes[i].elapsed) {
                    /* The tripled remaining time is spelt as a shift and an
                     * add, which keeps the 3 off the table colour. */
                    work->domes[i].color[0] = D_8013CF9C[work->command].color[0]
                        * (((work->domes[i].duration - work->domes[i].elapsed) << 1)
                            + (work->domes[i].duration - work->domes[i].elapsed))
                        / (work->domes[i].duration * 2);
                    work->domes[i].color[1] = D_8013CF9C[work->command].color[1]
                        * (((work->domes[i].duration - work->domes[i].elapsed) << 1)
                            + (work->domes[i].duration - work->domes[i].elapsed))
                        / (work->domes[i].duration * 2);
                    work->domes[i].color[2] = D_8013CF9C[work->command].color[2]
                        * (((work->domes[i].duration - work->domes[i].elapsed) << 1)
                            + (work->domes[i].duration - work->domes[i].elapsed))
                        / (work->domes[i].duration * 2);
                }
                for (j = 0; j < work->domes[i].stage; j++) {
                    for (k = 0; k < 8; k++) {
                        work->line.attribute = 0x50000000;
                        value = 8 - j;
                        work->line.r0 = work->domes[i].color[0] * value / 8;
                        work->line.g0 = work->domes[i].color[1] * value / 8;
                        work->line.b0 = work->domes[i].color[2] * value / 8;
                        value = 7 - j;
                        work->line.r1 = work->domes[i].color[0] * value / 8;
                        work->line.g1 = work->domes[i].color[1] * value / 8;
                        work->line.b1 = work->domes[i].color[2] * value / 8;
                        RotTransPers(&work->dome[k][j], (PSXLONG *)&work->line.x0, &interpolation, &flag);
                        RotTransPers(&work->dome[k][j + 1], (PSXLONG *)&work->line.x1, &interpolation, &flag);
                        /* Retail averages the trail, not the dome, for the depth. */
                        setVector(&mid, (work->trail[k][j].vx + work->trail[k][j + 1].vx) / 2,
                            (work->trail[k][j].vy + work->trail[k][j + 1].vy) / 2,
                            (work->trail[k][j].vz + work->trail[k][j + 1].vz) / 2);
                        depth = RotTransPers(&mid, &sxy, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortGLine(&work->line, func_80058F10(), (u16)depth);
                        }
                    }
                }
                work->domes[i].elapsed += step;
                work->domes[i].stage = work->domes[i].elapsed / 2;
                if (work->domes[i].stage > 8) {
                    work->domes[i].stage = 8;
                }
                /* Written this way round so the elapsed time is loaded first. */
                if (work->domes[i].elapsed > work->domes[i].duration) {
                    work->domes[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->rows; j++) {
            for (i = 0; i < work->cols; i++) {
                if (work->sprites[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->sprites[i][j].scale);
                    RotMatrixX(work->sprites[i][j].rotation.vx - (view.pitch - 4096) * (work->slot * 2 - 1),
                        &unit.matrix);
                    /* Grouped so the 3078 is added to the rotation first. */
                    RotMatrixY(work->sprites[i][j].rotation.vy - (view.yaw - 3078), &unit.matrix);
                    unit.matrix.t[0] = work->sprites[i][j].position.vx
                        + work->sprites[i][j].target.vx * work->sprites[i][j].elapsed / work->sprites[i][j].duration;
                    unit.matrix.t[1] = work->sprites[i][j].position.vy
                        + work->sprites[i][j].target.vy * work->sprites[i][j].elapsed / work->sprites[i][j].duration;
                    unit.matrix.t[2] = work->sprites[i][j].position.vz
                        + work->sprites[i][j].target.vz * work->sprites[i][j].elapsed / work->sprites[i][j].duration;
                    unit.super = work->sprites[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    if (work->sprites[i][j].duration / 2 < work->sprites[i][j].elapsed) {
                        duration = work->sprites[i][j].duration;
                        value = (duration - work->sprites[i][j].elapsed) << 1;
                        work->sprites[i][j].color[0] = D_8013CF9C[work->command].color[3] * value
                            / duration;
                        work->sprites[i][j].color[1] = D_8013CF9C[work->command].color[4] * value
                            / duration;
                        work->sprites[i][j].color[2] = D_8013CF9C[work->command].color[5] * value
                            / duration;
                    }
                    setPolyFT4(&work->sprite);
                    work->sprite.tpage = work->tpage;
                    work->sprite.clut = work->clut;
                    setSemiTrans(&work->sprite, 1);
                    setRGB0(&work->sprite, work->sprites[i][j].color[0], work->sprites[i][j].color[1],
                        work->sprites[i][j].color[2]);
                    setUV4(&work->sprite, work->sprites[i][j].u, work->sprites[i][j].v,
                        work->sprites[i][j].u + 31, work->sprites[i][j].v,
                        work->sprites[i][j].u, work->sprites[i][j].v + 31,
                        work->sprites[i][j].u + 31, work->sprites[i][j].v + 31);
                    depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                        (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                    }
                    work->sprites[i][j].u += 32;
                    if (work->sprites[i][j].u == 96) {
                        work->sprites[i][j].u = 0;
                    }
                    work->sprites[i][j].elapsed += step;
                    if (work->sprites[i][j].duration < work->sprites[i][j].elapsed) {
                        work->sprites[i][j].active = 0;
                    }
                    result = 4;
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->rows * (work->cols + 2)) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}
