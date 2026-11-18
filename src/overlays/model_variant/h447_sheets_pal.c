#include "../../types.h"
#include "h447_entry_pal.h"

void SpanishModelVariant464_DrawSheets(Variant464EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant464EntryState *state = context;
    /* Steps past the five primaries on the final sheet, which has none. */
    Variant464EntryPrimary *primary = state->primary;
    GsOT *ot;
    ModelVariantSheetSet *sheet = state->sheets;
    POLY_GT4 *polygon;
    SVECTOR *vertex;
    s32 i, j;
    s32 count;
    s32 extra;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    i = 0;
    count = state->config->group_count;
    polygon = &state->textured[1];
    for (; i <= count; i++, sheet++, primary++) {
        extra = 0;
        if (state->clock.frame_count & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == count) {
            matrix.t[0] = state->matrix.t[0];
            matrix.t[1] = state->matrix.t[1];
            matrix.t[2] = state->matrix.t[2];
        } else {
            matrix.t[0] = state->aim.target.vx;
            matrix.t[1] = state->aim.target.vy;
            matrix.t[2] = state->aim.target.vz;
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
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (i == count) {
            if (state->tail.phase == 0) {
                if (sheet->size < 4096) {
                    sheet->size = ((state->clock.frame - state->config->start) << 12) /
                                  (state->config->grow_end - state->config->start);
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        state->tail.phase = 1;
                    }
                }
            } else if (state->clock.frame > state->config->fade_begin && sheet->size > 0) {
                sheet->size = 4096 - ((state->clock.frame - state->config->fade_begin) << 12) /
                                    (state->config->fade_end - state->config->fade_begin);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                    state->tail.phase = 3;
                }
            }
        } else if (primary->active == 1) {
            if (sheet->size < 16384 && sheet->shown == 0) {
                sheet->size += state->clock.step * 4096;
                if (sheet->size >= 16384) {
                    sheet->size = 16384;
                    sheet->shown = 1;
                }
            } else if (sheet->size > 0) {
                sheet->size -= state->clock.step * 512;
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        }
    }
}
