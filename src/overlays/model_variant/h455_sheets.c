#include "../../types.h"
#include "../model_variant/h455_entry.h"

void SpanishModelVariant472_DrawSheets(EntryState455 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    EntryState455 *state = context;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j;
    SVECTOR *point;
    s32 extra;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    polygon = &state->textured[1];
    for (i = 0; i < 1; i++, sheet++) {
        extra = 0;
        if (state->clock.frame_count & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        matrix.t[0] = state->matrix.t[0];
        matrix.t[1] = state->matrix.t[1];
        matrix.t[2] = state->matrix.t[2];
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
        for (j = 0, point = sheet->v0; j < 4; j++, point++) {
            depth = RotTransPers4(point, &sheet->v1[j],
                                 &sheet->v2[j], &sheet->v3[j],
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
            depth = depth * 7 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (sheet->size < 4096 && state->phase == 0) {
            sheet->size = ((state->clock.frame - state->config->fan_start) << 12) /
                          (state->config->mesh_start - state->config->fan_start);
            if (sheet->size >= 4096) {
                sheet->size = 4096;
                state->phase = 1;
            }
        } else if (state->clock.frame >= state->config->fade_start) {
            sheet->size = 4096 - ((state->clock.frame - state->config->fade_start) << 12) /
                                (state->config->fade_end - state->config->fade_start);
            if (sheet->size <= 0) {
                sheet->size = 0;
                if (state->phase == 3) {
                    state->phase = 4;
                }
            }
        }
    }
}
