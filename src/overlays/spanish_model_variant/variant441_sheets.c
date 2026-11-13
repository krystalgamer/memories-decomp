#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "variant441_entry.h"

/* Draws two four-quad sheets in a colour cycling through eight hues with
 * the frame count: the first at the origin, the second advanced along the
 * direction by the progress. On the last index of each timing cycle the
 * first sheet grows over the timing window and shrinks over the fade
 * window, while the second follows the phase. */
void SpanishModelVariant441_DrawRainbowSheets(u8 *context)
{
    SVECTOR rot;
    /* The target reserves 16 unused bytes between rot and scale. */
    u8 unknown_stack[16];
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long projection;
    long flag;
    ModelVariantSheet *quad;
    GsOT *ot;
    Variant441EntryState *work;
    POLY_GT4 *poly;
    s32 i;
    u8 red;
    u8 green;
    s32 k;
    s32 bias;
    s32 depth;
    u8 blue;

    work = (Variant441EntryState *)context;
    quad = work->sheets;
    ot = Graphics_GetDefaultOrderingTable();
    poly = &work->packets.textured[1];
    if (work->frame_count % 8 == 0) {
        red = 255;
        green = 0;
        blue = 0;
    } else if ((s32)work->frame_count % 8 == 1) {
        red = 255;
        green = 128;
        blue = 0;
    } else if ((s32)work->frame_count % 8 == 2) {
        red = 255;
        green = 255;
        blue = 0;
    } else if ((s32)work->frame_count % 8 == 3) {
        red = 0;
        green = 255;
        blue = 0;
    } else if ((s32)work->frame_count % 8 == 4) {
        red = 0;
        green = 255;
        blue = 255;
    } else if ((s32)work->frame_count % 8 == 5) {
        red = 0;
        green = 0;
        blue = 255;
    } else if ((s32)work->frame_count % 8 == 6) {
        red = 255;
        green = 0;
        blue = 255;
    } else {
        red = 255;
        green = 0;
        blue = 128;
    }
    for (i = 0; i < 2; i++, quad++) {
        bias = 0;
        if (work->frame_count & 1) {
            bias = quad->size / 8;
        }
        setVector(&rot, 0, 0, 0);
        if (i == 0) {
            matrix.t[0] = work->matrix.t[0];
            matrix.t[1] = work->matrix.t[1];
            matrix.t[2] = work->matrix.t[2];
        } else {
            matrix.t[0] = work->matrix.t[0] +
                          work->aim.direction.vx * work->tube_progress / 1024;
            matrix.t[1] = work->matrix.t[1] +
                          work->aim.direction.vy * work->tube_progress / 1024;
            matrix.t[2] = work->matrix.t[2] +
                          work->aim.direction.vz * work->tube_progress / 1024;
        }
        setVector(&scale, quad->size + bias, quad->size + bias, quad->size + bias);
        RotMatrix(&rot, &matrix);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        ReadRotMatrix(&light);
        RotMatrix(&rot, &light);
        ScaleMatrix(&light, &scale);
        SetRotMatrix(&light);
        for (k = 0; k < 4; k++) {
            depth = RotTransPers4(
                &quad->v0[k], &quad->v1[k], &quad->v2[k], &quad->v3[k],
                (long *)&poly->x0, (long *)&poly->x1,
                (long *)&poly->x2, (long *)&poly->x3,
                &projection, &flag);
            poly->r0 = red;
            poly->g0 = green;
            poly->b0 = blue;
            poly->r1 = red;
            poly->g1 = green;
            poly->b1 = blue;
            poly->r2 = red;
            poly->g2 = green;
            poly->b2 = blue;
            poly->r3 = quad->outer[0];
            poly->g3 = quad->outer[1];
            poly->b3 = quad->outer[2];
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, depth);
            }
        }
        if (work->index + 1 == work->config->count) {
            if (i == 0) {
                if (work->phase == 0 && quad->size < 4096) {
                    quad->size = ((work->frame
                        - work->config->start) << 12)
                        / (work->config->expansion_start
                        - work->config->start);
                    if (quad->size >= 4096) {
                        quad->size = 4096;
                        work->phase = 1;
                    }
                }
                if (work->frame >= work->config->fade_start
                    && quad->size > 0) {
                    quad->size = 4096 - ((work->frame
                        - work->config->fade_start) << 12)
                        / (work->config->end
                        - work->config->fade_start);
                    if (quad->size <= 0) {
                        quad->size = 0;
                    }
                }
            } else if (work->phase <= 0) {
                quad->size = 0;
            } else if (work->phase == 1) {
                quad->size = 4096;
            } else if (work->phase == 2) {
                if (quad->size < 8192) {
                    quad->size += work->step << 9;
                    if (quad->size >= 8192) {
                        quad->size = 8192;
                        work->phase = 3;
                    }
                }
            } else if (work->phase == 4) {
                if (quad->size > 0) {
                    quad->size -= work->step << 6;
                    if (quad->size <= 0) {
                        quad->size = 0;
                        work->phase = 5;
                    }
                }
            }
        }
    }
}
