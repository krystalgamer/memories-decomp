#include "../../types.h"
#include "variant648_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

/* Header 648's entry: one or two rows of up to 48 elements, each set up from
 * a command row and released in waves; every element spins a cluster of
 * fading sprites outwards from its position, then draws a fan of spoke strips
 * over it, for 32 frames. The setup also prints the work-area size. */

s32 func_8013B004(u8 *context, s32 command)
{
    Model648State *work;
    MATRIX ls, rotation;
    GsCOORDUNIT unit;
    Model648ViewSnapshot view;
    SVECTOR offset, base;
    VECTOR scale;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    /* One temporary carries the side, fade, sprite step, divisor and scale;
     * keeping it shared gives the original register use. */
    s32 i, j, k, n, value;
    s32 grow, red, green, blue;
    u32 tex;
    u8 u;
    u16 duration;
    PSXLONG depth;

    work = (Model648State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model648State));
#ifndef VERSION_FRENCH
        printf("E_DATA size = %x byte\n", sizeof(Model648State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB84[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013CB84[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        switch (command) {
        case 0:
            work->rows = 2;
            work->cols = 48;
            break;
        case 1:
        case 2:
            work->rows = 2;
            work->cols = 48;
            break;
        case 4:
        case 8:
        case 12:
        case 16:
            work->rows = 2;
            work->cols = 4;
            break;
        case 6:
        case 7:
        case 15:
        case 27:
            work->rows = 2;
            work->cols = 48;
            break;
        case 3:
        case 5:
        case 9:
        case 10:
        case 11:
        case 13:
        case 14:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 25:
        case 26:
        case 28:
            work->rows = 1;
            work->cols = 4;
            break;
        case 24:
            work->rows = 1;
            work->cols = 48;
            break;
        }
        work->radius = D_8013CBBC[command].radius;
        work->height = D_8013CBBC[command].height;
        for (i = 0; i < work->rows; i++) {
            for (j = 0; j < work->cols; j++) {
                work->bursts[i][j].active = 0;
                work->bursts[i][j].elapsed = 0;
                work->bursts[i][j].duration = 32;
                work->bursts[i][j].unknown_0C = 1;
                work->bursts[i][j].coord = Model_GetCameraCoordinateUnit();
                work->bursts[i][j].color[0] = D_8013CBBC[command].color[0];
                work->bursts[i][j].color[1] = D_8013CBBC[command].color[1];
                work->bursts[i][j].color[2] = D_8013CBBC[command].color[2];
                work->bursts[i][j].color[3] = D_8013CBBC[command].color[3];
                work->bursts[i][j].color[4] = D_8013CBBC[command].color[4];
                work->bursts[i][j].color[5] = D_8013CBBC[command].color[5];
                work->bursts[i][j].u = 32;
                work->bursts[i][j].v = 0;
                work->bursts[i][j].unknown_14 = 1;
                setVector(&work->bursts[i][j].scale, 4096, 4096, 4096);
                setVector(&work->bursts[i][j].unknown_44, 4096, 4096, 4096);
                value = (j % 2) * 2 - 1;
                setVector(&work->bursts[i][j].rotation, value * (j * 50 + i * 400 + 200) * 100,
                    value * (j * 160 + i * 300 + 150) * 150 + work->slot * 2048,
                    value * -(j * 270 + i * 200 + 100) * 200);
                switch (command) {
                case 0:
                case 7:
                    setVector(&work->bursts[i][j].position, i * 200 - 100, 0, 0);
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                    setVector(&work->bursts[i][j].position, 0, 0, 0);
                    break;
                case 18:
                case 28:
                    setVector(&work->bursts[i][j].position, 0, 50, 0);
                    break;
                }
                work->bursts[i][j].position.vy -= 350;
                work->bursts[i][j].position.vz += work->direction * 450;
            }
        }
        work->delay = D_8013CBBC[command].delay;
        for (j = 0; j < 5; j++) {
            work->flags[j] = 0;
        }
        setVector(&work->corners[0], -work->radius / 2, -work->radius / 2, 0);
        setVector(&work->corners[1], work->radius / 2, -work->radius / 2, 0);
        setVector(&work->corners[2], -work->radius / 2, work->radius / 2, 0);
        setVector(&work->corners[3], work->radius / 2, work->radius / 2, 0);
        for (j = 0; j < 4; j++) {
            setVector(&work->ring[j], work->radius * 6 * rsin(j << 10) / 4096, 0,
                work->radius * 6 * rcos(j << 10) / 4096);
        }
        for (j = 0; j < 17; j++) {
            setVector(&work->spokes[j][0], work->radius * 3 * rsin(j << 8) / 4096, 0,
                work->radius * 3 * rcos(j << 8) / 4096);
            setVector(&work->spokes[j][1], work->radius * 6 * rsin(j << 8) / 4096, 0,
                work->radius * 6 * rcos(j << 8) / 4096);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed < 88 && work->elapsed >= work->flags[0] * 8 && work->flags[0] < work->cols) {
                for (j = 0; j < work->rows; j++) {
                    work->bursts[j][work->flags[0]].active = 1;
                }
                work->flags[0]++;
            }
            break;
        case 1:
            if (work->elapsed < 50 && work->elapsed >= work->flags[0] * 8 && work->flags[0] < work->cols) {
                for (j = 0; j < work->rows; j++) {
                    work->bursts[j][work->flags[0]].active = 1;
                }
                work->flags[0]++;
            }
            break;
        case 2:
            if (work->elapsed < 124 && work->elapsed >= work->flags[0] * 8 && work->flags[0] < work->cols) {
                for (j = 0; j < work->rows; j++) {
                    work->bursts[j][work->flags[0]].active = 1;
                }
                work->flags[0]++;
            }
            break;
        case 3:
        case 5:
        case 9:
        case 10:
        case 11:
        case 13:
        case 14:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 25:
        case 26:
        case 28:
            if (work->elapsed == 0) {
                for (j = 0; j < work->rows; j++) {
                    for (i = 0; i < work->cols; i++) {
                        work->bursts[j][i].active = 1;
                    }
                }
            }
            break;
        case 4:
            if (work->elapsed == 0) {
                for (i = 0; i < work->cols; i++) {
                    work->bursts[0][i].active = 1;
                }
            }
            if (work->flags[0] == 0 && work->elapsed > 20) {
                for (i = 0; i < work->cols; i++) {
                    work->bursts[1][i].active = 1;
                }
                work->flags[0] = 1;
            }
            break;
        case 6:
            if (work->elapsed < 48 && work->elapsed >= work->flags[0] * 8 && work->flags[0] < work->cols) {
                for (j = 0; j < work->rows; j++) {
                    work->bursts[j][work->flags[0]].active = 1;
                }
                work->flags[0]++;
            }
            break;
        case 7:
            if (work->elapsed < 36 && work->elapsed >= work->flags[0] * 8 && work->flags[0] < work->cols) {
                for (j = 0; j < work->rows; j++) {
                    work->bursts[j][work->flags[0]].active = 1;
                }
                work->flags[0]++;
            }
            break;
        case 8:
        case 12:
            if (work->elapsed == 0) {
                for (i = 0; i < work->cols; i++) {
                    work->bursts[0][i].active = 1;
                }
            }
            if (work->flags[0] == 0 && work->elapsed > 12) {
                for (i = 0; i < work->cols; i++) {
                    work->bursts[1][i].active = 1;
                }
                work->flags[0] = 1;
            }
            break;
        case 15:
            if (work->elapsed < 56 && work->elapsed >= work->flags[0] * 8 && work->flags[0] < work->cols) {
                for (j = 0; j < work->rows; j++) {
                    work->bursts[j][work->flags[0]].active = 1;
                }
                work->flags[0]++;
            }
            break;
        case 16:
            if (work->elapsed == 0) {
                for (i = 0; i < work->cols; i++) {
                    work->bursts[0][i].active = 1;
                }
            }
            if (work->flags[0] == 0 && work->elapsed > 16) {
                for (i = 0; i < work->cols; i++) {
                    work->bursts[1][i].active = 1;
                }
                work->flags[0] = 1;
            }
            break;
        case 24:
            if (work->elapsed < 36 && work->elapsed >= work->flags[0] * 8 && work->flags[0] < work->cols) {
                for (j = 0; j < work->rows; j++) {
                    work->bursts[j][work->flags[0]].active = 1;
                }
                work->flags[0]++;
            }
            Model_SetFrameStepOverride(1);
            break;
        case 27:
            if (work->elapsed < 24 && work->elapsed >= work->flags[0] * 8 && work->flags[0] < work->cols) {
                for (j = 0; j < work->rows; j++) {
                    work->bursts[j][work->flags[0]].active = 1;
                }
                work->flags[0]++;
            }
            break;
        }
        PushMatrix();
        view = *(Model648ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->rows; i++) {
            for (j = 0; j < work->cols; j++) {
                if (work->bursts[i][j].active == 1) {
                    duration = work->bursts[i][j].duration;
                    if (duration / 2 < work->bursts[i][j].elapsed) {
                        value = (duration - work->bursts[i][j].elapsed) * 2;
                        work->bursts[i][j].color[0] = D_8013CBBC[work->command].color[0] * value / duration;
                        work->bursts[i][j].color[1] = D_8013CBBC[work->command].color[1] * value / duration;
                        work->bursts[i][j].color[2] = D_8013CBBC[work->command].color[2] * value / duration;
                        work->bursts[i][j].color[3] = D_8013CBBC[work->command].color[3] * value / duration;
                        work->bursts[i][j].color[4] = D_8013CBBC[work->command].color[4] * value / duration;
                        work->bursts[i][j].color[5] = D_8013CBBC[work->command].color[5] * value / duration;
                    }
                    grow = (work->bursts[i][j].elapsed << 12) / work->bursts[i][j].duration + 0xAAA;
                    RotMatrix(&work->bursts[i][j].rotation, &rotation);
                    for (k = 0; k < 4; k++) {
                        PushMatrix();
                        ApplyMatrixSV(&rotation, &work->ring[k], &base);
                        PopMatrix();
                        for (n = 0; n < 4; n++) {
                            offset = base;
                            value = n + 1;
                            offset.vx *= value;
                            offset.vy *= value;
                            offset.vz *= value;
                            value = 5;
                            offset.vx /= value;
                            offset.vy /= value;
                            offset.vz /= value;
                            unit.flg = 0;
                            unit.matrix = D_800FE168;
                            value = grow * (n + value) / 9;
                            setVector(&work->bursts[i][j].scale, value, value, value);
                            ScaleMatrix(&unit.matrix, &work->bursts[i][j].scale);
                            RotMatrixZ(0, &unit.matrix);
                            RotMatrixX(-((view.pitch - 4096) * work->direction), &unit.matrix);
                            RotMatrixY(work->slot * 2048 + (0xC06 - view.yaw), &unit.matrix);
                            unit.matrix.t[0] = work->bursts[i][j].position.vx
                                + offset.vx * work->bursts[i][j].elapsed / work->bursts[i][j].duration;
                            unit.matrix.t[1] = work->bursts[i][j].position.vy
                                + offset.vy * work->bursts[i][j].elapsed / work->bursts[i][j].duration;
                            unit.matrix.t[2] = work->bursts[i][j].position.vz
                                + offset.vz * work->bursts[i][j].elapsed / work->bursts[i][j].duration;
                            unit.super = work->bursts[i][j].coord;
                            GsGetLsUnit(&unit, &ls);
                            GsSetLsMatrix(&ls);
                            setPolyFT4(&work->sprite);
                            work->sprite.tpage = work->tpage[1];
                            work->sprite.clut = work->clut[1];
                            setSemiTrans(&work->sprite, 1);
                            u = work->bursts[i][j].u + n * 32;
                            if (u >= 0xE0) {
                                u += 32;
                            }
                            if (u == 0) {
                                u = 0xC0;
                            }
                            setUV4(&work->sprite, u, work->bursts[i][j].v, u + 31, work->bursts[i][j].v,
                                u, work->bursts[i][j].v + 31, u + 31, work->bursts[i][j].v + 31);
                            value = n + 1;
                            red = work->bursts[i][j].color[0] * value / 4;
                            green = work->bursts[i][j].color[1] * value / 4;
                            blue = work->bursts[i][j].color[2] * value / 4;
                            setRGB0(&work->sprite, red, green, blue);
                            depth = RotAverage4(&work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3],
                                (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                                (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3,
                                &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    value = (work->bursts[i][j].elapsed << 12) / work->bursts[i][j].duration;
                    setVector(&scale, value, value, value);
                    ScaleMatrix(&unit.matrix, &scale);
                    RotMatrixZ(work->bursts[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->bursts[i][j].rotation.vx, &unit.matrix);
                    RotMatrixY(work->bursts[i][j].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->bursts[i][j].position.vx;
                    unit.matrix.t[1] = work->bursts[i][j].position.vy;
                    unit.matrix.t[2] = work->bursts[i][j].position.vz;
                    unit.super = work->bursts[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (k = 0; k < 16; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[0];
                        work->strip.clut = work->clut[0];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, 0, work->bursts[i][j].v + 32, 31, work->bursts[i][j].v + 32,
                            0, work->bursts[i][j].v + 159, 31, work->bursts[i][j].v + 159);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        setRGB2(&work->strip, work->bursts[i][j].color[3], work->bursts[i][j].color[4], work->bursts[i][j].color[5]);
                        setRGB3(&work->strip, work->bursts[i][j].color[3], work->bursts[i][j].color[4], work->bursts[i][j].color[5]);
                        depth = RotAverage4(&work->spokes[k][0], &work->spokes[k + 1][0],
                            &work->spokes[k][1], &work->spokes[k + 1][1],
                            (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                            (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                        }
                    }
                    work->bursts[i][j].u += 32;
                    if (work->bursts[i][j].u == 0xE0) {
                        work->bursts[i][j].u = 32;
                    }
                    work->bursts[i][j].elapsed += step;
                    if (work->bursts[i][j].duration < work->bursts[i][j].elapsed) {
                        work->bursts[i][j].active = 0;
                    }
                    result = 4;
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->rows * work->cols) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}
