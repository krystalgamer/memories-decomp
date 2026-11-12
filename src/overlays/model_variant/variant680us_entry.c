#include "../../types.h"
#include "variant680us_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 680's entry: blooms of shaded petals open at model joints and
 * release rings that fly out along a cone, bursts that flash and shoot
 * five spokes, and rows of sparks shed from further joints. The setup
 * also prints the work-area size. */

/* Fades COLOR[0..2] of E (an element head or a spark) from the command
 * colour at OFFSET. */
#define MODEL680US_FADE(e, offset, value, divisor)                                     \
    (e).color[0] = gModelVariant680Us_aConfigs[work->head.command].color[(offset) + 0] * (value) / (divisor); \
    (e).color[1] = gModelVariant680Us_aConfigs[work->head.command].color[(offset) + 1] * (value) / (divisor); \
    (e).color[2] = gModelVariant680Us_aConfigs[work->head.command].color[(offset) + 2] * (value) / (divisor)

/* Loads the camera-facing rotation of element E into UNIT. */
#define MODEL680US_FACE(e)                                                              \
    ScaleMatrix(&unit.matrix, &(e).xform.scale);                                              \
    RotMatrixZ((e).xform.rotation.vz, &unit.matrix);                                          \
    RotMatrixX((e).xform.rotation.vx - (view.pitch - 0x1000) * work->direction, &unit.matrix); \
    RotMatrixY((e).xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix)

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
        (long *)&work->quad.x0, (long *)&work->quad.x1,                         \
        (long *)&work->quad.x2, (long *)&work->quad.x3,                         \
        &interpolation, &flag);                                                         \
    if (flag >= 0) {                                                                    \
        GsSortPoly(&work->quad, Graphics_GetDefaultOrderingTable(), (u16)depth);                           \
    }

s32 ModelVariant680Us_HandleRequest(u8 *context, s32 command)
{
    Model680UsState *work;
    MATRIX ls;
    MATRIX rotation;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshot view;
    u8 red[2], green[2], blue[2], row[2];
    long interpolation, flag;
    VECTOR offset;
    s32 result, step, inactive;
    s32 i, j, k;
    s32 value, sign, angle;
    s32 turn, tilt;
    u32 tex;
    s16 corner;
    long depth;

    work = (Model680UsState *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model680UsState));
#ifndef VERSION_FRENCH
        printf(gModelVariant680Us_szWorkSizeFormat, sizeof(Model680UsState));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
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
         * assignment keeps the gModelVariant680Us_aImages high half ahead of the first
         * call's constant argument.  The assignment itself is sunk into
         * its single use before register allocation. */
        corner = 0;
        do {
        } while (0);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant680Us_aImages[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant680Us_aImages[1]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant680Us_aImages[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant680Us_aImages[3]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant680Us_aImages[4]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant680Us_aImages[5]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant680Us_aImages[6]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant680Us_aImages[7]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant680Us_aImages[8]);
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant680Us_aImages[9]);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant680Us_aImages[10]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        switch (command) {
        case 0:
            work->count = 32;
            work->rows = 6;
            work->cols = 8;
            break;
        }
        work->radius = gModelVariant680Us_aConfigs[command].radius;
        work->height = gModelVariant680Us_aConfigs[command].height;
        for (i = 0; i < work->count; i++) {
            work->blooms[i].head.duration = 16;
            work->blooms[i].head.active = 0;
            work->blooms[i].head.elapsed = 0;
            work->blooms[i].head.mode = 1;
            work->blooms[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->blooms[i].head.color[0] = gModelVariant680Us_aConfigs[command].color[0];
            work->blooms[i].head.color[1] = gModelVariant680Us_aConfigs[command].color[1];
            work->blooms[i].head.color[2] = gModelVariant680Us_aConfigs[command].color[2];
            work->blooms[i].head.u = 0;
            work->blooms[i].head.v = 0;
            setVector(&work->blooms[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->blooms[i].target, 0, 0, 0);
            turn = work->direction * 6;
            setVector(&work->blooms[i].xform.rotation, i * turn, 0x2AA, 0);
            setVector(&work->blooms[i].xform.position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->rings[i].head.duration = 32;
            work->rings[i].head.active = 0;
            work->rings[i].head.elapsed = 0;
            work->rings[i].head.mode = 1;
            work->rings[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->rings[i].head.color[0] = gModelVariant680Us_aConfigs[command].color[3];
            work->rings[i].head.color[1] = gModelVariant680Us_aConfigs[command].color[4];
            work->rings[i].head.color[2] = gModelVariant680Us_aConfigs[command].color[5];
            work->rings[i].head.u = 0;
            work->rings[i].head.v = 64;
            setVector(&work->rings[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->rings[i].target, 0, 0, 0);
            setVector(&work->rings[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->rings[i].xform.position, 0, 0, 0);
        }
        for (i = 0; i < work->count; i++) {
            work->bursts[i].head.active = 0;
            work->bursts[i].head.elapsed = 0;
            work->bursts[i].head.duration = 32;
            work->bursts[i].head.mode = 1;
            work->bursts[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->bursts[i].head.color[0] = gModelVariant680Us_aConfigs[command].color[3];
            work->bursts[i].head.color[1] = gModelVariant680Us_aConfigs[command].color[4];
            work->bursts[i].head.color[2] = gModelVariant680Us_aConfigs[command].color[5];
            work->bursts[i].head.u = 0;
            work->bursts[i].head.v = 32;
            setVector(&work->bursts[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->bursts[i].target, 0, 0, 0);
            setVector(&work->bursts[i].xform.rotation, 0, work->head.slot * 2048, 0);
            value = work->radius * (s32)((u32)(rsin(i * 500) * 5) << 2) / 4096;
            sign = i % 2 * 2 - 1;
            setVector(&work->bursts[i].xform.position, sign * value * rsin(i % 16 << 8) / 4096,
                sign * value * rcos(i % 16 << 8) / 4096, value * rsin(i % 16 << 8) / 4096);
            work->bursts[i].xform.position.vy -= 350;
            work->bursts[i].xform.position.vz += work->direction * 450;
        }
        for (j = 0; j < work->rows; j++) {
            for (i = 0; i < work->cols; i++) {
                work->sparks[i][j].active = 0;
                work->sparks[i][j].elapsed = 0;
                work->sparks[i][j].duration = 16;
                work->sparks[i][j].coord = Model_GetCameraCoordinateUnit();
                work->sparks[i][j].color[0] = gModelVariant680Us_aConfigs[command].color[0];
                work->sparks[i][j].color[1] = gModelVariant680Us_aConfigs[command].color[1];
                work->sparks[i][j].color[2] = gModelVariant680Us_aConfigs[command].color[2];
                work->sparks[i][j].u = 0;
                setVector(&work->sparks[i][j].position, 0, 0, 0);
            }
        }
        setVector(&offset, work->radius * 6, 0, work->radius * -(work->direction * 2));
        for (j = 0; j < work->count; j++) {
            for (i = 0; i < 5; i++) {
                PushMatrix();
                rotation = GsIDMATRIX;
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
            rotation = GsIDMATRIX;
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
        work->delay = gModelVariant680Us_aConfigs[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->head.command) {
        case 0:
            if (work->head.elapsed >= 98 && work->head.elapsed < 204) {
                if (work->head.elapsed >= work->flags[0] * 3 + 98 && work->flags[0] < work->count) {
                    work->blooms[work->flags[0]].head.active = 1;
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(
                        work->head.slot, (work->joints[work->flags[0] & 3] >> 1) + 29);
                    GsGetLwUnit(&unit, &ls);
                    (work->blooms + work->flags[0])->xform.position.vx = ls.t[0];
                    (work->blooms + work->flags[0])->xform.position.vy = ls.t[1];
                    (work->blooms + work->flags[0])->xform.position.vz = ls.t[2];
                    work->rings[work->flags[0]].head.active = 1;
                    work->rings[work->flags[0]].xform.position = work->blooms[work->flags[0]].xform.position;
                    work->flags[0]++;
                }
            }
            if (work->head.elapsed < 78 || (work->head.elapsed >= 257 && work->head.elapsed < 358)) {
                for (i = 0; i < work->rows; i++) {
                    work->sparks[work->flags[1]][i].active = 1;
                    work->sparks[work->flags[1]][i].elapsed = 0;
                    work->sparks[work->flags[1]][i].u = (i * 5 + work->flags[1] * 3) % 8 << 5;
                    work->sparks[work->flags[1]][i].color[0] = gModelVariant680Us_aConfigs[work->head.command].color[0];
                    work->sparks[work->flags[1]][i].color[1] = gModelVariant680Us_aConfigs[work->head.command].color[1];
                    work->sparks[work->flags[1]][i].color[2] = gModelVariant680Us_aConfigs[work->head.command].color[2];
                    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, i + 34);
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
            if (work->head.elapsed >= 301) {
                work->flags[2] = 1;
            }
            if (work->head.elapsed >= 223 && work->flags[4] == 0) {
                work->flags[4] = result = 1;
            }
            break;
        }
        PushMatrix();
        view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->count; i++) {
            if (work->blooms[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                value = (work->blooms[i].head.elapsed << 12) / work->blooms[i].head.duration;
                setVector(&work->blooms[i].xform.scale, 4096, 4096, value);
                ScaleMatrix(&unit.matrix, &work->blooms[i].xform.scale);
                RotMatrixX(work->blooms[i].xform.rotation.vx, &unit.matrix);
                RotMatrixY(work->blooms[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->blooms[i].xform.position.vx;
                unit.matrix.t[1] = work->blooms[i].xform.position.vy;
                unit.matrix.t[2] = work->blooms[i].xform.position.vz;
                unit.super = work->blooms[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->blooms[i].head.elapsed > (u16)(work->blooms[i].head.duration / 2)) {
                    value = (work->blooms[i].head.duration - work->blooms[i].head.elapsed) * 2;
                    sign = work->blooms[i].head.duration;
                    MODEL680US_FADE(work->blooms[i].head, 0, value, sign);
                }
                for (j = 0; j < 4; j++) {
                    if (j == 0) {
                        red[0] = 0;
                        green[0] = 0;
                        blue[0] = 0;
                    } else {
                        red[0] = work->blooms[i].head.color[0] * (4 - j) / 4;
                        green[0] = work->blooms[i].head.color[1] * (4 - j) / 4;
                        blue[0] = work->blooms[i].head.color[2] * (4 - j) / 4;
                    }
                    red[1] = work->blooms[i].head.color[0] * (3 - j) / 4;
                    green[1] = work->blooms[i].head.color[1] * (3 - j) / 4;
                    blue[1] = work->blooms[i].head.color[2] * (3 - j) / 4;
                    row[0] = work->blooms[i].head.v + (4 - j) * 12;
                    row[1] = work->blooms[i].head.v + (3 - j) * 12;
                    for (k = 0; k < 4; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[0];
                        work->strip.clut = work->clut[0];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->blooms[i].head.u, row[0], work->blooms[i].head.u + 31, row[0],
                            work->blooms[i].head.u, row[1], work->blooms[i].head.u + 31, row[1]);
                        setRGB0(&work->strip, red[0], green[0], blue[0]);
                        setRGB1(&work->strip, red[0], green[0], blue[0]);
                        setRGB2(&work->strip, red[1], green[1], blue[1]);
                        setRGB3(&work->strip, red[1], green[1], blue[1]);
                        value = i % 2;
                        depth = RotAverage4(&work->petals[k][j][value], &work->petals[k + 1][j][value],
                            &work->petals[k][j + 1][value], &work->petals[k + 1][j + 1][value],
                            (long *)&work->strip.x0, (long *)&work->strip.x1,
                            (long *)&work->strip.x2, (long *)&work->strip.x3,
                            &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                for (j = 0; j < 5; j++) {
                    setPolyGT4(&work->strip);
                    work->strip.tpage = work->tpage[2];
                    work->strip.clut = work->clut[2];
                    setSemiTrans(&work->strip, 1);
                    setUV4(&work->strip, 32, 0, 95, 0, 32, 16, 95, 16);
                    setRGB0(&work->strip, work->blooms[i].head.color[0], work->blooms[i].head.color[1], work->blooms[i].head.color[2]);
                    setRGB1(&work->strip, work->blooms[i].head.color[0], work->blooms[i].head.color[1], work->blooms[i].head.color[2]);
                    setRGB2(&work->strip, 0, 0, 0);
                    setRGB3(&work->strip, 0, 0, 0);
                    value = i % 2;
                    depth = RotAverage4(&work->rim[0][j][value], &work->rim[0][j + 1][value],
                        &work->rim[1][j][value], &work->rim[1][j + 1][value],
                        (long *)&work->strip.x0, (long *)&work->strip.x1,
                        (long *)&work->strip.x2, (long *)&work->strip.x3,
                        &interpolation, &flag);
                    if (flag >= 0) {
                        GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);
                    }
                }
                work->blooms[i].head.elapsed += step;
                if (work->blooms[i].head.duration < work->blooms[i].head.elapsed) {
                    work->blooms[i].head.active = 0;
                    work->bursts[i].head.active = 1;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->rings[i].head.active == 1) {
                value = (work->rings[i].head.elapsed << 12) / work->rings[i].head.duration + 0x1000;
                setVector(&work->rings[i].xform.scale, value, value, value);
                if (work->rings[i].head.elapsed > (u16)(work->rings[i].head.duration / 2)) {
                    value = (work->rings[i].head.duration - work->rings[i].head.elapsed) * 2;
                    k = work->rings[i].head.duration;
                    MODEL680US_FADE(work->rings[i].head, 3, value, k);
                }
                for (k = 0; k < 5; k++) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    MODEL680US_FACE(work->rings[i]);
                    value = i % 2 * 2 - 1;
                    unit.matrix.t[0] = work->rings[i].xform.position.vx
                        + work->cone[k][i].vx * work->rings[i].head.elapsed / work->rings[i].head.duration;
                    unit.matrix.t[1] = work->rings[i].xform.position.vy - value * 30
                        + work->cone[k][i].vy * work->rings[i].head.elapsed / work->rings[i].head.duration;
                    unit.matrix.t[2] = work->rings[i].xform.position.vz
                        + work->cone[k][i].vz * work->rings[i].head.elapsed / work->rings[i].head.duration;
                    unit.super = work->rings[i].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    MODEL680US_QUAD(1, work->rings[i].head.u, work->rings[i].head.v, 31, 31);
                    setRGB0(&work->quad, work->rings[i].head.color[0], work->rings[i].head.color[1], work->rings[i].head.color[2]);
                    MODEL680US_SORT_QUAD(work->flare);
                }
                if (work->rings[i].head.u < 224) {
                    work->rings[i].head.u += 32;
                }
                work->rings[i].head.elapsed += step;
                if (work->rings[i].head.duration < work->rings[i].head.elapsed) {
                    work->rings[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (i = 0; i < work->count; i++) {
            if (work->bursts[i].head.active == 1) {
                value = (work->bursts[i].head.elapsed << 13) / work->bursts[i].head.duration + 0x1000;
                setVector(&work->bursts[i].xform.scale, value, value, value);
                if (work->bursts[i].head.elapsed > (u16)(work->bursts[i].head.duration / 2)) {
                    value = (work->bursts[i].head.duration - work->bursts[i].head.elapsed) * 2;
                    sign = work->bursts[i].head.duration;
                    MODEL680US_FADE(work->bursts[i].head, 3, value, sign);
                }
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                MODEL680US_FACE(work->bursts[i]);
                unit.matrix.t[0] = work->bursts[i].xform.position.vx;
                unit.matrix.t[1] = work->bursts[i].xform.position.vy;
                unit.matrix.t[2] = work->bursts[i].xform.position.vz;
                unit.super = work->bursts[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                MODEL680US_QUAD(3, work->frame_u[work->bursts[i].head.u], work->frame_v[work->bursts[i].head.u], 63, 63);
                setRGB0(&work->quad, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                MODEL680US_SORT_QUAD(work->squares[0]);
                for (j = 0; j < 5; j++) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    MODEL680US_FACE(work->bursts[i]);
                    unit.matrix.t[0] = work->bursts[i].xform.position.vx
                        + work->spokes[j].vx * work->bursts[i].head.elapsed / work->bursts[i].head.duration;
                    unit.matrix.t[1] = work->bursts[i].xform.position.vy
                        + work->spokes[j].vy * work->bursts[i].head.elapsed / work->bursts[i].head.duration;
                    unit.matrix.t[2] = work->bursts[i].xform.position.vz
                        + work->spokes[j].vz * work->bursts[i].head.elapsed / work->bursts[i].head.duration;
                    unit.super = work->bursts[i].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    MODEL680US_QUAD(1, work->bursts[i].head.v, 64, 31, 31);
                    setRGB0(&work->quad, work->bursts[i].head.color[0], work->bursts[i].head.color[1], work->bursts[i].head.color[2]);
                    MODEL680US_SORT_QUAD(work->squares[1]);
                }
                if (work->bursts[i].head.u < 7) {
                    work->bursts[i].head.u++;
                }
                if (work->bursts[i].head.v < 224) {
                    work->bursts[i].head.v += 32;
                }
                work->bursts[i].head.elapsed += step;
                if (work->bursts[i].head.duration < work->bursts[i].head.elapsed) {
                    work->bursts[i].head.active = 0;
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
                    unit.matrix = GsIDMATRIX;
                    value = (work->sparks[i][j].elapsed << 12) / work->sparks[i][j].duration + 0x1000;
                    setVector(&offset, value, value, value);
                    ScaleMatrix(&unit.matrix, &offset);
                    RotMatrixZ(0, &unit.matrix);
                    RotMatrixX(-((view.pitch - 0x1000) * work->direction), &unit.matrix);
                    RotMatrixY(work->head.slot * 2048 + (0xC06 - view.yaw), &unit.matrix);
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
        work->head.elapsed += step;
        if (inactive == work->count * 3 + work->rows * work->cols && work->flags[2] == 1) {
            result = 2;
        }
    }
    return result;
}
