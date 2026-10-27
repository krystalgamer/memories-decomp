#include "../../types.h"
#include "variant681_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 681's entry: one or two flickering flashes that slide toward the
 * model, rings of Gouraud strips and spinning burst sprites released from
 * model points, a spinning dome of sprites wrapped in a cone of strips, and
 * rows of sparks that drift away. The setup also prints the work-area size. */

/* The work-size format string; retail's padding byte after the terminator is 0xA7. */
#ifndef VERSION_FRENCH
static const char D_8013EE20[24] = "E_DATA size = %x byte\n\0\xA7";
#endif

#define TRACK_POINT(index, matrix)                                             \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->slot, index);          \
    GsGetLwUnit(&unit, &matrix)

#define START_FLASHES(index)                                                   \
    if (work->elapsed == 0) {                                                  \
        for (i = 0; i < work->flash_count; i++) {                              \
            work->flashes[i].active = 1;                                       \
            TRACK_POINT(index, ls);                                            \
            setVector(&work->flashes[i].position, ls.t[0], ls.t[1], ls.t[2]); \
        }                                                                      \
    }

#define START_RING(i, index)                                                   \
    work->rings[0][i].active = 1;                                              \
    TRACK_POINT(index, ls);                                                    \
    setVector(&work->rings[0][i].position, ls.t[0], ls.t[1], ls.t[2]);        \
    work->bursts[0][i].active = 1;                                             \
    work->bursts[0][i].position = work->rings[0][i].position

#define EMIT_SPARKS(timer, depth)                                              \
    if (work->flags[timer] >= 5) {                                             \
        TRACK_POINT(15, ls);                                                   \
        for (i = 0; i < work->spark_columns; i++) {                            \
            work->sparks[work->flags[1]][i].active = 1;                        \
            work->sparks[work->flags[1]][i].elapsed = 0;                       \
            work->sparks[work->flags[1]][i].color[0] = D_8013EF18[work->command].color[3]; \
            work->sparks[work->flags[1]][i].color[1] = D_8013EF18[work->command].color[4]; \
            work->sparks[work->flags[1]][i].color[2] = D_8013EF18[work->command].color[5]; \
            work->sparks[work->flags[1]][i].u =                                \
                (i * 5 + work->flags[1] * 3 + work->elapsed * 2) % 8 << 5;     \
            /* (i * 2 - 1) * 80 keeps the -80 in the induction start value. */ \
            setVector(&work->sparks[work->flags[1]][i].position, ls.t[0] + (i * 2 - 1) * 80, \
                ls.t[1] + 50, ls.t[2] + (depth));                              \
        }                                                                      \
        work->flags[1]++;                                                      \
        if (work->flags[1] >= work->spark_rows) {                              \
            work->flags[1] = 0;                                                \
        }                                                                      \
        work->flags[timer] = 0;                                                \
    }                                                                          \
    work->flags[timer] += step

#define FADE(element, first)                                                   \
    if ((element).duration / 2 < (element).elapsed) {                          \
        value = ((element).duration - (element).elapsed) * 2;                  \
        amount = (element).duration;                                           \
        (element).color[0] = D_8013EF18[work->command].color[first] * value / amount; \
        (element).color[1] = D_8013EF18[work->command].color[first + 1] * value / amount; \
        (element).color[2] = D_8013EF18[work->command].color[first + 2] * value / amount; \
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

#define SORT_STRIP(a, b, c, d)                                                 \
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
    (element).color[0] = D_8013EF18[command].color[first];                     \
    (element).color[1] = D_8013EF18[command].color[first + 1];                 \
    (element).color[2] = D_8013EF18[command].color[first + 2]

s32 func_8013B004(u8 *context, s32 command)
{
    Model681State *work;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    Model681ViewSnapshot view;
    VECTOR vec;
    /* Only two bytes are used; the size keeps the original frame layout. */
    u8 uv[8];
    PSXLONG interpolation, flag;
    /* j sits between result and step to keep the spill-slot order. */
    s32 result, j, step, inactive;
    s32 i, k, value, amount, scale;
    u32 tex;
    PSXLONG depth;

    work = (Model681State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model681State));
#ifndef VERSION_FRENCH
        printf(D_8013EE20, sizeof(Model681State));
#endif
        work->command = command;
        work->slot = Model_GetActiveSlotIndex();
        work->direction = work->slot * 2 - 1;
        work->elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013EE38[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013EE38[4]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013EE38[1]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013EE38[2]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013EE38[3]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013EE38[5]);
        work->tpage[4] = tex >> 16;
        work->clut[4] = tex;
        ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013EE38[6]);
        tex = ModelTexture_PackPageClutCopy(work->slot, 1, &D_8013EE38[7]);
        work->tpage[5] = tex >> 16;
        work->clut[5] = tex;
        switch (command) {
        case 0:
        case 2:
        case 3:
            work->flash_count = 1;
            work->column_count = 2;
            work->row_count = 1;
            work->spark_columns = 0;
            work->spark_rows = 0;
            break;
        case 1:
            work->flash_count = 1;
            work->column_count = 3;
            work->row_count = 1;
            work->spark_columns = 2;
            work->spark_rows = 32;
            break;
        }
        work->radius = D_8013EF18[command].radius;
        work->height = D_8013EF18[command].height;
        /* amount rather than value: it keeps value in s4 and amount in s3. */
        amount = work->radius;
        setVector(&vec, amount, 0, amount * -work->direction);
        for (i = 0; i < 5; i++) {
            PushMatrix();
            m = D_800FE168;
            switch (command) {
            case 0:
            case 3:
                RotMatrixZ(i * 4096 / 5, &m);
                break;
            case 1:
                RotMatrixZ(i * 4096 / 5, &m);
                RotMatrixX(work->direction * 910, &m);
                break;
            case 2:
                RotMatrixZ(i * 4096 / 5, &m);
                RotMatrixX(work->direction * 512, &m);
                break;
            }
            ApplyMatrixLV(&m, &vec, &work->offsets[i]);
            PopMatrix();
        }
        for (i = 0; i < work->flash_count; i++) {
            work->flashes[i].active = 0;
            work->flashes[i].elapsed = 0;
            switch (command) {
            case 0:
                work->flashes[i].duration = 128;
                break;
            case 1:
                work->flashes[i].duration = 280;
                break;
            case 2:
                work->flashes[i].duration = 200;
                break;
            case 3:
                work->flashes[i].duration = 120;
                break;
            }
            work->flashes[i].mode = 1;
            work->flashes[i].coord = Model_GetCameraCoordinateUnit();
            work->flashes[i].color[0] = D_8013EF18[command].color[0];
            work->flashes[i].color[1] = D_8013EF18[command].color[1];
            work->flashes[i].color[2] = D_8013EF18[command].color[2];
            work->flashes[i].u = 0;
            work->flashes[i].v = 0;
            setVector(&work->flashes[i].scale, 4096, 4096, 4096);
            setVector(&work->flashes[i].target, 0, 0, work->direction * 450);
            setVector(&work->flashes[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->flashes[i].position, 0, 0, 0);
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                INIT_ELEMENT(work->rings[i][j], 32, 3);
                work->rings[i][j].u = 0;
                work->rings[i][j].v = 128;
                setVector(&work->rings[i][j].scale, 4096, 4096, 4096);
                setVector(&work->rings[i][j].target, 0, 0, 0);
                switch (command) {
                case 1:
                    setVector(&work->rings[i][j].rotation, -910, work->slot * 2048, 0);
                    break;
                case 2:
                    setVector(&work->rings[i][j].rotation, -512, work->slot * 2048, 0);
                    break;
                default:
                    setVector(&work->rings[i][j].rotation, 0, work->slot * 2048, 0);
                    break;
                }
                setVector(&work->rings[i][j].position, 0, 0, 0);
            }
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                INIT_ELEMENT(work->bursts[i][j], 48, 3);
                work->bursts[i][j].u = 0;
                work->bursts[i][j].v = 64;
                setVector(&work->bursts[i][j].scale, 4096, 4096, 4096);
                setVector(&work->bursts[i][j].target, 0, 0, 0);
                setVector(&work->bursts[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->bursts[i][j].position, 0, 0, 0);
            }
        }
        for (i = 0; i < work->row_count; i++) {
            INIT_ELEMENT(work->domes[i], 96, 6);
            work->domes[i].u = 32;
            work->domes[i].v = 128;
            setVector(&work->domes[i].scale, 4096, 4096, 4096);
            setVector(&work->domes[i].target, 0, 0, 0);
            setVector(&work->domes[i].rotation, 0, work->slot * 2048, 0);
            setVector(&work->domes[i].position, 0, -350, work->direction * 450);
        }
        for (j = 0; j < work->spark_columns; j++) {
            for (i = 0; i < work->spark_rows; i++) {
                INIT_ELEMENT(work->sparks[i][j], 64, 3);
                work->sparks[i][j].u = 0;
                work->sparks[i][j].v = 64;
                setVector(&work->sparks[i][j].scale, 4096, 4096, 4096);
                setVector(&work->sparks[i][j].target, 0, 0, 0);
                setVector(&work->sparks[i][j].rotation, 0, work->slot * 2048, 0);
                setVector(&work->sparks[i][j].position, 0, 0, 0);
            }
        }
        value = work->radius;
        setVector(&work->corners[0], -value, -value, 0);
        setVector(&work->corners[1], value, -value, 0);
        setVector(&work->corners[2], -value, value, 0);
        setVector(&work->corners[3], value, value, 0);
        value = work->radius / 5;
        amount = work->radius * 2 / 3;
        for (i = 0; i < 17; i++) {
            setVector(&work->ring[i], value * rsin(i << 8) / 4096, value * rcos(i << 8) / 4096, 0);
            setVector(&work->rim[i], amount * rsin(i << 8) / 4096, amount * rcos(i << 8) / 4096,
                -work->height / 2);
        }
        for (i = 0; i < 8; i++) {
            if (i == 0) {
                value = work->radius / 6 * rcos(0) / 4096;
                amount = work->radius / 2 * rcos(128) / 4096;
                /* Multiplied, then negated, as the original does. */
                scale = work->height * -3;
            } else {
                value = work->radius / 8 * rcos(i << 7) / 4096;
                amount = work->radius / 4 * rcos((i + 1) << 7) / 4096;
                scale = work->height * -(rsin((i + 1) << 7) * 3) / 4096;
            }
            for (j = 0; j < 6; j++) {
                setVector(&work->inner[j][i], value * rsin(j * 4096 / 5) / 4096,
                    value * rcos(j * 4096 / 5) / 4096, 0);
                setVector(&work->outer[j][i], amount * rsin(j * 4096 / 5) / 4096,
                    amount * rcos(j * 4096 / 5) / 4096, scale);
            }
        }
        value = work->radius / 3;
        setVector(&work->burst_corners[0], -value, -value, 0);
        setVector(&work->burst_corners[1], value, -value, 0);
        setVector(&work->burst_corners[2], -value, value, 0);
        setVector(&work->burst_corners[3], value, value, 0);
        value = work->radius;
        scale = value * value;
        for (i = 0; i < 9; i++) {
            for (j = 0; j < 17; j++) {
                setVector(&work->dome[j][i],
                    SquareRoot0(scale - value * rcos(i << 8) / 4096 * (value * rcos(i << 8) / 4096)) *
                        rsin(j << 8) / 4096,
                    value * rcos(i << 8) / 4096,
                    SquareRoot0(scale - value * rcos(i << 8) / 4096 * (value * rcos(i << 8) / 4096)) *
                        rcos(j << 8) / 4096);
            }
        }
        value = work->radius / 2;
        amount = work->radius * 2;
        for (i = 0; i < 9; i++) {
            setVector(&work->cone_inner[i], value * rsin(i << 9) / 4096, value * rcos(i << 9) / 4096, 0);
            setVector(&work->cone_outer[i], amount * rsin(i << 9) / 4096, amount * rcos(i << 9) / 4096, 0);
            setVector(&work->cone_middle[i], (work->cone_inner[i].vx + work->cone_outer[i].vx) / 2,
                (work->cone_inner[i].vy + work->cone_outer[i].vy) / 2,
                (work->cone_inner[i].vz + work->cone_outer[i].vz) / 2);
        }
        value = work->radius / 4;
        setVector(&work->spark_corners[0], -value, -value, 0);
        setVector(&work->spark_corners[1], value, -value, 0);
        setVector(&work->spark_corners[2], -value, value, 0);
        setVector(&work->spark_corners[3], value, value, 0);
        work->delay = D_8013EF18[command].delay;
        for (i = 0; i < 6; i++) {
            work->flags[i] = 0;
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->command) {
        case 0:
            if (work->elapsed == 0) {
                for (i = 0; i < work->flash_count; i++) {
                    work->flashes[i].active = 1;
                    TRACK_POINT(8, ls);
                    TRACK_POINT(15, m);
                    setVector(&work->flashes[i].position, (ls.t[0] + m.t[0]) / 2, (ls.t[1] + m.t[1]) / 2,
                        (ls.t[2] + m.t[2]) / 2);
                }
            }
            if (work->elapsed >= 67 && work->flags[0] == 0) {
                for (i = 0; i < work->column_count; i++) {
                    START_RING(i, (i + 1) * 7);
                }
                work->flags[0] = 1;
            }
            if (work->elapsed >= 83 && work->flags[5] == 0) {
                work->domes[0].active = 1;
                work->flags[5] = 1;
            }
            break;
        case 1:
            START_FLASHES(3);
            if (work->elapsed >= 179 && work->flags[0] == 0) {
                for (i = 0; i < work->column_count; i++) {
                    START_RING(i, i + 8);
                }
                work->flags[0] = 1;
            }
            if (work->elapsed < 40) {
                EMIT_SPARKS(2, work->direction * 90);
            }
            if (work->elapsed >= 277 && work->elapsed < 320) {
                EMIT_SPARKS(3, -(work->direction * 110));
            }
            if (work->elapsed >= 251 && work->flags[5] == 0) {
                work->domes[0].active = 1;
                work->flags[5] = 1;
            }
            break;
        case 2:
            START_FLASHES(6);
            if (work->elapsed >= 127 && work->flags[0] == 0) {
                for (i = 0; i < work->column_count; i++) {
                    START_RING(i, i + 19);
                }
                work->flags[0] = 1;
            }
            if (work->elapsed >= 167 && work->flags[5] == 0) {
                work->domes[0].active = 1;
                work->flags[5] = 1;
            }
            break;
        case 3:
            START_FLASHES(3);
            if (work->elapsed >= 61 && work->flags[0] == 0) {
                for (i = 0; i < work->column_count; i++) {
                    START_RING(i, i * 3 + 16);
                }
                work->flags[0] = 1;
            }
            if (work->elapsed >= 81 && work->flags[5] == 0) {
                work->domes[0].active = 1;
                work->flags[5] = 1;
            }
            break;
        }
        PushMatrix();
        view = *(Model681ViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->flash_count; i++) {
            if (work->flashes[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                /* An if/else rather than a preset 4096: it keeps the /5
                 * constant inside the loop and the scale address unshared. */
                if (work->flashes[i].elapsed < work->flashes[i].duration / 5) {
                    value = work->flashes[i].elapsed * 0x5000 / work->flashes[i].duration;
                } else {
                    value = 4096;
                }
                if (!(work->flashes[i].rotation.vx & 1)) {
                    value /= 2;
                }
                setVector(&work->flashes[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->flashes[i].scale);
                RotMatrixY(work->flashes[i].rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->flashes[i].position.vx;
                unit.matrix.t[1] = work->flashes[i].position.vy;
                if (work->flashes[i].elapsed < work->flashes[i].duration / 5) {
                    /* The shift spelling of elapsed * 5 stops GCC moving the 5 onto
                     * the distance. */
                    unit.matrix.t[2] = work->flashes[i].position.vz +
                        (work->flashes[i].target.vz - work->flashes[i].position.vz) *
                            ((work->flashes[i].elapsed << 2) + work->flashes[i].elapsed) /
                            work->flashes[i].duration;
                } else {
                    unit.matrix.t[2] = work->flashes[i].target.vz;
                }
                unit.super = work->flashes[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                DRAW_SPRITE(0, work->flashes[i].u, work->flashes[i].v, work->flashes[i].u + 63, work->flashes[i].v,
                    work->flashes[i].u, work->flashes[i].v + 63, work->flashes[i].u + 63, work->flashes[i].v + 63,
                    work->flashes[i], &work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3]);
                work->flashes[i].rotation.vx++;
                work->flashes[i].elapsed += step;
                if (work->flashes[i].duration < work->flashes[i].elapsed) {
                    work->flashes[i].active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                if (work->rings[i][j].active == 1) {
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    value = work->rings[i][j].elapsed * 0x3000 / work->rings[i][j].duration;
                    setVector(&work->rings[i][j].scale, value, value, value);
                    ScaleMatrix(&unit.matrix, &work->rings[i][j].scale);
                    RotMatrixX(work->rings[i][j].rotation.vx, &unit.matrix);
                    RotMatrixY(work->rings[i][j].rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->rings[i][j].position.vx;
                    unit.matrix.t[1] = work->rings[i][j].position.vy;
                    unit.matrix.t[2] = work->rings[i][j].position.vz;
                    unit.super = work->rings[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    FADE(work->rings[i][j], 3);
                    for (k = 0; k < 16; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[1];
                        work->strip.clut = work->clut[1];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->rings[i][j].u, work->rings[i][j].v,
                            work->rings[i][j].u + 31, work->rings[i][j].v,
                            work->rings[i][j].u, work->rings[i][j].v + 127,
                            work->rings[i][j].u + 31, work->rings[i][j].v + 127);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        setRGB2(&work->strip, work->rings[i][j].color[0], work->rings[i][j].color[1],
                            work->rings[i][j].color[2]);
                        setRGB3(&work->strip, work->rings[i][j].color[0], work->rings[i][j].color[1],
                            work->rings[i][j].color[2]);
                        SORT_STRIP(&work->ring[k], &work->ring[k + 1], &work->rim[k], &work->rim[k + 1]);
                    }
                    /* The band index reuses amount and the texture index value,
                     * which keeps their registers. */
                    for (amount = 0; amount < 8; amount++) {
                        value = 1;
                        if (amount == 0) {
                            value = 2;
                        }
                        for (k = 0; k < 5; k++) {
                            setPolyGT4(&work->strip);
                            work->strip.tpage = work->tpage[value];
                            work->strip.clut = work->clut[value];
                            setSemiTrans(&work->strip, 1);
                            if (value == 1) {
                                setUV4(&work->strip, work->rings[i][j].u, work->rings[i][j].v,
                                    work->rings[i][j].u + 31, work->rings[i][j].v,
                                    work->rings[i][j].u, work->rings[i][j].v + 127,
                                    work->rings[i][j].u + 31, work->rings[i][j].v + 127);
                            } else {
                                setUV4(&work->strip, 64, 0, 127, 0, 64, 63, 127, 63);
                            }
                            setRGB2(&work->strip, 0, 0, 0);
                            setRGB3(&work->strip, 0, 0, 0);
                            setRGB0(&work->strip, work->rings[i][j].color[0], work->rings[i][j].color[1],
                                work->rings[i][j].color[2]);
                            setRGB1(&work->strip, work->rings[i][j].color[0], work->rings[i][j].color[1],
                                work->rings[i][j].color[2]);
                            SORT_STRIP(&work->inner[k][amount], &work->inner[k + 1][amount], &work->outer[k][amount],
                                &work->outer[k + 1][amount]);
                        }
                    }
                    work->rings[i][j].elapsed += step;
                    if (work->rings[i][j].duration < work->rings[i][j].elapsed) {
                        work->rings[i][j].active = 0;
                    }
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
                        DRAW_SPRITE(3, work->bursts[i][j].u, work->bursts[i][j].v, work->bursts[i][j].u + 31,
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
        for (i = 0; i < work->row_count; i++) {
            if (work->domes[i].active == 1) {
                unit.flg = 0;
                unit.matrix = D_800FE168;
                value = (work->domes[i].elapsed << 14) / work->domes[i].duration + 0x800;
                setVector(&work->domes[i].scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->domes[i].scale);
                RotMatrixZ(work->domes[i].elapsed * 50, &unit.matrix);
                RotMatrixX(work->domes[i].elapsed * 60, &unit.matrix);
                RotMatrixY(work->domes[i].elapsed * 70, &unit.matrix);
                unit.matrix.t[0] = work->domes[i].position.vx;
                unit.matrix.t[1] = work->domes[i].position.vy;
                unit.matrix.t[2] = work->domes[i].position.vz;
                unit.super = work->domes[i].coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->domes[i].duration / 2 < work->domes[i].elapsed) {
                    FADE(work->domes[i], 6);
                } else {
                    work->domes[i].color[0] = D_8013EF18[work->command].color[6];
                    work->domes[i].color[1] = D_8013EF18[work->command].color[7];
                    work->domes[i].color[2] = D_8013EF18[work->command].color[8];
                }
                work->domes[i].v = (work->domes[i].elapsed & 0xF) * 63 / 16 - 128;
                for (j = 0; j < 8; j++) {
                    for (k = 0; k < 16; k++) {
                        setPolyFT4(&work->sprite);
                        work->sprite.tpage = work->tpage[5];
                        work->sprite.clut = work->clut[5];
                        setSemiTrans(&work->sprite, 1);
                        uv[0] = work->domes[i].u + k % 2 * 63 / 2 + 64;
                        uv[1] = work->domes[i].u + (k % 2 + 1) * 63 / 2 + 64;
                        setUV4(&work->sprite, uv[0], work->domes[i].v, uv[1], work->domes[i].v, uv[0],
                            work->domes[i].v + 64, uv[1], work->domes[i].v + 64);
                        setRGB0(&work->sprite, work->domes[i].color[0], work->domes[i].color[1],
                            work->domes[i].color[2]);
                        depth = RotAverage4(&work->dome[k][j], &work->dome[k + 1][j], &work->dome[k][j + 1],
                            &work->dome[k + 1][j + 1], (PSXLONG *)&work->sprite.x0, (PSXLONG *)&work->sprite.x1,
                            (PSXLONG *)&work->sprite.x2, (PSXLONG *)&work->sprite.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->sprite, func_80058F10(), (u16)depth);
                        }
                    }
                }
                if (work->domes[i].duration / 2 < work->domes[i].elapsed) {
                    FADE(work->domes[i], 3);
                } else {
                    work->domes[i].color[0] = D_8013EF18[work->command].color[3];
                    work->domes[i].color[1] = D_8013EF18[work->command].color[4];
                    work->domes[i].color[2] = D_8013EF18[work->command].color[5];
                }
                for (j = 0; j < 8; j++) {
                    work->domes[i].v = (work->domes[i].elapsed + j * 10) % 32 * 63 / 32 - 128;
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->domes[i].scale);
                    /* The sign and base angle reuse value and amount. */
                    value = j % 2 * 2 - 1;
                    amount = j << 9;
                    RotMatrixZ(value * work->domes[i].elapsed * 5 + amount, &unit.matrix);
                    RotMatrixX(value * work->domes[i].elapsed * 6 + amount, &unit.matrix);
                    RotMatrixY(value * work->domes[i].elapsed * 7 + amount, &unit.matrix);
                    unit.matrix.t[0] = work->domes[i].position.vx;
                    unit.matrix.t[1] = work->domes[i].position.vy;
                    unit.matrix.t[2] = work->domes[i].position.vz;
                    unit.super = work->domes[i].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (k = 0; k < 8; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[4];
                        work->strip.clut = work->clut[4];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->domes[i].u, work->domes[i].v + 63, work->domes[i].u + 63,
                            work->domes[i].v + 63, work->domes[i].u, work->domes[i].v, work->domes[i].u + 63,
                            work->domes[i].v);
                        setRGB2(&work->strip, work->domes[i].color[0], work->domes[i].color[1],
                            work->domes[i].color[2]);
                        setRGB3(&work->strip, work->domes[i].color[0], work->domes[i].color[1],
                            work->domes[i].color[2]);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        SORT_STRIP(&work->cone_inner[k], &work->cone_inner[k + 1], &work->cone_middle[k],
                            &work->cone_middle[k + 1]);
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[4];
                        work->strip.clut = work->clut[4];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->domes[i].u, work->domes[i].v + 63, work->domes[i].u + 63,
                            work->domes[i].v + 63, work->domes[i].u, work->domes[i].v, work->domes[i].u + 63,
                            work->domes[i].v);
                        setRGB0(&work->strip, work->domes[i].color[0], work->domes[i].color[1],
                            work->domes[i].color[2]);
                        setRGB1(&work->strip, work->domes[i].color[0], work->domes[i].color[1],
                            work->domes[i].color[2]);
                        setRGB2(&work->strip, 0, 0, 0);
                        setRGB3(&work->strip, 0, 0, 0);
                        SORT_STRIP(&work->cone_middle[k], &work->cone_middle[k + 1], &work->cone_outer[k],
                            &work->cone_outer[k + 1]);
                    }
                }
                result = 4;
                work->domes[i].elapsed += step;
                if (work->domes[i].duration < work->domes[i].elapsed) {
                    work->domes[i].active = 0;
                    if (i == work->row_count - 1) {
                        result = 1;
                    }
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->spark_columns; j++) {
            for (i = 0; i < work->spark_rows; i++) {
                if (work->sparks[i][j].active == 1) {
                    value = (work->sparks[i][j].elapsed << 12) / work->sparks[i][j].duration + 4096;
                    setVector(&work->sparks[i][j].scale, value, value, value);
                    FADE(work->sparks[i][j], 3);
                    unit.flg = 0;
                    unit.matrix = D_800FE168;
                    ScaleMatrix(&unit.matrix, &work->sparks[i][j].scale);
                    RotMatrixZ(work->sparks[i][j].rotation.vz, &unit.matrix);
                    RotMatrixX(work->sparks[i][j].rotation.vx - (view.pitch - 4096) * work->direction,
                        &unit.matrix);
                    RotMatrixY(work->sparks[i][j].rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][j].position.vx;
                    unit.matrix.t[1] = work->sparks[i][j].position.vy;
                    unit.matrix.t[2] = work->sparks[i][j].position.vz;
                    unit.super = work->sparks[i][j].coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    DRAW_SPRITE(3, work->sparks[i][j].u, work->sparks[i][j].v, work->sparks[i][j].u + 31,
                        work->sparks[i][j].v, work->sparks[i][j].u, work->sparks[i][j].v + 31,
                        work->sparks[i][j].u + 31, work->sparks[i][j].v + 31, work->sparks[i][j],
                        &work->spark_corners[0], &work->spark_corners[1], &work->spark_corners[2],
                        &work->spark_corners[3]);
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
        if (inactive == work->flash_count + work->column_count * work->row_count * 2 + work->row_count +
                work->spark_columns * work->spark_rows) {
            result = 2;
        }
    }
    return result;
}
