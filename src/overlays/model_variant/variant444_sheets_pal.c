#include "../../types.h"
#include "../french_model_variant/variant444_entry_pal.h"
#include "../../game/gpu_packets.h"

void SpanishModelVariant444_DrawSheets(Variant444EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant444EntryState *state = context;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, j;
    s32 bias;
    s32 depth;
    s32 r, g, b, outer_r, outer_g, outer_b;

    ot = Graphics_GetDefaultOrderingTable();
    quad = &state->textured[1];
    for (i = 0; i < 2; i++, sheet++) {
        bias = 0;
        if (state->clock.frame_count & 1) {
            bias = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = state->matrix.t[0];
            matrix.t[1] = state->matrix.t[1];
            matrix.t[2] = state->matrix.t[2];
        } else {
            matrix.t[0] = state->matrix.t[0] + state->aim.direction.vx * state->progress / 1024;
            matrix.t[1] = state->matrix.t[1] + state->aim.direction.vy * state->progress / 1024;
            matrix.t[2] = state->matrix.t[2] + state->aim.direction.vz * state->progress / 1024;
        }
        scale.vx = sheet->size + bias;
        scale.vy = sheet->size + bias;
        scale.vz = sheet->size + bias;
        RotMatrix(&rotation, &matrix);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        ReadRotMatrix(&light);
        RotMatrix(&rotation, &light);
        ScaleMatrix(&light, &scale);
        SetRotMatrix(&light);
        for (j = 0; j < 4; j++) {
            depth = RotTransPers4(&sheet->v0[j], &sheet->v1[j],
                                 &sheet->v2[j], &sheet->v3[j],
                                 (long *)&quad->x0, (long *)&quad->x1,
                                 (long *)&quad->x2, (long *)&quad->x3,
                                 &interpolation, &flag);
            if (state->tail.phase >= 3 && i != 0 && state->width < 512) {
                r = sheet->inner.r * state->width / 512;
                g = sheet->inner.g * state->width / 512;
                b = sheet->inner.b * state->width / 512;
                outer_r = sheet->outer.r * state->width / 512;
                outer_g = sheet->outer.g * state->width / 512;
                outer_b = sheet->outer.b * state->width / 512;
                quad->r0 = r;
                quad->g0 = g;
                quad->b0 = b;
                quad->r1 = r;
                quad->g1 = g;
                quad->b1 = b;
                quad->r2 = r;
                quad->g2 = g;
                quad->b2 = b;
                quad->r3 = outer_r;
                quad->g3 = outer_g;
                quad->b3 = outer_b;
            } else {
                quad->r0 = sheet->inner.r;
                quad->g0 = sheet->inner.g;
                quad->b0 = sheet->inner.b;
                quad->r1 = sheet->inner.r;
                quad->g1 = sheet->inner.g;
                quad->b1 = sheet->inner.b;
                quad->r2 = sheet->inner.r;
                quad->g2 = sheet->inner.g;
                quad->b2 = sheet->inner.b;
                quad->r3 = sheet->outer.r;
                quad->g3 = sheet->outer.g;
                quad->b3 = sheet->outer.b;
            }
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, (u16)depth);
            }
        }
        if (i == 0) {
            if (state->tail.phase == 0 && sheet->size < 4096) {
                sheet->size = (state->clock.frame - state->config->sheet_start) * 4096
                    / (state->config->tube_start - state->config->sheet_start);
                if (sheet->size >= 4096) {
                    sheet->size = 4096;
                    state->tail.phase = 1;
                }
            }
            if (state->tail.phase == 3 && state->clock.frame >= state->config->fade_start && sheet->size > 0) {
                sheet->size = 4096 - (state->clock.frame - state->config->fade_start) * 4096
                    / (state->config->fade_end - state->config->fade_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else if (state->tail.phase == 1) {
            sheet->size = 4096;
        } else if (state->tail.phase == 2) {
            if (sheet->size < 8192) {
                sheet->size += state->clock.step * 512;
                if (sheet->size >= 8192) {
                    sheet->size = 8192;
                    state->tail.phase = 3;
                }
            }
        } else if (state->tail.phase == 3) {
            if (state->clock.frame >= state->config->fade_start && sheet->size < 16384) {
                sheet->size = 8192 + (state->clock.frame - state->config->fade_start) * 16384
                    / (state->config->fade_end - state->config->fade_start);
                if (sheet->size >= 24576) {
                    sheet->size = 24576;
                }
            }
        }
    }
}
