#include "../../types.h"
#include "variant628_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 628's entry: a sprite ball arcs away from a model joint while a
 * ring of sparks bursts out of it. Where the ball lands, two wobbling
 * Gouraud domes rise and shed a trail of spinning ring bursts. The setup
 * also prints the work-area size. */

/* The work-size format string; the padding byte after the terminator
 * is 0x26 in slot zero and 0x00 in slot one. */
#ifndef MODEL628_STRING_PAD
#define MODEL628_STRING_PAD "&"
#endif
#ifndef VERSION_FRENCH
static const char D_8013D580[24] = "E_DATA size = %x byte\n\0" MODEL628_STRING_PAD;
#endif

s32 func_8013B004(u8 *context, s32 command)
{
    Model628State *work;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    Model628ViewSnapshot view;
    SVECTOR rotation;
    SVECTOR mid[2];
    VECTOR vec;
    PSXLONG interpolation, flag;
    s32 result, i, j, step, inactive;
    s32 k, l, fade, n, shade, h;
    u8 color;
    u32 tex;
    PSXLONG depth;

    work = (Model628State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model628State));
#ifndef VERSION_FRENCH
        printf(D_8013D580, sizeof(Model628State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D598[1]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D598[0]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013D598[2]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        switch (command) {
        case 0:
            work->count = 1;
            work->spark_count = 8;
            work->dome_count = 2;
            work->burst_count = 32;
            break;
        }
        work->radius = D_8013D5EC[command].radius;
        work->height = D_8013D5EC[command].height;
        for (i = 0; i < work->count; i++) {
            work->balls[i].active = 0;
            work->balls[i].elapsed = 0;
            work->balls[i].duration = 0x20;
            work->balls[i].mode = 1;
            work->balls[i].coord = Model_GetCameraCoordinateUnit();
            work->balls[i].red = D_8013D5EC[command].red;
            work->balls[i].green = D_8013D5EC[command].green;
            work->balls[i].blue = D_8013D5EC[command].blue;
            work->balls[i].u = 0;
            work->balls[i].v = 0x60;
            setVector(&work->balls[i].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->balls[i].rotation, 0, work->slot * 2048, 0);
            switch (command) {
            case 0:
                setVector(&work->balls[i].velocity, -20, 2, 7);
                if (i == 0) {
                    setVector(&work->balls[i].position, 0, 0, 0);
                }
                break;
            }
        }
        PushMatrix();
        for (i = 0; i < work->spark_count; i++) {
            work->sparks[i][0].active = 0;
            work->sparks[i][0].elapsed = 0;
            work->sparks[i][0].duration = 0x40;
            work->sparks[i][0].mode = 1;
            work->sparks[i][0].coord = Model_GetCameraCoordinateUnit();
            work->sparks[i][0].red = 0x80;
            work->sparks[i][0].green = 0x80;
            work->sparks[i][0].blue = 0x80;
            work->sparks[i][0].u = 0;
            work->sparks[i][0].v = 0;
            setVector(&work->sparks[i][0].scale, 0x1000, 0x1000, 0x1000);
            setVector(&work->sparks[i][0].rotation, 0, work->slot * 2048, 0);
            setVector(&rotation, (work->slot * 2 - 1) * 0x28000 / 360, 0, 0);
            setVector(&vec, rsin((i << 12) / work->spark_count) * 10 / 4096,
                rcos((i << 12) / work->spark_count) * 10 / 4096, -((work->slot * 2 - 1) * 10));
            RotMatrix(&rotation, &m);
            ApplyMatrixLV(&m, &vec, &work->sparks[i][0].velocity);
            switch (command) {
            case 0:
                if (i == 0) {
                    setVector(&work->sparks[i][0].position, 0, 0, 0);
                }
                break;
            }
        }
        PopMatrix();
        for (i = 0; i < work->dome_count; i++) {
            work->domes[i][0].active = 0;
            work->domes[i][0].elapsed = 0;
            work->domes[i][0].duration = 0x80;
            work->domes[i][0].mode = 1;
            work->domes[i][0].coord = Model_GetCameraCoordinateUnit();
            work->domes[i][0].red = D_8013D5EC[command].red;
            work->domes[i][0].green = D_8013D5EC[command].green;
            work->domes[i][0].blue = D_8013D5EC[command].blue;
            work->domes[i][0].u = 0;
            work->domes[i][0].v = 0x20;
            work->domes[i][0].stage = 0;
            setVector(&work->domes[i][0].rotation, 0, work->slot * 2048, 0);
            setVector(&work->domes[i][0].velocity, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->domes[i][0].position, 0, 0, 0);
                switch (i) {
                case 0:
                    setVector(&work->domes[i][0].scale, 0x1000, 0, 0x1000);
                    break;
                case 1:
                    setVector(&work->domes[i][0].scale, 0x1800, 0, 0x1800);
                    break;
                }
                break;
            }
        }
        for (i = 0; i < work->burst_count; i++) {
            work->bursts[i][0].active = 0;
            work->bursts[i][0].elapsed = 0;
            work->bursts[i][0].duration = 0x30;
            work->bursts[i][0].mode = 1;
            work->bursts[i][0].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i][0].red = D_8013D5EC[command].red;
            work->bursts[i][0].green = D_8013D5EC[command].green;
            work->bursts[i][0].blue = D_8013D5EC[command].blue;
            work->bursts[i][0].u = 0;
            work->bursts[i][0].v = 0x20;
            setVector(&work->bursts[i][0].rotation, 0, work->slot * 2048, 0);
            setVector(&work->bursts[i][0].velocity, 0, 0, 0);
            switch (command) {
            case 0:
                setVector(&work->bursts[i][0].position, 0, 0, 0);
                setVector(&work->bursts[i][0].scale, 0x1000, 0x1000, 0x1000);
                break;
            }
        }
        work->delay = D_8013D5EC[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        setVector(&work->square[0], -work->radius / 2, -work->radius / 2, 0);
        setVector(&work->square[1], work->radius / 2, -work->radius / 2, 0);
        setVector(&work->square[2], -work->radius / 2, work->radius / 2, 0);
        setVector(&work->square[3], work->radius / 2, work->radius / 2, 0);
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 17; j++) {
                setVector(&work->dome[j][i],
                    (work->radius * 2 - work->radius * i / 8) * rsin(j << 8) / 4096,
                    /* One read of the height feeds both terms. */
                    (h = work->height, -h + h * i / 8),
                    (work->radius * 2 - work->radius * i / 8) * rcos(j << 8) / 4096);
            }
        }
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i].inner, work->radius * rsin(i << 8) / 4096, 0,
                work->radius * rcos(i << 8) / 4096);
            setVector(&work->ring[i].outer, ((work->radius * 3) >> 1) * rsin((i << 8) + 0xE3) / 4096, -300,
                ((work->radius * 3) >> 1) * rcos((i << 8) + 0xE3) / 4096);
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed != 0) {
                break;
            }
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, 0xC);
            GsGetLwUnit(&unit, &ls);
            work->balls[0].active = 1;
            work->balls[0].position.vx += ls.t[0];
            work->balls[0].position.vy += ls.t[1];
            work->balls[0].position.vz += ls.t[2] * -(work->slot * 2 + -1);
            for (i = 0; i < work->spark_count; i++) {
                work->sparks[i][0].active = 1;
                work->sparks[i][0].position.vx += ls.t[0];
                work->sparks[i][0].position.vy += ls.t[1];
                work->sparks[i][0].position.vz += ls.t[2];
            }
            break;
        }
        PushMatrix();
        view = *(Model628ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->balls[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                ScaleMatrix(&unit.matrix, &work->balls[i].scale);
                RotMatrixX(work->balls[i].rotation.vx - (*(s16 *)&view.bytes[4] - 0x1000) * (work->slot * 2 - 1),
                    &unit.matrix);
                RotMatrixY(work->balls[i].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                unit.matrix.t[0] = work->balls[i].position.vx;
                unit.matrix.t[1] = work->balls[i].velocity.vx * -work->balls[i].velocity.vz
                        * work->balls[i].elapsed * work->balls[i].elapsed
                        / (work->balls[i].duration * (work->balls[i].velocity.vy << 1))
                    + work->balls[i].velocity.vx * work->balls[i].elapsed + work->balls[i].position.vy;
                unit.matrix.t[2] = ((work->balls[i].position.vz + 450) * work->balls[i].elapsed
                        / work->balls[i].duration - work->balls[i].position.vz) * (work->slot * 2 - 1);
                unit.super = work->balls[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                setPolyFT4(&work->flat);
                setSemiTrans(&work->flat, 1);
                work->flat.tpage = work->tpage[0];
                work->flat.clut = work->clut[0];
                setUV4(&work->flat, work->balls[i].u, work->balls[i].v, work->balls[i].u + 31,
                    work->balls[i].v, work->balls[i].u, work->balls[i].v + 31,
                    work->balls[i].u + 31, work->balls[i].v + 31);
                setRGB0(&work->flat, work->balls[i].red, work->balls[i].green, work->balls[i].blue);
                depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                    (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                    (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                if (flag >= 0) {
                    GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                }
                work->balls[i].u += 0x20;
                if (work->balls[i].u == 0x60) {
                    work->balls[i].u = 0;
                }
                work->balls[i].elapsed += step;
                if (work->balls[i].duration < work->balls[i].elapsed) {
                    work->balls[i].active = 0;
                    for (j = 0; j < work->dome_count; j++) {
                        work->domes[j][i].active = 1;
                        work->domes[j][i].position.vx = unit.matrix.t[0];
                        work->domes[j][i].position.vy = unit.matrix.t[1];
                        work->domes[j][i].position.vz = unit.matrix.t[2];
                    }
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < work->spark_count; i++) {
                if (work->sparks[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->sparks[i][j].scale);
                    RotMatrixX(work->sparks[i][j].rotation.vx
                            - (*(s16 *)&view.bytes[4] - 0x1000) * (work->slot * 2 - 1),
                        &unit.matrix);
                    RotMatrixY(work->sparks[i][j].rotation.vy + (0xC06 - *(s16 *)&view.bytes[2]), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][j].position.vx;
                    unit.matrix.t[1] = work->sparks[i][j].position.vy;
                    unit.matrix.t[2] = work->sparks[i][j].position.vz;
                    unit.super = work->sparks[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    work->sparks[i][j].position.vx += work->sparks[i][j].velocity.vx;
                    work->sparks[i][j].position.vy += work->sparks[i][j].velocity.vy;
                    work->sparks[i][j].position.vz += work->sparks[i][j].velocity.vz;
                    if (work->sparks[i][j].duration / 2 < work->sparks[i][j].elapsed) {
                        shade = ((work->sparks[i][j].duration - work->sparks[i][j].elapsed) << 8)
                            / work->sparks[i][j].duration;
                        work->sparks[i][j].red = shade;
                        work->sparks[i][j].green = shade;
                        work->sparks[i][j].blue = shade;
                    }
                    setPolyFT4(&work->flat);
                    setSemiTrans(&work->flat, 1);
                    work->flat.tpage = work->tpage[1];
                    work->flat.clut = work->clut[1];
                    setUV4(&work->flat, work->sparks[i][j].u, work->sparks[i][j].v, work->sparks[i][j].u + 31,
                        work->sparks[i][j].v, work->sparks[i][j].u, work->sparks[i][j].v + 31,
                        work->sparks[i][j].u + 31, work->sparks[i][j].v + 31);
                    setRGB0(&work->flat, work->sparks[i][j].red, work->sparks[i][j].green,
                        work->sparks[i][j].blue);
                    depth = RotAverage4(&work->square[0], &work->square[1], &work->square[2], &work->square[3],
                        (PSXLONG *)&work->flat.x0, (PSXLONG *)&work->flat.x1,
                        (PSXLONG *)&work->flat.x2, (PSXLONG *)&work->flat.x3, &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->flat, func_80058F10(), (u16)depth);
                    }
                    work->sparks[i][j].u += 0x20;
                    if (work->sparks[i][j].u == 0xE0) {
                        work->sparks[i][j].u = 0;
                    }
                    work->sparks[i][j].elapsed += step;
                    if (work->sparks[i][j].duration < work->sparks[i][j].elapsed) {
                        work->sparks[i][j].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < work->dome_count; i++) {
                /* Holding the flag in n, which later takes a third of the
                 * duration, pins that value in a saved register. */
                n = work->domes[i][j].active;
                if (n == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->domes[i][j].scale);
                    RotMatrixY(work->domes[i][j].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->domes[i][j].position.vx;
                    unit.matrix.t[1] = work->domes[i][j].position.vy;
                    unit.matrix.t[2] = work->domes[i][j].position.vz;
                    unit.super = work->domes[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    switch (i) {
                    case 0:
                        if (work->domes[i][j].elapsed < 20) {
                            work->domes[i][j].scale.vy += step * 0xCC;
                            if (work->domes[i][j].scale.vy > 0x1000) {
                                work->domes[i][j].scale.vy = 0x1000;
                            }
                        } else {
                            work->domes[i][j].scale.vy = rsin(work->domes[i][j].stage * 1000 + 0x800) / 32 + 0x1000;
                            work->domes[i][j].stage += step;
                        }
                        break;
                    case 1:
                        if (work->domes[i][j].elapsed < 20) {
                            work->domes[i][j].scale.vy += step * 0x88;
                            if (work->domes[i][j].scale.vy > 0xAAA) {
                                work->domes[i][j].scale.vy = 0xAAA;
                            }
                        } else {
                            work->domes[i][j].scale.vy = rsin(work->domes[i][j].stage * 1000) / 32 + 0xAAA;
                            work->domes[i][j].stage += step;
                        }
                        break;
                    }
                    n = work->domes[i][j].duration / 3;
                    if (work->domes[i][j].duration * 2 / 3 < work->domes[i][j].elapsed) {
                        fade = D_8013D5EC[work->command].red / (n - 1) * step;
                        if (fade < work->domes[i][j].red) {
                            work->domes[i][j].red -= fade;
                        } else {
                            work->domes[i][j].red = 0;
                        }
                        fade = D_8013D5EC[work->command].green / (n - 1) * step;
                        if (fade < work->domes[i][j].green) {
                            work->domes[i][j].green -= fade;
                        } else {
                            work->domes[i][j].green = 0;
                        }
                        fade = D_8013D5EC[work->command].blue / (n - 1) * step;
                        if (fade < work->domes[i][j].blue) {
                            work->domes[i][j].blue -= fade;
                        } else {
                            work->domes[i][j].blue = 0;
                        }
                    }
                    PushMatrix();
                    for (k = 0; k < 9; k++) {
                        m = D_800FE168;
                        RotMatrixY(rsin(work->domes[i][j].elapsed * 80 + k * 500) / 15, &m);
                        for (l = 0; l < 17; l++) {
                            ApplyMatrixSV(&m, &work->dome[l][k], &work->spun[l][k]);
                        }
                    }
                    PopMatrix();
                    for (k = 0; k < 8; k++) {
                        for (l = 0; l < 16; l++) {
                            setPolyGT4(&work->shaded);
                            setSemiTrans(&work->shaded, 1);
                            work->shaded.tpage = work->tpage[2];
                            work->shaded.clut = work->clut[2];
                            setUV4(&work->shaded, work->domes[i][j].u, work->domes[i][j].v + k * 2,
                                work->domes[i][j].u + 63, work->domes[i][j].v + k * 2,
                                work->domes[i][j].u, work->domes[i][j].v + (k + 1) * 2,
                                work->domes[i][j].u + 63, work->domes[i][j].v + (k + 1) * 2);
                            setRGB0(&work->shaded, work->domes[i][j].red / (9 - k),
                                work->domes[i][j].green / (9 - k), work->domes[i][j].blue / (9 - k));
                            setRGB1(&work->shaded, work->domes[i][j].red / (9 - k),
                                work->domes[i][j].green / (9 - k), work->domes[i][j].blue / (9 - k));
                            if (k != 7) {
                                setRGB2(&work->shaded, work->domes[i][j].red / (8 - k),
                                    work->domes[i][j].green / (8 - k), work->domes[i][j].blue / (8 - k));
                                setRGB3(&work->shaded, work->domes[i][j].red / (8 - k),
                                    work->domes[i][j].green / (8 - k), work->domes[i][j].blue / (8 - k));
                            } else {
                                setRGB2(&work->shaded, 0, 0, 0);
                                setRGB3(&work->shaded, 0, 0, 0);
                            }
                            depth = RotAverage4(&work->spun[l][k], &work->spun[l + 1][k],
                                &work->spun[l][k + 1], &work->spun[l + 1][k + 1],
                                (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                                (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                            if (flag >= 0) {
                                GsSortPoly(&work->shaded, func_80058F10(), (u16)depth);
                            }
                        }
                    }
                    if (work->flags[0] < work->burst_count && work->flags[0] * 20 < work->elapsed
                        && work->domes[i][j].elapsed < work->domes[i][j].duration * 4 / 5) {
                        work->bursts[work->flags[0]][j].active = 1;
                        work->bursts[work->flags[0]][j].position = work->domes[i][j].position;
                        work->flags[0]++;
                    }
                    work->domes[i][j].u += 0x40;
                    if (work->domes[i][j].u == 0xC0) {
                        work->domes[i][j].u = 0;
                    }
                    work->domes[i][j].elapsed += step;
                    if (work->domes[i][j].duration < work->domes[i][j].elapsed) {
                        work->domes[i][j].active = 0;
                        if (work->flags[4] == 0) {
                            work->flags[4] = 1;
                        }
                    }
                    if (work->flags[4] == 0) {
                        result = 4;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < work->burst_count; i++) {
                if (work->bursts[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->bursts[i][j].scale);
                    RotMatrixY(work->bursts[i][j].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->bursts[i][j].position.vx;
                    unit.matrix.t[1] = work->bursts[i][j].position.vy;
                    unit.matrix.t[2] = work->bursts[i][j].position.vz;
                    unit.super = work->bursts[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    work->bursts[i][j].rotation.vy -= step * 1500;
                    work->bursts[i][j].scale.vx += 0x5000 / work->bursts[i][j].duration * step;
                    work->bursts[i][j].scale.vz += 0x5000 / work->bursts[i][j].duration * step;
                    if (work->bursts[i][j].duration / 2 < work->bursts[i][j].elapsed) {
                        /* Reading each colour before its fade step keeps the
                         * burst address ahead of the table lookup. */
                        color = work->bursts[i][j].red;
                        fade = D_8013D5EC[work->command].red / (work->bursts[i][j].duration / 2 - 1) * step;
                        if (fade < color) {
                            work->bursts[i][j].red -= fade;
                        } else {
                            work->bursts[i][j].red = 0;
                        }
                        color = work->bursts[i][j].green;
                        fade = D_8013D5EC[work->command].green / (work->bursts[i][j].duration / 2 - 1) * step;
                        if (fade < color) {
                            work->bursts[i][j].green -= fade;
                        } else {
                            work->bursts[i][j].green = 0;
                        }
                        color = work->bursts[i][j].blue;
                        fade = D_8013D5EC[work->command].blue / (work->bursts[i][j].duration / 2 - 1) * step;
                        if (fade < color) {
                            work->bursts[i][j].blue -= fade;
                        } else {
                            work->bursts[i][j].blue = 0;
                        }
                    }
                    for (l = 0; l < 16; l++) {
                        for (k = 0; k < 2; k++) {
                            mid[k].vx = (work->ring[l + k].inner.vx + work->ring[l + k].outer.vx) / 2;
                            mid[k].vy = (work->ring[l + k].inner.vy + work->ring[l + k].outer.vy) / 2;
                            mid[k].vz = (work->ring[l + k].inner.vz + work->ring[l + k].outer.vz) / 2;
                        }
                        setPolyGT4(&work->shaded);
                        setSemiTrans(&work->shaded, 1);
                        work->shaded.tpage = work->tpage[2];
                        work->shaded.clut = work->clut[2];
                        setUV4(&work->shaded, work->bursts[i][j].u + 0x30, work->bursts[i][j].v + 0x10,
                            work->bursts[i][j].u + 0x10, work->bursts[i][j].v + 0x10,
                            work->bursts[i][j].u + 0x30, work->bursts[i][j].v + 8,
                            work->bursts[i][j].u + 0x10, work->bursts[i][j].v + 8);
                        setRGB2(&work->shaded, work->bursts[i][j].red, work->bursts[i][j].green,
                            work->bursts[i][j].blue);
                        setRGB3(&work->shaded, work->bursts[i][j].red, work->bursts[i][j].green,
                            work->bursts[i][j].blue);
                        setRGB0(&work->shaded, 0, 0, 0);
                        setRGB1(&work->shaded, 0, 0, 0);
                        depth = RotAverage4(&work->ring[l].inner, &work->ring[l + 1].inner, &mid[0], &mid[1],
                            (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                            (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->shaded, func_80058F10(), (u16)depth);
                        }
                        setPolyGT4(&work->shaded);
                        setSemiTrans(&work->shaded, 1);
                        work->shaded.tpage = work->tpage[2];
                        work->shaded.clut = work->clut[2];
                        setUV4(&work->shaded, work->bursts[i][j].u + 0x30, work->bursts[i][j].v + 8,
                            work->bursts[i][j].u + 0x10, work->bursts[i][j].v + 8,
                            work->bursts[i][j].u + 0x30, work->bursts[i][j].v,
                            work->bursts[i][j].u + 0x10, work->bursts[i][j].v);
                        setRGB0(&work->shaded, work->bursts[i][j].red, work->bursts[i][j].green,
                            work->bursts[i][j].blue);
                        setRGB1(&work->shaded, work->bursts[i][j].red, work->bursts[i][j].green,
                            work->bursts[i][j].blue);
                        setRGB2(&work->shaded, 0, 0, 0);
                        setRGB3(&work->shaded, 0, 0, 0);
                        depth = RotAverage4(&mid[0], &mid[1], &work->ring[l].outer, &work->ring[l + 1].outer,
                            (PSXLONG *)&work->shaded.x0, (PSXLONG *)&work->shaded.x1,
                            (PSXLONG *)&work->shaded.x2, (PSXLONG *)&work->shaded.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->shaded, func_80058F10(), (u16)depth);
                        }
                    }
                    work->bursts[i][j].u += 0x40;
                    if (work->bursts[i][j].u == 0xC0) {
                        work->bursts[i][j].u = 0;
                    }
                    work->bursts[i][j].elapsed += step;
                    if (work->bursts[i][j].duration < work->bursts[i][j].elapsed) {
                        work->bursts[i][j].active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        if (work->flags[4] == 1) {
            result = 1;
            work->flags[4] = 2;
        }
        work->elapsed += step;
        if (inactive == work->count * (1 + work->spark_count + work->dome_count + work->burst_count)) {
            result = 2;
        }
    }
    return result;
}
