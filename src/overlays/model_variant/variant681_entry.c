#include "../../types.h"
#include "variant681_entry.h"
#include "../../psyq/memory.h"
#include "../../psyq/stdio.h"

/* Header 681's entry: one or two flickering flashes that slide toward the
 * model, rings of Gouraud strips and spinning burst sprites released from
 * model points, a spinning dome of sprites wrapped in a cone of strips, and
 * rows of sparks that drift away. The setup also prints the work-area size. */

/* The work-size format string. The padding byte after its terminator is 0xA7
 * in the slot-zero images and 0 in the slot-one copies. */
#ifndef MODEL681_STRING_PAD
#define MODEL681_STRING_PAD "\xA7"
#endif
#ifndef VERSION_FRENCH
static const char gModelVariant681_szWorkSizeFormat[24] = "E_DATA size = %x byte\n\0" MODEL681_STRING_PAD;
#endif

#define TRACK_POINT(index, matrix)                                             \
    unit = *(GsCOORDUNIT *)Model_GetSlotDataEntry(work->head.slot, index);          \
    GsGetLwUnit(&unit, &matrix)

#define START_FLASHES(index)                                                   \
    if (work->head.elapsed == 0) {                                                  \
        for (i = 0; i < work->flash_count; i++) {                              \
            work->flashes[i].head.active = 1;                                       \
            TRACK_POINT(index, ls);                                            \
            setVector(&work->flashes[i].xform.position, ls.t[0], ls.t[1], ls.t[2]); \
        }                                                                      \
    }

#define START_RING(i, index)                                                   \
    work->rings[0][i].head.active = 1;                                              \
    TRACK_POINT(index, ls);                                                    \
    setVector(&work->rings[0][i].xform.position, ls.t[0], ls.t[1], ls.t[2]);        \
    work->bursts[0][i].head.active = 1;                                             \
    work->bursts[0][i].xform.position = work->rings[0][i].xform.position

#define EMIT_SPARKS(timer, depth)                                              \
    if (work->flags[timer] >= 5) {                                             \
        TRACK_POINT(15, ls);                                                   \
        for (i = 0; i < work->spark_columns; i++) {                            \
            work->sparks[work->flags[1]][i].head.active = 1;                        \
            work->sparks[work->flags[1]][i].head.elapsed = 0;                       \
            work->sparks[work->flags[1]][i].head.color[0] = gModelVariant681_aConfigs[work->head.command].color[3]; \
            work->sparks[work->flags[1]][i].head.color[1] = gModelVariant681_aConfigs[work->head.command].color[4]; \
            work->sparks[work->flags[1]][i].head.color[2] = gModelVariant681_aConfigs[work->head.command].color[5]; \
            work->sparks[work->flags[1]][i].head.u =                                \
                (i * 5 + work->flags[1] * 3 + work->head.elapsed * 2) % 8 << 5;     \
            /* (i * 2 - 1) * 80 keeps the -80 in the induction start value. */ \
            setVector(&work->sparks[work->flags[1]][i].xform.position, ls.t[0] + (i * 2 - 1) * 80, \
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
    if ((element).head.duration / 2 < (element).head.elapsed) {                          \
        value = ((element).head.duration - (element).head.elapsed) * 2;                  \
        amount = (element).head.duration;                                           \
        (element).head.color[0] = gModelVariant681_aConfigs[work->head.command].color[first] * value / amount; \
        (element).head.color[1] = gModelVariant681_aConfigs[work->head.command].color[first + 1] * value / amount; \
        (element).head.color[2] = gModelVariant681_aConfigs[work->head.command].color[first + 2] * value / amount; \
    }

#define DRAW_SPRITE(page, u0, v0, u1, v1, u2, v2, u3, v3, element, a, b, c, d)   \
    setPolyFT4(&work->sprite);                                                 \
    work->sprite.tpage = work->tpage[page];                                    \
    work->sprite.clut = work->clut[page];                                      \
    setSemiTrans(&work->sprite, 1);                                            \
    setUV4(&work->sprite, u0, v0, u1, v1, u2, v2, u3, v3);                     \
    setRGB0(&work->sprite, (element).head.color[0], (element).head.color[1], (element).head.color[2]); \
    depth = RotAverage4(a, b, c, d, (long *)&work->sprite.x0, (long *)&work->sprite.x1, \
        (long *)&work->sprite.x2, (long *)&work->sprite.x3, &interpolation, &flag); \
    if (flag >= 0) {                                                           \
        GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);                \
    }

#define SORT_STRIP(a, b, c, d)                                                 \
    depth = RotAverage4(a, b, c, d, (long *)&work->strip.x0, (long *)&work->strip.x1, \
        (long *)&work->strip.x2, (long *)&work->strip.x3, &interpolation, &flag); \
    if (flag >= 0) {                                                           \
        GsSortPoly(&work->strip, Graphics_GetDefaultOrderingTable(), (u16)depth);                 \
    }

#define INIT_ELEMENT(element, length, first)                                   \
    (element).head.active = 0;                                                      \
    (element).head.elapsed = 0;                                                     \
    (element).head.duration = length;                                               \
    (element).head.mode = 1;                                                        \
    (element).xform.coord = Model_GetCameraCoordinateUnit();                         \
    (element).head.color[0] = gModelVariant681_aConfigs[command].color[first];                     \
    (element).head.color[1] = gModelVariant681_aConfigs[command].color[first + 1];                 \
    (element).head.color[2] = gModelVariant681_aConfigs[command].color[first + 2]

s32 ModelVariant681_HandleRequest(u8 *context, s32 command)
{
    Model681State *work;
    MATRIX ls;
    MATRIX m;
    GsCOORDUNIT unit;
    ModelVariantViewSnapshot view;
    VECTOR vec;
    /* Only two bytes are used; the size keeps the original frame layout. */
    u8 uv[8];
    long interpolation, flag;
    /* j sits between result and step to keep the spill-slot order. */
    s32 result, j, step, inactive;
    s32 i, k, value, amount, scale;
    u32 tex;
    long depth;

    work = (Model681State *)context;
    result = 0;
    step = Model_GetFrameStep();
    if (command >= 0) {
        bzero((u8 *)work, sizeof(Model681State));
#ifndef VERSION_FRENCH
        printf(gModelVariant681_szWorkSizeFormat, sizeof(Model681State));
#endif
        work->head.command = command;
        work->head.slot = Model_GetActiveSlotIndex();
        work->direction = work->head.slot * 2 - 1;
        work->head.elapsed = 0;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant681_aImages[0]);
        work->tpage[0] = tex >> 16;
        work->clut[0] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant681_aImages[4]);
        work->tpage[1] = tex >> 16;
        work->clut[1] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant681_aImages[1]);
        work->tpage[2] = tex >> 16;
        work->clut[2] = tex;
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant681_aImages[2]);
        work->tpage[3] = tex >> 16;
        work->clut[3] = tex;
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant681_aImages[3]);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant681_aImages[5]);
        work->tpage[4] = tex >> 16;
        work->clut[4] = tex;
        ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant681_aImages[6]);
        tex = ModelTexture_PackPageClutCopy(work->head.slot, 1, &gModelVariant681_aImages[7]);
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
        work->radius = gModelVariant681_aConfigs[command].radius;
        work->height = gModelVariant681_aConfigs[command].height;
        /* amount rather than value: it keeps value in s4 and amount in s3. */
        amount = work->radius;
        setVector(&vec, amount, 0, amount * -work->direction);
        for (i = 0; i < 5; i++) {
            PushMatrix();
            m = GsIDMATRIX;
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
            work->flashes[i].head.active = 0;
            work->flashes[i].head.elapsed = 0;
            switch (command) {
            case 0:
                work->flashes[i].head.duration = 128;
                break;
            case 1:
                work->flashes[i].head.duration = 280;
                break;
            case 2:
                work->flashes[i].head.duration = 200;
                break;
            case 3:
                work->flashes[i].head.duration = 120;
                break;
            }
            work->flashes[i].head.mode = 1;
            work->flashes[i].xform.coord = Model_GetCameraCoordinateUnit();
            work->flashes[i].head.color[0] = gModelVariant681_aConfigs[command].color[0];
            work->flashes[i].head.color[1] = gModelVariant681_aConfigs[command].color[1];
            work->flashes[i].head.color[2] = gModelVariant681_aConfigs[command].color[2];
            work->flashes[i].head.u = 0;
            work->flashes[i].head.v = 0;
            setVector(&work->flashes[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->flashes[i].target, 0, 0, work->direction * 450);
            setVector(&work->flashes[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->flashes[i].xform.position, 0, 0, 0);
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                INIT_ELEMENT(work->rings[i][j], 32, 3);
                work->rings[i][j].head.u = 0;
                work->rings[i][j].head.v = 128;
                setVector(&work->rings[i][j].xform.scale, 4096, 4096, 4096);
                setVector(&work->rings[i][j].target, 0, 0, 0);
                switch (command) {
                case 1:
                    setVector(&work->rings[i][j].xform.rotation, -910, work->head.slot * 2048, 0);
                    break;
                case 2:
                    setVector(&work->rings[i][j].xform.rotation, -512, work->head.slot * 2048, 0);
                    break;
                default:
                    setVector(&work->rings[i][j].xform.rotation, 0, work->head.slot * 2048, 0);
                    break;
                }
                setVector(&work->rings[i][j].xform.position, 0, 0, 0);
            }
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                INIT_ELEMENT(work->bursts[i][j], 48, 3);
                work->bursts[i][j].head.u = 0;
                work->bursts[i][j].head.v = 64;
                setVector(&work->bursts[i][j].xform.scale, 4096, 4096, 4096);
                setVector(&work->bursts[i][j].target, 0, 0, 0);
                setVector(&work->bursts[i][j].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->bursts[i][j].xform.position, 0, 0, 0);
            }
        }
        for (i = 0; i < work->row_count; i++) {
            INIT_ELEMENT(work->domes[i], 96, 6);
            work->domes[i].head.u = 32;
            work->domes[i].head.v = 128;
            setVector(&work->domes[i].xform.scale, 4096, 4096, 4096);
            setVector(&work->domes[i].target, 0, 0, 0);
            setVector(&work->domes[i].xform.rotation, 0, work->head.slot * 2048, 0);
            setVector(&work->domes[i].xform.position, 0, -350, work->direction * 450);
        }
        for (j = 0; j < work->spark_columns; j++) {
            for (i = 0; i < work->spark_rows; i++) {
                INIT_ELEMENT(work->sparks[i][j], 64, 3);
                work->sparks[i][j].head.u = 0;
                work->sparks[i][j].head.v = 64;
                setVector(&work->sparks[i][j].xform.scale, 4096, 4096, 4096);
                setVector(&work->sparks[i][j].target, 0, 0, 0);
                setVector(&work->sparks[i][j].xform.rotation, 0, work->head.slot * 2048, 0);
                setVector(&work->sparks[i][j].xform.position, 0, 0, 0);
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
        work->delay = gModelVariant681_aConfigs[command].delay;
        for (i = 0; i < 6; i++) {
            work->flags[i] = 0;
        }
    }
    if (work->delay > 0) {
        work->delay -= step;
    } else {
        switch (work->head.command) {
        case 0:
            if (work->head.elapsed == 0) {
                for (i = 0; i < work->flash_count; i++) {
                    work->flashes[i].head.active = 1;
                    TRACK_POINT(8, ls);
                    TRACK_POINT(15, m);
                    setVector(&work->flashes[i].xform.position, (ls.t[0] + m.t[0]) / 2, (ls.t[1] + m.t[1]) / 2,
                        (ls.t[2] + m.t[2]) / 2);
                }
            }
            if (work->head.elapsed >= 67 && work->flags[0] == 0) {
                for (i = 0; i < work->column_count; i++) {
                    START_RING(i, (i + 1) * 7);
                }
                work->flags[0] = 1;
            }
            if (work->head.elapsed >= 83 && work->flags[5] == 0) {
                work->domes[0].head.active = 1;
                work->flags[5] = 1;
            }
            break;
        case 1:
            START_FLASHES(3);
            if (work->head.elapsed >= 179 && work->flags[0] == 0) {
                for (i = 0; i < work->column_count; i++) {
                    START_RING(i, i + 8);
                }
                work->flags[0] = 1;
            }
            if (work->head.elapsed < 40) {
                EMIT_SPARKS(2, work->direction * 90);
            }
            if (work->head.elapsed >= 277 && work->head.elapsed < 320) {
                EMIT_SPARKS(3, -(work->direction * 110));
            }
            if (work->head.elapsed >= 251 && work->flags[5] == 0) {
                work->domes[0].head.active = 1;
                work->flags[5] = 1;
            }
            break;
        case 2:
            START_FLASHES(6);
            if (work->head.elapsed >= 127 && work->flags[0] == 0) {
                for (i = 0; i < work->column_count; i++) {
                    START_RING(i, i + 19);
                }
                work->flags[0] = 1;
            }
            if (work->head.elapsed >= 167 && work->flags[5] == 0) {
                work->domes[0].head.active = 1;
                work->flags[5] = 1;
            }
            break;
        case 3:
            START_FLASHES(3);
            if (work->head.elapsed >= 61 && work->flags[0] == 0) {
                for (i = 0; i < work->column_count; i++) {
                    START_RING(i, i * 3 + 16);
                }
                work->flags[0] = 1;
            }
            if (work->head.elapsed >= 81 && work->flags[5] == 0) {
                work->domes[0].head.active = 1;
                work->flags[5] = 1;
            }
            break;
        }
        PushMatrix();
        view = *(ModelVariantViewSnapshot *)Model_GetViewMetricsBuffer();
        inactive = 0;
        for (i = 0; i < work->flash_count; i++) {
            if (work->flashes[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                /* An if/else rather than a preset 4096: it keeps the /5
                 * constant inside the loop and the scale address unshared. */
                if (work->flashes[i].head.elapsed < work->flashes[i].head.duration / 5) {
                    value = work->flashes[i].head.elapsed * 0x5000 / work->flashes[i].head.duration;
                } else {
                    value = 4096;
                }
                if (!(work->flashes[i].xform.rotation.vx & 1)) {
                    value /= 2;
                }
                setVector(&work->flashes[i].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->flashes[i].xform.scale);
                RotMatrixY(work->flashes[i].xform.rotation.vy, &unit.matrix);
                unit.matrix.t[0] = work->flashes[i].xform.position.vx;
                unit.matrix.t[1] = work->flashes[i].xform.position.vy;
                if (work->flashes[i].head.elapsed < work->flashes[i].head.duration / 5) {
                    /* The shift spelling of elapsed * 5 stops GCC moving the 5 onto
                     * the distance. */
                    unit.matrix.t[2] = work->flashes[i].xform.position.vz +
                        (work->flashes[i].target.vz - work->flashes[i].xform.position.vz) *
                            ((work->flashes[i].head.elapsed << 2) + work->flashes[i].head.elapsed) /
                            work->flashes[i].head.duration;
                } else {
                    unit.matrix.t[2] = work->flashes[i].target.vz;
                }
                unit.super = work->flashes[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                DRAW_SPRITE(0, work->flashes[i].head.u, work->flashes[i].head.v, work->flashes[i].head.u + 63, work->flashes[i].head.v,
                    work->flashes[i].head.u, work->flashes[i].head.v + 63, work->flashes[i].head.u + 63, work->flashes[i].head.v + 63,
                    work->flashes[i], &work->corners[0], &work->corners[1], &work->corners[2], &work->corners[3]);
                work->flashes[i].xform.rotation.vx++;
                work->flashes[i].head.elapsed += step;
                if (work->flashes[i].head.duration < work->flashes[i].head.elapsed) {
                    work->flashes[i].head.active = 0;
                }
            } else {
                inactive++;
            }
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                if (work->rings[i][j].head.active == 1) {
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    value = work->rings[i][j].head.elapsed * 0x3000 / work->rings[i][j].head.duration;
                    setVector(&work->rings[i][j].xform.scale, value, value, value);
                    ScaleMatrix(&unit.matrix, &work->rings[i][j].xform.scale);
                    RotMatrixX(work->rings[i][j].xform.rotation.vx, &unit.matrix);
                    RotMatrixY(work->rings[i][j].xform.rotation.vy, &unit.matrix);
                    unit.matrix.t[0] = work->rings[i][j].xform.position.vx;
                    unit.matrix.t[1] = work->rings[i][j].xform.position.vy;
                    unit.matrix.t[2] = work->rings[i][j].xform.position.vz;
                    unit.super = work->rings[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    FADE(work->rings[i][j], 3);
                    for (k = 0; k < 16; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[1];
                        work->strip.clut = work->clut[1];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->rings[i][j].head.u, work->rings[i][j].head.v,
                            work->rings[i][j].head.u + 31, work->rings[i][j].head.v,
                            work->rings[i][j].head.u, work->rings[i][j].head.v + 127,
                            work->rings[i][j].head.u + 31, work->rings[i][j].head.v + 127);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        setRGB2(&work->strip, work->rings[i][j].head.color[0], work->rings[i][j].head.color[1],
                            work->rings[i][j].head.color[2]);
                        setRGB3(&work->strip, work->rings[i][j].head.color[0], work->rings[i][j].head.color[1],
                            work->rings[i][j].head.color[2]);
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
                                setUV4(&work->strip, work->rings[i][j].head.u, work->rings[i][j].head.v,
                                    work->rings[i][j].head.u + 31, work->rings[i][j].head.v,
                                    work->rings[i][j].head.u, work->rings[i][j].head.v + 127,
                                    work->rings[i][j].head.u + 31, work->rings[i][j].head.v + 127);
                            } else {
                                setUV4(&work->strip, 64, 0, 127, 0, 64, 63, 127, 63);
                            }
                            setRGB2(&work->strip, 0, 0, 0);
                            setRGB3(&work->strip, 0, 0, 0);
                            setRGB0(&work->strip, work->rings[i][j].head.color[0], work->rings[i][j].head.color[1],
                                work->rings[i][j].head.color[2]);
                            setRGB1(&work->strip, work->rings[i][j].head.color[0], work->rings[i][j].head.color[1],
                                work->rings[i][j].head.color[2]);
                            SORT_STRIP(&work->inner[k][amount], &work->inner[k + 1][amount], &work->outer[k][amount],
                                &work->outer[k + 1][amount]);
                        }
                    }
                    work->rings[i][j].head.elapsed += step;
                    if (work->rings[i][j].head.duration < work->rings[i][j].head.elapsed) {
                        work->rings[i][j].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (j = 0; j < work->column_count; j++) {
            for (i = 0; i < work->row_count; i++) {
                if (work->bursts[i][j].head.active == 1) {
                    value = (work->bursts[i][j].head.elapsed << 12) / work->bursts[i][j].head.duration + 4096;
                    setVector(&work->bursts[i][j].xform.scale, value, value, value);
                    FADE(work->bursts[i][j], 3);
                    for (k = 0; k < 5; k++) {
                        unit.flg = 0;
                        unit.matrix = GsIDMATRIX;
                        ScaleMatrix(&unit.matrix, &work->bursts[i][j].xform.scale);
                        RotMatrixZ(work->bursts[i][j].xform.rotation.vz, &unit.matrix);
                        RotMatrixX(work->bursts[i][j].xform.rotation.vx - (view.pitch - 4096) * work->direction,
                            &unit.matrix);
                        RotMatrixY(work->bursts[i][j].xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                        unit.matrix.t[0] = work->bursts[i][j].xform.position.vx +
                            work->offsets[k].vx * work->bursts[i][j].head.elapsed / work->bursts[i][j].head.duration;
                        unit.matrix.t[1] = work->bursts[i][j].xform.position.vy +
                            work->offsets[k].vy * work->bursts[i][j].head.elapsed / work->bursts[i][j].head.duration;
                        unit.matrix.t[2] = work->bursts[i][j].xform.position.vz +
                            work->offsets[k].vz * work->bursts[i][j].head.elapsed / work->bursts[i][j].head.duration;
                        unit.super = work->bursts[i][j].xform.coord;
                        GsGetLsUnit(&unit, &ls);
                        GsSetLsMatrix(&ls);
                        DRAW_SPRITE(3, work->bursts[i][j].head.u, work->bursts[i][j].head.v, work->bursts[i][j].head.u + 31,
                            work->bursts[i][j].head.v, work->bursts[i][j].head.u, work->bursts[i][j].head.v + 31,
                            work->bursts[i][j].head.u + 31, work->bursts[i][j].head.v + 31, work->bursts[i][j],
                            &work->burst_corners[0], &work->burst_corners[1], &work->burst_corners[2],
                            &work->burst_corners[3]);
                    }
                    if (work->bursts[i][j].head.u < 224) {
                        work->bursts[i][j].head.u += 32;
                    }
                    work->bursts[i][j].head.elapsed += step;
                    if (work->bursts[i][j].head.duration < work->bursts[i][j].head.elapsed) {
                        work->bursts[i][j].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        for (i = 0; i < work->row_count; i++) {
            if (work->domes[i].head.active == 1) {
                unit.flg = 0;
                unit.matrix = GsIDMATRIX;
                value = (work->domes[i].head.elapsed << 14) / work->domes[i].head.duration + 0x800;
                setVector(&work->domes[i].xform.scale, value, value, value);
                ScaleMatrix(&unit.matrix, &work->domes[i].xform.scale);
                RotMatrixZ(work->domes[i].head.elapsed * 50, &unit.matrix);
                RotMatrixX(work->domes[i].head.elapsed * 60, &unit.matrix);
                RotMatrixY(work->domes[i].head.elapsed * 70, &unit.matrix);
                unit.matrix.t[0] = work->domes[i].xform.position.vx;
                unit.matrix.t[1] = work->domes[i].xform.position.vy;
                unit.matrix.t[2] = work->domes[i].xform.position.vz;
                unit.super = work->domes[i].xform.coord;
                GsGetLsUnit(&unit, &ls);
                GsSetLsMatrix(&ls);
                if (work->domes[i].head.duration / 2 < work->domes[i].head.elapsed) {
                    FADE(work->domes[i], 6);
                } else {
                    work->domes[i].head.color[0] = gModelVariant681_aConfigs[work->head.command].color[6];
                    work->domes[i].head.color[1] = gModelVariant681_aConfigs[work->head.command].color[7];
                    work->domes[i].head.color[2] = gModelVariant681_aConfigs[work->head.command].color[8];
                }
                work->domes[i].head.v = (work->domes[i].head.elapsed & 0xF) * 63 / 16 - 128;
                for (j = 0; j < 8; j++) {
                    for (k = 0; k < 16; k++) {
                        setPolyFT4(&work->sprite);
                        work->sprite.tpage = work->tpage[5];
                        work->sprite.clut = work->clut[5];
                        setSemiTrans(&work->sprite, 1);
                        uv[0] = work->domes[i].head.u + k % 2 * 63 / 2 + 64;
                        uv[1] = work->domes[i].head.u + (k % 2 + 1) * 63 / 2 + 64;
                        setUV4(&work->sprite, uv[0], work->domes[i].head.v, uv[1], work->domes[i].head.v, uv[0],
                            work->domes[i].head.v + 64, uv[1], work->domes[i].head.v + 64);
                        setRGB0(&work->sprite, work->domes[i].head.color[0], work->domes[i].head.color[1],
                            work->domes[i].head.color[2]);
                        depth = RotAverage4(&work->dome[k][j], &work->dome[k + 1][j], &work->dome[k][j + 1],
                            &work->dome[k + 1][j + 1], (long *)&work->sprite.x0, (long *)&work->sprite.x1,
                            (long *)&work->sprite.x2, (long *)&work->sprite.x3, &interpolation, &flag);
                        if (flag >= 0) {
                            GsSortPoly(&work->sprite, Graphics_GetDefaultOrderingTable(), (u16)depth);
                        }
                    }
                }
                if (work->domes[i].head.duration / 2 < work->domes[i].head.elapsed) {
                    FADE(work->domes[i], 3);
                } else {
                    work->domes[i].head.color[0] = gModelVariant681_aConfigs[work->head.command].color[3];
                    work->domes[i].head.color[1] = gModelVariant681_aConfigs[work->head.command].color[4];
                    work->domes[i].head.color[2] = gModelVariant681_aConfigs[work->head.command].color[5];
                }
                for (j = 0; j < 8; j++) {
                    work->domes[i].head.v = (work->domes[i].head.elapsed + j * 10) % 32 * 63 / 32 - 128;
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->domes[i].xform.scale);
                    /* The sign and base angle reuse value and amount. */
                    value = j % 2 * 2 - 1;
                    amount = j << 9;
                    RotMatrixZ(value * work->domes[i].head.elapsed * 5 + amount, &unit.matrix);
                    RotMatrixX(value * work->domes[i].head.elapsed * 6 + amount, &unit.matrix);
                    RotMatrixY(value * work->domes[i].head.elapsed * 7 + amount, &unit.matrix);
                    unit.matrix.t[0] = work->domes[i].xform.position.vx;
                    unit.matrix.t[1] = work->domes[i].xform.position.vy;
                    unit.matrix.t[2] = work->domes[i].xform.position.vz;
                    unit.super = work->domes[i].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    for (k = 0; k < 8; k++) {
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[4];
                        work->strip.clut = work->clut[4];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->domes[i].head.u, work->domes[i].head.v + 63, work->domes[i].head.u + 63,
                            work->domes[i].head.v + 63, work->domes[i].head.u, work->domes[i].head.v, work->domes[i].head.u + 63,
                            work->domes[i].head.v);
                        setRGB2(&work->strip, work->domes[i].head.color[0], work->domes[i].head.color[1],
                            work->domes[i].head.color[2]);
                        setRGB3(&work->strip, work->domes[i].head.color[0], work->domes[i].head.color[1],
                            work->domes[i].head.color[2]);
                        setRGB0(&work->strip, 0, 0, 0);
                        setRGB1(&work->strip, 0, 0, 0);
                        SORT_STRIP(&work->cone_inner[k], &work->cone_inner[k + 1], &work->cone_middle[k],
                            &work->cone_middle[k + 1]);
                        setPolyGT4(&work->strip);
                        work->strip.tpage = work->tpage[4];
                        work->strip.clut = work->clut[4];
                        setSemiTrans(&work->strip, 1);
                        setUV4(&work->strip, work->domes[i].head.u, work->domes[i].head.v + 63, work->domes[i].head.u + 63,
                            work->domes[i].head.v + 63, work->domes[i].head.u, work->domes[i].head.v, work->domes[i].head.u + 63,
                            work->domes[i].head.v);
                        setRGB0(&work->strip, work->domes[i].head.color[0], work->domes[i].head.color[1],
                            work->domes[i].head.color[2]);
                        setRGB1(&work->strip, work->domes[i].head.color[0], work->domes[i].head.color[1],
                            work->domes[i].head.color[2]);
                        setRGB2(&work->strip, 0, 0, 0);
                        setRGB3(&work->strip, 0, 0, 0);
                        SORT_STRIP(&work->cone_middle[k], &work->cone_middle[k + 1], &work->cone_outer[k],
                            &work->cone_outer[k + 1]);
                    }
                }
                result = 4;
                work->domes[i].head.elapsed += step;
                if (work->domes[i].head.duration < work->domes[i].head.elapsed) {
                    work->domes[i].head.active = 0;
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
                if (work->sparks[i][j].head.active == 1) {
                    value = (work->sparks[i][j].head.elapsed << 12) / work->sparks[i][j].head.duration + 4096;
                    setVector(&work->sparks[i][j].xform.scale, value, value, value);
                    FADE(work->sparks[i][j], 3);
                    unit.flg = 0;
                    unit.matrix = GsIDMATRIX;
                    ScaleMatrix(&unit.matrix, &work->sparks[i][j].xform.scale);
                    RotMatrixZ(work->sparks[i][j].xform.rotation.vz, &unit.matrix);
                    RotMatrixX(work->sparks[i][j].xform.rotation.vx - (view.pitch - 4096) * work->direction,
                        &unit.matrix);
                    RotMatrixY(work->sparks[i][j].xform.rotation.vy + (0xC06 - view.yaw), &unit.matrix);
                    unit.matrix.t[0] = work->sparks[i][j].xform.position.vx;
                    unit.matrix.t[1] = work->sparks[i][j].xform.position.vy;
                    unit.matrix.t[2] = work->sparks[i][j].xform.position.vz;
                    unit.super = work->sparks[i][j].xform.coord;
                    GsGetLsUnit(&unit, &ls);
                    GsSetLsMatrix(&ls);
                    DRAW_SPRITE(3, work->sparks[i][j].head.u, work->sparks[i][j].head.v, work->sparks[i][j].head.u + 31,
                        work->sparks[i][j].head.v, work->sparks[i][j].head.u, work->sparks[i][j].head.v + 31,
                        work->sparks[i][j].head.u + 31, work->sparks[i][j].head.v + 31, work->sparks[i][j],
                        &work->spark_corners[0], &work->spark_corners[1], &work->spark_corners[2],
                        &work->spark_corners[3]);
                    work->sparks[i][j].head.u += 32;
                    if (work->sparks[i][j].head.u > 224) {
                        work->sparks[i][j].head.u = 0;
                    }
                    work->sparks[i][j].head.elapsed += step;
                    if (work->sparks[i][j].head.duration < work->sparks[i][j].head.elapsed) {
                        work->sparks[i][j].head.active = 0;
                    }
                } else {
                    inactive++;
                }
            }
        }
        PopMatrix();
        work->head.elapsed += step;
        if (inactive == work->flash_count + work->column_count * work->row_count * 2 + work->row_count +
                work->spark_columns * work->spark_rows) {
            result = 2;
        }
    }
    return result;
}
