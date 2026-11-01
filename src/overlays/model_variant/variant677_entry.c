#include "../../types.h"
#include "variant677_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 677's entry: sparks released in rows from model points drift
 * towards the camera and spin out as flipbook sprites; each spent spark
 * becomes a ring that throws five sprites outwards and turns a striped
 * band (and, for command 1, a second cone band), while a matching burst
 * of five sprites spreads from the same point. The setup also prints the
 * work-area size. */

#define FADE(element, first)                                                   \
    if ((element).duration / 2 < (element).elapsed) {                          \
        value = ((element).duration - (element).elapsed) * 2;                  \
        amount = (element).duration;                                           \
        (element).color[0] = gModelVariant677_D2898[work->command].color[first] * value / amount; \
        (element).color[1] = gModelVariant677_D2898[work->command].color[first + 1] * value / amount; \
        (element).color[2] = gModelVariant677_D2898[work->command].color[first + 2] * value / amount; \
    }

#define DRAW_SPRITE(page, u0, v0, u1, v1, u2, v2, u3, v3, element, a, b, c, d)   \
    setPolyFT4(&work->sprite);                                                 \
    work->sprite.tpage = work->tpage[page];                                    \
    work->sprite.clut = work->clut[page];                                      \
    setSemiTrans(&work->sprite, 1);                                            \
    setUV4(&work->sprite, u0, v0, u1, v1, u2, v2, u3, v3);                     \
    setRGB0(&work->sprite, (element).color[0], (element).color[1], (element).color[2]); \
    depth = RotAverage4(a, b, c, d, (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1, \
        (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3, &interpolation, &flag); \
    if (flag >= 0) {                                                           \
        GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);                \
    }

#define DRAW_STRIP(element, a, b, c, d)                                        \
    setPolyGT4(&work->strip);                                                  \
    work->strip.tpage = work->tpage[3];                                        \
    work->strip.clut = work->clut[3];                                          \
    setSemiTrans(&work->strip, 1);                                             \
    setUV4(&work->strip, 0, 64, 31, 64, 0, 127, 31, 127);                      \
    setRGB0(&work->strip, 0, 0, 0);                                            \
    setRGB1(&work->strip, 0, 0, 0);                                            \
    setRGB2(&work->strip, (element).color[0], (element).color[1], (element).color[2]); \
    setRGB3(&work->strip, (element).color[0], (element).color[1], (element).color[2]); \
    depth = RotAverage4(a, b, c, d, (PSXLONG *)&work->strip.x0, (PSXLONG *)&work->strip.x1, \
        (PSXLONG *)&work->strip.x2, (PSXLONG *)&work->strip.x3, &interpolation, &flag); \
    if (flag >= 0) {                                                           \
        GsSortPoly(&work->strip, func_80058F10(), (u16)depth);                 \
    }

#define INIT_ELEMENT(element, length, first)                                   \
    (element).active = 0;                                                      \
    (element).elapsed = 0;                                                     \
    (element).duration = length;                                               \
    (element).mode = 1;                                                        \
    (element).coord = Model_GetCameraCoordinateUnit();                         \
    (element).color[0] = gModelVariant677_D2898[command].color[first];                     \
    (element).color[1] = gModelVariant677_D2898[command].color[first + 1];                 \
    (element).color[2] = gModelVariant677_D2898[command].color[first + 2]

#define START_SPARKS(index)                                                    \
    if (work->flags[0] < work->row_count && work->elapsed >= work->flags[0] * 20) { \
        for (i = 0; i < work->column_count; i++) {                             \
            work->sparks[work->flags[0]][i].active = 1;                        \
            unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, index);  \
            GsGetLwUnit(&unit, &ls);                                           \
            setVector(&work->sparks[work->flags[0]][i].position, ls.t[0], ls.t[1], ls.t[2]); \
            work->bursts[work->flags[0]][i].active = 1;                        \
            work->bursts[work->flags[0]][i].position = work->sparks[work->flags[0]][i].position; \
        }                                                                      \
        work->flags[0]++;                                                      \
    }

s32 ModelVariant677_HandleRequest(u8 *context, s32 command)
{
    Model677State *work;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    Model677ViewSnapshot view;
    VECTOR vec;
    PSXLONG interpolation, flag;
    s32 result, step, inactive;
    s32 i, j, k, value, amount;
    u32 tex;
    s32 columns;
    PSXLONG depth;

    work = (Model677State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model677State));
#ifndef VERSION_FRENCH
        printf(gModelVariant677_D274C, sizeof(Model677State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        for (i = 0; i < 8; i++) {
            work->cell_u[i] = i % 4 * 64;
            work->cell_v[i] = i / 4 * 64 - 128;
        }
        /* Scheduling barrier: the empty loop after the early column
         * assignment keeps the gModelVariant677_D2764 high half ahead of the first
         * call's constant argument.  The assignment itself is sunk into
         * the switch below before register allocation. */
        columns = 2;
        do {
        } while (0);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant677_D2764[2]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant677_D2764[3]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant677_D2764[4]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant677_D2764[5]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant677_D2764[6]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant677_D2764[7]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant677_D2764[8]);
        ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant677_D2764[9]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant677_D2764[10]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant677_D2764[0]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &gModelVariant677_D2764[1]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        switch (command) {
        case 0:
            work->column_count = columns;
            work->row_count = 8;
            break;
        case 1:
            work->column_count = 1;
            work->row_count = 5;
            break;
        }
        work->radius = gModelVariant677_D2898[command].radius;
        work->height = gModelVariant677_D2898[command].height;
        for (i = 0; i < 5; i++) {
            PushMatrix();
            m = D_800FE168;
            switch (command) {
            case 0:
            case 1:
                setVector(&vec, work->radius * 2, 0, -(work->radius * work->direction));
                RotMatrixZ(i * 4096 / 5, &m);
                break;
            }
            ApplyMatrixLV(&m, &vec, &work->offsets[i]);
            PopMatrix();
        }
        for (i = 0; i < 5; i++) {
            PushMatrix();
            m = D_800FE168;
            switch (command) {
            case 0:
            case 1:
                setVector(&vec, 0, work->radius * 5, 0);
                RotMatrixZ(i * 2048 / 5, &m);
                RotMatrixY(i % 3 * 4096 / 3, &m);
                break;
            }
            ApplyMatrixLV(&m, &vec, &work->spread[i]);
            PopMatrix();
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                INIT_ELEMENT(work->sparks[i][j], 32, 0);
                work->sparks[i][j].u = 0;
                work->sparks[i][j].v = 0;
                setVector(&work->sparks[i][j].scale, 4096, 4096, 4096);
                setVector(&work->sparks[i][j].target, 0, 0, work->direction * 450);
                setVector(&work->sparks[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->sparks[i][j].position, 0, 0, 0);
            }
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                INIT_ELEMENT(work->rings[i][j], 48, 3);
                work->rings[i][j].u = 0;
                work->rings[i][j].v = 0;
                setVector(&work->rings[i][j].scale, 4096, 4096, 4096);
                setVector(&work->rings[i][j].target, 0, 0, 0);
                setVector(&work->rings[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->rings[i][j].position, 0, 0, 0);
            }
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                INIT_ELEMENT(work->bursts[i][j], 32, 3);
                work->bursts[i][j].u = 0;
                work->bursts[i][j].v = 32;
                setVector(&work->bursts[i][j].scale, 4096, 4096, 4096);
                setVector(&work->bursts[i][j].target, 0, 0, 0);
                setVector(&work->bursts[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->bursts[i][j].position, 0, 0, 0);
            }
        }
        work->delay = gModelVariant677_D2898[command].delay;
        for (i = 0; i < 5; i++) {
            work->flags[i] = 0;
        }
        value = work->radius;
        setVector(&work->corners[0], -value, -value, 0);
        setVector(&work->corners[1], value, -value, 0);
        setVector(&work->corners[2], -value, value, 0);
        setVector(&work->corners[3], value, value, 0);
        value = work->radius * 2;
        setVector(&work->ring_corners[0], -value, -value, 0);
        setVector(&work->ring_corners[1], value, -value, 0);
        setVector(&work->ring_corners[2], -value, value, 0);
        setVector(&work->ring_corners[3], value, value, 0);
        value = work->radius >> 1;
        setVector(&work->burst_corners[0], -value, -value, 0);
        setVector(&work->burst_corners[1], value, -value, 0);
        setVector(&work->burst_corners[2], -value, value, 0);
        setVector(&work->burst_corners[3], value, value, 0);
        value = work->radius * 2;
        amount = work->radius * 4;
        for (i = 0; i < 17; i++) {
            setVector(&work->ring_inner[i], value * rsin(i << 8) / 4096, value * rcos(i << 8) / 4096, 0);
            setVector(&work->ring_outer[i], amount * rsin(i << 8) / 4096, amount * rcos(i << 8) / 4096, 0);
            setVector(&work->ring_middle[i], (work->ring_inner[i].vx + work->ring_outer[i].vx) / 2,
                (work->ring_inner[i].vy + work->ring_outer[i].vy) / 2,
                (work->ring_inner[i].vz + work->ring_outer[i].vz) / 2);
            setVector(&work->cone_inner[i], value * rsin(i << 8) / 4096, 0, value * rcos(i << 8) / 4096);
            setVector(&work->cone_outer[i], amount * rsin(i << 8) / 4096, 0, amount * rcos(i << 8) / 4096);
            setVector(&work->cone_middle[i], (work->cone_inner[i].vx + work->cone_outer[i].vx) / 2,
                (work->cone_inner[i].vy + work->cone_outer[i].vy) / 2,
                (work->cone_inner[i].vz + work->cone_outer[i].vz) / 2);
        }
        value = work->radius;
        setVector(&work->wave_corners[0], -value, -value, 0);
        setVector(&work->wave_corners[1], value, -value, 0);
        setVector(&work->wave_corners[2], -value, value, 0);
        setVector(&work->wave_corners[3], value, value, 0);
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            START_SPARKS(i * 2 + 3);
            break;
        case 1:
            START_SPARKS(22);
            break;
        }
        PushMatrix();
        view = *(Model677ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                if (work->sparks[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->sparks[i][j].scale);
                    RotMatrixZ(work->sparks[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->sparks[i][j].rotation.vx - (view.pitch - 4096) * work->direction,
                        &unit.matrix);
                    RotMatrixY(work->sparks[i][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][j].position.vx;
                    unit.matrix.t[1] = work->sparks[i][j].position.vy;
                    unit.matrix.t[2] = work->sparks[i][j].position.vz +
                        (work->sparks[i][j].target.vz - work->sparks[i][j].position.vz) *
                            work->sparks[i][j].elapsed / work->sparks[i][j].duration;
                    unit.super = work->sparks[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    DRAW_SPRITE(0, work->sparks[i][j].u, work->sparks[i][j].v, work->sparks[i][j].u + 31,
                        work->sparks[i][j].v, work->sparks[i][j].u, work->sparks[i][j].v + 31,
                        work->sparks[i][j].u + 31, work->sparks[i][j].v + 31, work->sparks[i][j],
                        &work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3]);
                    work->sparks[i][j].u += 32;
                    if (work->sparks[i][j].u >= 96) {
                        work->sparks[i][j].u = 0;
                    }
                    work->sparks[i][j].elapsed += step;
                    if (work->sparks[i][j].duration < work->sparks[i][j].elapsed) {
                        work->sparks[i][j].active = 0;
                        work->rings[i][j].active = 1;
                        setVector(&work->rings[i][j].position, unit.matrix.t[0], unit.matrix.t[1],
                            unit.matrix.t[2]);
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                if (work->rings[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    value = (work->rings[i][j].elapsed << 13) / work->rings[i][j].duration + 4096;
                    setVector(&work->rings[i][j].scale, value, value, value);
                    ScaleMatrix(&unit.matrix, &work->rings[i][j].scale);
                    RotMatrixZ(work->rings[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->rings[i][j].rotation.vx - (view.pitch - 4096) * work->direction,
                        &unit.matrix);
                    RotMatrixY(work->rings[i][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->rings[i][j].position.vx;
                    unit.matrix.t[1] = work->rings[i][j].position.vy;
                    unit.matrix.t[2] = work->rings[i][j].position.vz;
                    unit.super = work->rings[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    FADE(work->rings[i][j], 3);
                    DRAW_SPRITE(1, work->cell_u[work->rings[i][j].u], work->cell_v[work->rings[i][j].u],
                        work->cell_u[work->rings[i][j].u] + 63, work->cell_v[work->rings[i][j].u],
                        work->cell_u[work->rings[i][j].u], work->cell_v[work->rings[i][j].u] + 63,
                        work->cell_u[work->rings[i][j].u] + 63, work->cell_v[work->rings[i][j].u] + 63,
                        work->rings[i][j], &work->ring_corners[0], &work->ring_corners[1],
                        &work->ring_corners[2], &work->ring_corners[3]);
                    value = j % 2 * 2 - 1;
                    for (k = 0; k < 5; k++) {
                        unit.flg = 0;
                        unit.matrix = D_800FE168;
                        ScaleMatrix(&unit.matrix, &work->rings[i][j].scale);
                        RotMatrixZ(work->rings[i][j].rotation.vz, &unit.matrix);
                        RotMatrixX(work->rings[i][j].rotation.vx - (view.pitch - 4096) * work->direction,
                            &unit.matrix);
                        RotMatrixY(work->rings[i][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                        unit.matrix.t[0] = work->rings[i][j].position.vx +
                            value * work->spread[k].vx * work->rings[i][j].elapsed / work->rings[i][j].duration;
                        unit.matrix.t[1] = work->rings[i][j].position.vy +
                            value * work->spread[k].vy * work->rings[i][j].elapsed / work->rings[i][j].duration;
                        unit.matrix.t[2] = work->rings[i][j].position.vz +
                            value * work->spread[k].vz * work->rings[i][j].elapsed / work->rings[i][j].duration;
                        unit.super = work->rings[i][j].coord;
                        GsGetLsUnit(&unit, &ls);
                        GsSetLsMatrix(&ls);
                        DRAW_SPRITE(2, work->rings[i][j].v, 32, work->rings[i][j].v + 31, 32,
                            work->rings[i][j].v, 63, work->rings[i][j].v + 31, 63, work->rings[i][j],
                            &work->wave_corners[0], &work->wave_corners[1], &work->wave_corners[2],
                            &work->wave_corners[3]);
                    }
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    value = (work->rings[i][j].elapsed << 14) / work->rings[i][j].duration;
                    setVector(&work->rings[i][j].scale, value, value, value);
                    ScaleMatrix(&unit.matrix, &work->rings[i][j].scale);
                    RotMatrixZ(i * 500 + j * 700, &unit.matrix);
                    RotMatrixX((i + j) * 600, &unit.matrix);
                    RotMatrixY(i * 700 + j * 500, &unit.matrix);
                    unit.matrix.t[0] = work->rings[i][j].position.vx;
                    unit.matrix.t[1] = work->rings[i][j].position.vy;
                    unit.matrix.t[2] = work->rings[i][j].position.vz;
                    unit.super = work->rings[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (k = 0; k < 16; k++) {
                        DRAW_STRIP(work->rings[i][j], &work->ring_inner[k], &work->ring_inner[k + 1],
                            &work->ring_middle[k], &work->ring_middle[k + 1]);
                        DRAW_STRIP(work->rings[i][j], &work->ring_outer[k], &work->ring_outer[k + 1],
                            &work->ring_middle[k], &work->ring_middle[k + 1]);
                        if (work->command == 1) {
                            DRAW_STRIP(work->rings[i][j], &work->cone_inner[k], &work->cone_inner[k + 1],
                                &work->cone_middle[k], &work->cone_middle[k + 1]);
                            DRAW_STRIP(work->rings[i][j], &work->cone_outer[k], &work->cone_outer[k + 1],
                                &work->cone_middle[k], &work->cone_middle[k + 1]);
                        }
                    }
                    if (work->rings[i][j].u < 7) {
                        work->rings[i][j].u++;
                    }
                    if (work->rings[i][j].v < 224) {
                        work->rings[i][j].v += 32;
                    }
                    work->rings[i][j].elapsed += step;
                    if (work->rings[i][j].duration < work->rings[i][j].elapsed) {
                        work->rings[i][j].active = 0;
                    }
                    result = 4;
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                if (work->bursts[i][j].active == 1) {
                    value = (work->bursts[i][j].elapsed << 12) / work->bursts[i][j].duration + 4096;
                    setVector(&work->bursts[i][j].scale, value, value, value);
                    FADE(work->bursts[i][j], 3);
                    for (k = 0; k < 5; k++) {
                        unit.flg = 0;
                        unit.matrix = D_800FE168;
                        ScaleMatrix(&unit.matrix, &work->bursts[i][j].scale);
                        RotMatrixZ(work->bursts[i][j].rotation.vz, &unit.matrix);
                        RotMatrixX(work->bursts[i][j].rotation.vx - (view.pitch - 4096) * work->direction,
                            &unit.matrix);
                        RotMatrixY(work->bursts[i][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                        unit.matrix.t[0] = work->bursts[i][j].position.vx +
                            work->offsets[k].vx * work->bursts[i][j].elapsed / work->bursts[i][j].duration;
                        unit.matrix.t[1] = work->bursts[i][j].position.vy +
                            work->offsets[k].vy * work->bursts[i][j].elapsed / work->bursts[i][j].duration;
                        unit.matrix.t[2] = work->bursts[i][j].position.vz +
                            work->offsets[k].vz * work->bursts[i][j].elapsed / work->bursts[i][j].duration;
                        unit.super = work->bursts[i][j].coord;
                        GsGetLsUnit(&unit, &ls);
                        GsSetLsMatrix(&ls);
                        DRAW_SPRITE(2, work->bursts[i][j].u, work->bursts[i][j].v, work->bursts[i][j].u + 31,
                            work->bursts[i][j].v, work->bursts[i][j].u, work->bursts[i][j].v + 31,
                            work->bursts[i][j].u + 31, work->bursts[i][j].v + 31, work->bursts[i][j],
                            &work->burst_corners[0], &work->burst_corners[1], &work->burst_corners[2],
                            &work->burst_corners[3]);
                    }
                    if (work->bursts[i][j].u < 224) {
                        work->bursts[i][j].u += 32;
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
        work->elapsed += step;
        if (inactive == (work->column_count * 2 + work->column_count) * work->row_count) {
            result = 1;
            if (work->flags[4] == 1) {
                result = 2;
            }
            work->flags[4] = 1;
        }
    }
    return result;
}
