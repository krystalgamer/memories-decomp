#include "../../types.h"
#include "../french_model_variant/variant461_entry.h"

void SpanishModelVariant461_DrawSheets(EntryState461 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    EntryState461 *state = context;
    ModelVariantCountedRibbon *primary = state->ribbons;
    ModelVariantSheetSet *sheet = state->quads;
    POLY_GT4 *polygon = state->packets.textured;
    ModelVariantGrowConfig *timing;
    GsOT *ot;
    s32 count, first;
    s32 i, j;
    s32 extra;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    timing = state->config;
    if (timing->paired == 0) {
        count = timing->first_count + 1;
        first = timing->first_count;
    } else {
        count = timing->first_count * 2;
        first = timing->first_count;
    }
    polygon++;
    for (i = 0; i < count; i++, sheet++) {
        extra = 0;
        if (state->clock.frame_count & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i < first) {
            matrix.t[0] = state->path_origins[i].vx;
            matrix.t[1] = state->path_origins[i].vy;
            matrix.t[2] = state->path_origins[i].vz;
            scale.vx = sheet->size + extra * 10;
            scale.vy = sheet->size + extra * 6;
            scale.vz = sheet->size + extra * 10;
        } else {
            matrix.t[0] = state->path_targets[i - first].vx;
            matrix.t[1] = state->path_targets[i - first].vy;
            matrix.t[2] = state->path_targets[i - first].vz;
            scale.vx = sheet->size + extra;
            scale.vy = sheet->size + extra;
            scale.vz = sheet->size + extra;
        }
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
            depth = RotTransPers4(&sheet->v0[j], &sheet->v1[j], &sheet->v2[j], &sheet->v3[j],
                                 (long *)&polygon->x0, (long *)&polygon->x1,
                                 (long *)&polygon->x2, (long *)&polygon->x3,
                                 &interpolation, &flag);
            polygon->r0 = sheet->inner.r;
            polygon->g0 = sheet->inner.g;
            polygon->b0 = sheet->inner.b;
            polygon->r1 = sheet->inner.r;
            polygon->g1 = sheet->inner.g;
            polygon->b1 = sheet->inner.b;
            polygon->r2 = sheet->inner.r;
            polygon->g2 = sheet->inner.g;
            polygon->b2 = sheet->inner.b;
            polygon->r3 = sheet->outer.r;
            polygon->g3 = sheet->outer.g;
            polygon->b3 = sheet->outer.b;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (i < first) {
            if (state->phase == 1) {
                if (sheet->size < 4096) {
                    sheet->size += ((u32)state->clock.step >> 1) << 11;
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        if (i + 1 == first) {
                            state->phase = 2;
                        }
                    }
                }
            } else if (state->phase >= 4 && sheet->size > 0) {
                if (state->clock.frame >= state->config->phase3_end) {
                    sheet->size = 4096 - ((state->clock.frame - state->config->phase3_end) << 12) /
                                        (state->config->matrix_end - state->config->phase3_end);
                }
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else {
            if (primary->state == 1) {
                if (sheet->shown == 0) {
                    sheet->size = 16384;
                    sheet->shown = 1;
                } else {
                    sheet->size = primary->length * 16;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        if (state->phase < 5) {
                            sheet->shown = 0;
                        }
                    }
                }
            } else {
                sheet->size = 0;
            }
            primary++;
        }
    }
}
