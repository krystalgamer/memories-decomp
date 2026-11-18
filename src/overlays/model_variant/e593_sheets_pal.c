#include "../../types.h"
#include "e593_entry_pal.h"

void SpanishModelVariant473_DrawSheets(EntryState473 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    GsOT *ot;
    EntryState473 *state = context;
    ModelVariantSheet *sheet = state->sheets;
    EntryRibbon473 *primary;
    POLY_GT4 *polygon;
    SVECTOR *vertex;
    s32 i, j;
    s32 extra;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    polygon = &state->packets.textured[1];
    /* The sixth sheet has no primary record; the cursor steps past the five. */
    primary = state->ribbons;
    for (i = 0; i < 6; i++, sheet++, primary++) {
        extra = 0;
        if (state->clock.frame_count & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 5) {
            matrix.t[0] = state->matrix.t[0];
            matrix.t[1] = state->matrix.t[1];
            matrix.t[2] = state->matrix.t[2];
        } else {
            matrix.t[0] = state->matrix.t[0] + primary->a[8].vx;
            matrix.t[1] = state->matrix.t[1] + primary->a[8].vy;
            matrix.t[2] = state->matrix.t[2] + primary->a[8].vz;
        }
        scale.vx = sheet->size + extra;
        scale.vy = sheet->size + extra;
        scale.vz = sheet->size + extra;
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
        j = 0;
        vertex = sheet->v0;
        for (; j < 4; j++, vertex++) {
            depth = RotTransPers4(vertex, &sheet->v1[j], &sheet->v2[j], &sheet->v3[j],
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
            depth = depth * 8 / 10;
            if (i == 5 || primary->state != 0) {
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(polygon, ot, depth);
                }
            }
        }
        if (i == 5) {
            if (state->phase == 0 && sheet->size < 4096) {
                sheet->size = ((state->clock.frame - state->config->draw_start) << 12) /
                              (state->config->grow_end - state->config->draw_start);
                if (sheet->size >= 4096) {
                    sheet->size = 4096;
                    state->phase = 1;
                }
            }
            if (state->clock.frame >= state->config->fade_begin && sheet->size > 0) {
                sheet->size = 4096 - ((state->clock.frame - state->config->fade_begin) << 12) /
                                    (state->config->fade_end - state->config->fade_begin);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else {
            if (state->phase == 2) {
                sheet->size = 4096;
            } else if (state->phase == 3 && sheet->size < 8192) {
                sheet->size += state->clock.step * 512;
                if (sheet->size >= 8192) {
                    sheet->size = 8192;
                    state->phase = 4;
                }
            }
            if (state->clock.frame >= state->config->fade_begin && sheet->size > 0) {
                sheet->size = 8192 - ((state->clock.frame - state->config->fade_begin) << 13) /
                                    (state->config->fade_end - state->config->fade_begin);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        }
    }
}
