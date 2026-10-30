#include "../../types.h"
#include "variant680us_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 680's entry: blooms of shaded petals open at model joints and
 * release rings that fly out along a cone, bursts that flash and shoot
 * five spokes, and rows of sparks shed from further joints. The setup
 * also prints the work-area size. */

/* Fades COLOR[0..2] of element E from the command colour at OFFSET. */
#define MODEL680US_FADE(e, offset, value, divisor)                                     \
    (e).color[0] = D_8013DBF8[work->command].color[(offset) + 0] * (value) / (divisor); \
    (e).color[1] = D_8013DBF8[work->command].color[(offset) + 1] * (value) / (divisor); \
    (e).color[2] = D_8013DBF8[work->command].color[(offset) + 2] * (value) / (divisor)

/* Loads the camera-facing rotation of element E into UNIT. */
#define MODEL680US_FACE(e)                                                              \
    ScaleMatrix(&unit.matrix, &(e).scale);                                              \
    RotMatrixZ((e).rotation.vz, &unit.matrix);                                          \
    RotMatrixX((e).rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix); \
    RotMatrixY((e).rotation.vy + (0xC06 - view.yaw), &unit.matrix)

/* Sets a semi-transparent textured quad on page PAGE with UVs from U,V
 * of size W,H. */
#define MODEL680US_QUAD(page, u, v, w, h)                                  \
    setPolyFT4(&work->quad);                                               \
    work->quad.tpage = work->tpage[page];                                  \
    work->quad.clut = work->clut[page];                                    \
    setSemiTrans(&work->quad, 1);                                          \
    setUV4(&work->quad, (u), (v), (u) + (w), (v), (u), (v) + (h), (u) + (w), (v) + (h))

#define MODEL680US_SORT_QUAD(s)                                                         \
    depth = RotAverage4(&(s)[0], &(s)[1], &(s)[2], &(s)[3],                            \
        (PSXLONG *)&work->quad.x0, (PSXLONG *)&work->quad.x1,                         \
        (PSXLONG *)&work->quad.x2, (PSXLONG *)&work->quad.x3,                         \
        &interpolation, &flag);                                                         \
    if (flag >= 0) {                                                                    \
        GsSortPoly(&work->quad, func_80058F10(), (u16)depth);                           \
    }

s32 func_8013B004(u8 *context, s32 command)
{
    Model680UsState *work;
    MATRIX ls;
    MATRIX rotation;
    GsCOORDUNIT unit;
    Model680UsViewSnapshot view;
    u8 red[2], green[2], blue[2], row[2];
    PSXLONG interpolation, flag;
    VECTOR offset;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 value, sign, angle;
    s32 turn, tilt;
    u32 tex;
    s16 corner;
    PSXLONG depth;

    work = (Model680UsState *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model680UsState));
#ifndef VERSION_FRENCH
        printf(D_8013DAAC, sizeof(Model680UsState));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        switch (command) {
        case 0:
            work->joints[1] = 1;
            work->joints[2] = 2;
            work->joints[0] = 0;
            work->joints[3] = 3;
            break;
        }
        for (i = 0; i < 8; i++) {
            work->frame_u[i] = (i % 4) << 6;
            work->frame_v[i] = (i / 4 << 6) - 128;
        }
        /* Scheduling barrier: the empty loop after the early corner
         * assignment keeps the D_8013DAC4 high half ahead of the first
         * call's constant argument.  The assignment itself is sunk into
         * its single use before register allocation. */
        corner = 0;
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
        switch (command) {
        case 0:
            work->count = 32;
            work->rows = 6;
            work->cols = 8;
            break;
        }
        work->radius = D_8013DBF8[command].radius;
        work->height = D_8013DBF8[command].height;
        for (i = 0; i < work->count; i++) {
            work->blooms[i].duration = 16;
            work->blooms[i].active = 0;
            work->blooms[i].elapsed = 0;
            work->blooms[i].mode = 1;
            work->blooms[i].coord = Model_GetCameraCoordinateUnit();
            work->blooms[i].color[0] = D_8013DBF8[command].color[0];
            work->blooms[i].color[1] = D_8013DBF8[command].color[1];
            work->blooms[i].color[2] = D_8013DBF8[command].color[2];
            work->blooms[i].u = 0;
            work->blooms[i].v = 0;
            setVector(&work->blooms[i].scale, 4096, 4096, 4096);
            setVector(&work->blooms[i].target, 0, 0, 0);
            turn = work->direction * 6;
            setVector(&work->blooms[i].rotation, i * turn, 0x2AA, 0);
            setVector(&work->blooms[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->rings[i].duration = 32;
            work->rings[i].active = 0;
            work->rings[i].elapsed = 0;
            work->rings[i].mode = 1;
            work->rings[i].coord = Model_GetCameraCoordinateUnit();
            work->rings[i].color[0] = D_8013DBF8[command].color[3];
            work->rings[i].color[1] = D_8013DBF8[command].color[4];
            work->rings[i].color[2] = D_8013DBF8[command].color[5];
            work->rings[i].u = 0;
            work->rings[i].v = 64;
            setVector(&work->rings[i].scale, 4096, 4096, 4096);
            setVector(&work->rings[i].target, 0, 0, 0);
            setVector(&work->rings[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->rings[i].position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->bursts[i].active = 0;
            work->bursts[i].elapsed = 0;
            work->bursts[i].duration = 32;
            work->bursts[i].mode = 1;
            work->bursts[i].coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].color[0] = D_8013DBF8[command].color[3];
            work->bursts[i].color[1] = D_8013DBF8[command].color[4];
            work->bursts[i].color[2] = D_8013DBF8[command].color[5];
            work->bursts[i].u = 0;
            work->bursts[i].v = 32;
            setVector(&work->bursts[i].scale, 4096, 4096, 4096);
            setVector(&work->bursts[i].target, 0, 0, 0);
            setVector(&work->bursts[i].rotation, 0, work->slot * 2048, 0);
            value = work->radius * (rsin(i * 500) * 5 << 2) / 4096;
            sign = i % 2 * 2 - 1;
            setVector(&work->bursts[i].position, sign * value * rsin(i % 16 << 8) / 4096,
                sign * value * rcos(i % 16 << 8) / 4096, value * rsin(i % 16 << 8) / 4096);
            work->bursts[i].position.vy -= 350;
            work->bursts[i].position.vz += work->direction * 450;
        }
        for (j = 0; j < work->rows; j++) {
            for (i = 0; i < work->cols; i++) {
                work->sparks[i][j].active = 0;
                work->sparks[i][j].elapsed = 0;
                work->sparks[i][j].duration = 16;
                work->sparks[i][j].coord = Model_GetCameraCoordinateUnit();
                work->sparks[i][j].color[0] = D_8013DBF8[command].color[0];
                work->sparks[i][j].color[1] = D_8013DBF8[command].color[1];
                work->sparks[i][j].color[2] = D_8013DBF8[command].color[2];
                work->sparks[i][j].u = 0;
                setVector(&work->sparks[i][j].position, 0, 0, 0);
            }
        }
        setVector(&offset, work->radius * 6, 0, work->radius * -(work->direction * 2));
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < 5; i++) {
                PushMatrix();
                rotation = D_800FE168;
                RotMatrixZ((i << 12) / 5, &rotation);
                tilt = work->direction * 6;
                RotMatrixX(j * tilt, &rotation);
                RotMatrixY(0x2AA, &rotation);
                ApplyMatrixLV(&rotation, &offset, &work->cone[i][j]);
                PopMatrix();
            }
        }
        setVector(&offset, 0, work->radius * 20, 0);
        for (i = 0; i < 5; i++) {
            PushMatrix();
            rotation = D_800FE168;
            RotMatrixZ((i << 11) / 5, &rotation);
            RotMatrixY((i % 3 << 12) / 3, &rotation);
            ApplyMatrixLV(&rotation, &offset, &work->spokes[i]);
            PopMatrix();
        }
        sign = work->height * work->direction;
        for (i = 0; i < 5; i++) {
            value = work->radius * rcos(i * 819 / 4) / 4096;
            for (j = 0; j < 5; j++) {
                setVector(&work->petals[j][i][0], value * rsin(j << 10) / 4096, value * rcos(j << 10) / 4096,
                    i * sign / 4);
                work->petals[j][i][1] = work->petals[j][i][0];
                work->petals[j][i][0].vy += 30;
                work->petals[j][i][1].vy -= 30;
            }
        }
        value = work->radius * 2;
        setVector(&work->flare[0], -value, -value, corner);
        setVector(&work->flare[1], value, -value, 0);
        setVector(&work->flare[2], -value, value, 0);
        setVector(&work->flare[3], value, value, 0);
        value = work->radius * 2;
        sign = work->radius * 4;
        for (i = 0; i < 6; i++) {
            angle = (i << 12) / 5;
            setVector(&work->rim[0][i][0], value * rsin(angle) / 4096, value * rcos(angle) / 4096, 0);
            setVector(&work->rim[1][i][0], sign * rsin(angle) / 4096, sign * rcos(angle) / 4096,
                work->height * 2 / 3 * work->direction);
            for (j = 0; j < 2; j++) {
                work->rim[j][i][1] = work->rim[j][i][0];
                work->rim[j][i][0].vy += 30;
                work->rim[j][i][1].vy -= 30;
            }
        }
        value = work->radius * 7;
        setVector(&work->squares[0][0], -value, -value, 0);
        setVector(&work->squares[0][1], value, -value, 0);
        setVector(&work->squares[0][2], -value, value, 0);
        setVector(&work->squares[0][3], value, value, 0);
        value = work->radius * 4;
        setVector(&work->squares[1][0], -value, -value, 0);
        setVector(&work->squares[1][1], value, -value, 0);
        setVector(&work->squares[1][2], -value, value, 0);
        setVector(&work->squares[1][3], value, value, 0);
        value = work->radius * 2;
        setVector(&work->squares[2][0], -value, -value, 0);
        setVector(&work->squares[2][1], value, -value, 0);
        setVector(&work->squares[2][2], -value, value, 0);
        setVector(&work->squares[2][3], value, value, 0);
        work->delay = D_8013DBF8[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed >= 98 && work->elapsed < 204) {
                if (work->elapsed >= work->flags[0] * 3 + 98 && work->flags[0] < work->count) {
                    work->blooms[work->flags[0]].active = 1;
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(
                        work->slot, (work->joints[work->flags[0] & 3] >> 1) + 29);
                    GsGetLwUnit(&unit, &ls);
                    (work->blooms + work->flags[0])->position.vx = ls.t[0];
                    (work->blooms + work->flags[0])->position.vy = ls.t[1];
                    (work->blooms + work->flags[0])->position.vz = ls.t[2];
                    work->rings[work->flags[0]].active = 1;
                    work->rings[work->flags[0]].position = work->blooms[work->flags[0]].position;
                    work->flags[0]++;
                }
            }
            if (work->elapsed < 78 || (work->elapsed >= 257 && work->elapsed < 358)) {
                for (i = 0; i < work->rows; i++) {
                    work->sparks[work->flags[1]][i].active = 1;
                    work->sparks[work->flags[1]][i].elapsed = 0;
                    work->sparks[work->flags[1]][i].u = (i * 5 + work->flags[1] * 3) % 8 << 5;
                    work->sparks[work->flags[1]][i].color[0] = D_8013DBF8[work->command].color[0];
                    work->sparks[work->flags[1]][i].color[1] = D_8013DBF8[work->command].color[1];
                    work->sparks[work->flags[1]][i].color[2] = D_8013DBF8[work->command].color[2];
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, i + 34);
                    GsGetLwUnit(&unit, &ls);
                    (work->sparks[work->flags[1]] + i)->position.vx = ls.t[0];
                    (work->sparks[work->flags[1]] + i)->position.vy = ls.t[1];
                    (work->sparks[work->flags[1]] + i)->position.vz = ls.t[2];
                }
                work->flags[1]++;
                if (work->flags[1] >= work->cols) {
                    work->flags[1] = 0;
                }
            }
            if (work->elapsed >= 301) {
                work->flags[2] = 1;
            }
            if (work->elapsed >= 223 && work->flags[4] == 0) {
                work->flags[4] = result = 1;
            }
            break;
        }
        PushMatrix();
        view = *(Model680UsViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->blooms[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->blooms[i].elapsed << 12) / work->blooms[i].duration;
                setVector(&work->blooms[i].scale, 4096, 4096, value);
                ScaleMatrix(&unit.matrix, &work->blooms[i].scale);
                RotMatrixX(work->blooms[i].rotation.vx, &unit.matrix);
                RotMatrixY(work->blooms[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->blooms[i].position.vx;
                unit.matrix.t[1] = work->blooms[i].position.vy;
                unit.matrix.t[2] = work->blooms[i].position.vz;
                unit.super = work->blooms[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->blooms[i].elapsed > (u16)(work->blooms[i].duration / 2)) {
                    value = (work->blooms[i].duration - work->blooms[i].elapsed) * 2;
                    sign = work->blooms[i].duration;
                    MODEL680US_FADE(work->blooms[i], 0, value, sign);
                }
                for (j = 0; j < 4; j++) {
                    if (j == 0) {
                        red[0] = 0;
                        green[0] = 0;
                        blue[0] = 0;
                    } else {
                        red[0] = work->blooms[i].color[0] * (4 - j) / 4;
                        green[0] = work->blooms[i].color[1] * (4 - j) / 4;
                        blue[0] = work->blooms[i].color[2] * (4 - j) / 4;
                    }
                    red[1] = work->blooms[i].color[0] * (3 - j) / 4;
                    green[1] = work->blooms[i].color[1] * (3 - j) / 4;
                    blue[1] = work->blooms[i].color[2] * (3 - j) / 4;
                    row[0] = work->blooms[i].v + (4 - j) * 12;
                    row[1] = work->blooms[i].v + (3 - j) * 12;
                    for (k = 0; k < 4; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[0];
                        work->strip.clut = work->clut[0];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->blooms[i].u, row[0], work->blooms[i].u + 31, row[0],
                            work->blooms[i].u, row[1], work->blooms[i].u + 31, row[1]);
                        setRGB0(&work->strip, red[0], green[0], blue[0]);
                        setRGB1(&work->strip, red[0], green[0], blue[0]);
                        setRGB2(&work->strip, red[1], green[1], blue[1]);
                        setRGB3(&work->strip, red[1], green[1], blue[1]);
                        value = i % 2;
                        depth = RotAverage4(&work->petals[k][j][value], &work->petals[k + 1][j][value],
                            &work->petals[k][j + 1][value], &work->petals[k + 1][j + 1][value],
                            (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                            (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                        }
                    }
                }
                for (j = 0; j < 5; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[2];
                    work->strip.clut = work->clut[2];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, 32, 0, 95, 0, 32, 16, 95, 16);
                    setRGB0(&work->strip, work->blooms[i].color[0], work->blooms[i].color[1], work->blooms[i].color[2]);
                    setRGB1(&work->strip, work->blooms[i].color[0], work->blooms[i].color[1], work->blooms[i].color[2]);
                    setRGB2(&work->strip, 0, 0, 0);
                    setRGB3(&work->strip, 0, 0, 0);
                    value = i % 2;
                    depth = RotAverage4(&work->rim[0][j][value], &work->rim[0][j + 1][value],
                        &work->rim[1][j][value], &work->rim[1][j + 1][value],
                        (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1,
                        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);
                    }
                }
                work->blooms[i].elapsed += step;
                if (work->blooms[i].duration < work->blooms[i].elapsed) {
                    work->blooms[i].active = 0;
                    work->bursts[i].active = 1;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->rings[i].active == 1) {
                value = (work->rings[i].elapsed << 12) / work->rings[i].duration + 0x1000;
                setVector(&work->rings[i].scale, value, value, value);
                if (work->rings[i].elapsed > (u16)(work->rings[i].duration / 2)) {
                    value = (work->rings[i].duration - work->rings[i].elapsed) * 2;
                    k = work->rings[i].duration;
                    MODEL680US_FADE(work->rings[i], 3, value, k);
                }
                for (k = 0; k < 5; k++) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    MODEL680US_FACE(work->rings[i]);
                    value = i % 2 * 2 - 1;
                    unit.matrix.t[0] = work->rings[i].position.vx
                        + work->cone[k][i].vx * work->rings[i].elapsed / work->rings[i].duration;
                    unit.matrix.t[1] = work->rings[i].position.vy - value * 30
                        + work->cone[k][i].vy * work->rings[i].elapsed / work->rings[i].duration;
                    unit.matrix.t[2] = work->rings[i].position.vz
                        + work->cone[k][i].vz * work->rings[i].elapsed / work->rings[i].duration;
                    unit.super = work->rings[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    MODEL680US_QUAD(1, work->rings[i].u, work->rings[i].v, 31, 31);
                    setRGB0(&work->quad, work->rings[i].color[0], work->rings[i].color[1], work->rings[i].color[2]);
                    MODEL680US_SORT_QUAD(work->flare);
                }
                if (work->rings[i].u < 224) {
                    work->rings[i].u += 32;
                }
                work->rings[i].elapsed += step;
                if (work->rings[i].duration < work->rings[i].elapsed) {
                    work->rings[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->bursts[i].active == 1) {
                value = (work->bursts[i].elapsed << 13) / work->bursts[i].duration + 0x1000;
                setVector(&work->bursts[i].scale, value, value, value);
                if (work->bursts[i].elapsed > (u16)(work->bursts[i].duration / 2)) {
                    value = (work->bursts[i].duration - work->bursts[i].elapsed) * 2;
                    sign = work->bursts[i].duration;
                    MODEL680US_FADE(work->bursts[i], 3, value, sign);
                }
                unit.flg = 0;
                unit.matrix = D_800FE168;
                MODEL680US_FACE(work->bursts[i]);
                unit.matrix.t[0] = work->bursts[i].position.vx;
                unit.matrix.t[1] = work->bursts[i].position.vy;
                unit.matrix.t[2] = work->bursts[i].position.vz;
                unit.super = work->bursts[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                MODEL680US_QUAD(3, work->frame_u[work->bursts[i].u], work->frame_v[work->bursts[i].u], 63, 63);
                setRGB0(&work->quad, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                MODEL680US_SORT_QUAD(work->squares[0]);
                for (j = 0; j < 5; j++) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    MODEL680US_FACE(work->bursts[i]);
                    unit.matrix.t[0] = work->bursts[i].position.vx
                        + work->spokes[j].vx * work->bursts[i].elapsed / work->bursts[i].duration;
                    unit.matrix.t[1] = work->bursts[i].position.vy
                        + work->spokes[j].vy * work->bursts[i].elapsed / work->bursts[i].duration;
                    unit.matrix.t[2] = work->bursts[i].position.vz
                        + work->spokes[j].vz * work->bursts[i].elapsed / work->bursts[i].duration;
                    unit.super = work->bursts[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    MODEL680US_QUAD(1, work->bursts[i].v, 64, 31, 31);
                    setRGB0(&work->quad, work->bursts[i].color[0], work->bursts[i].color[1], work->bursts[i].color[2]);
                    MODEL680US_SORT_QUAD(work->squares[1]);
                }
                if (work->bursts[i].u < 7) {
                    work->bursts[i].u++;
                }
                if (work->bursts[i].v < 224) {
                    work->bursts[i].v += 32;
                }
                work->bursts[i].elapsed += step;
                if (work->bursts[i].duration < work->bursts[i].elapsed) {
                    work->bursts[i].active = 0;
                }
                if (work->flags[4] == 0) {
                    result = 4;
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->rows; j++) {
            for (i = 0; i < work->cols; i++) {
                if (work->sparks[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    value = (work->sparks[i][j].elapsed << 12) / work->sparks[i][j].duration + 0x1000;
                    setVector(&offset, value, value, value);
                    ScaleMatrix(&unit.matrix, &offset);
                    RotMatrixZ(0, &unit.matrix);
                    RotMatrixX(-((view.pitch - 0x1000) * work->direction), &unit.matrix);
                    RotMatrixY(work->slot * 2048 + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][j].position.vx;
                    unit.matrix.t[1] = work->sparks[i][j].position.vy;
                    unit.matrix.t[2] = work->sparks[i][j].position.vz;
                    unit.super = work->sparks[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    value = work->sparks[i][j].duration - work->sparks[i][j].elapsed;
                    sign = work->sparks[i][j].duration;
                    MODEL680US_FADE(work->sparks[i][j], 0, value, sign);
                    MODEL680US_QUAD(1, work->sparks[i][j].u, 64, 31, 31);
                    setRGB0(&work->quad, work->sparks[i][j].color[0], work->sparks[i][j].color[1], work->sparks[i][j].color[2]);
                    MODEL680US_SORT_QUAD(work->squares[2]);
                    work->sparks[i][j].u += 32;
                    if (work->sparks[i][j].u > 224) {
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
        PopMatrix();
        work->elapsed += step;
        if (inactive == work->count * 3 + work->rows * work->cols && work->flags[2] == 1) {
            result = 2;
        }
    }
    return result;
}
