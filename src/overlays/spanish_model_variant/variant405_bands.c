#include "../../types.h"
#include "variant405_entry.h"

void SpanishModelVariant405_DrawBands(Variant405EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    GsOT *ot;
    Variant405EntryState *state = context;
    Variant405EntryBand *band = state->bands;
    POLY_GT4 *polygon;
    s32 i, j;
    s32 size;
    s32 depth;
    u8 inner_r, inner_g, inner_b;
    u8 outer_r, outer_g, outer_b;

    ot = Graphics_GetDefaultOrderingTable();
    for (i = 0; i < state->config->count; i++, band++) {
        if (band->size > 0) {
            size = band->size;
            if (size > 3072) {
                inner_r = band->inner_color.r * (4096 - size) / 1024;
                inner_g = band->inner_color.g * (4096 - size) / 1024;
                inner_b = band->inner_color.b * (4096 - size) / 1024;
                outer_r = band->outer_color.r * (4096 - size) / 1024;
                outer_g = band->outer_color.g * (4096 - size) / 1024;
                outer_b = band->outer_color.b * (4096 - size) / 1024;
            } else {
                inner_r = band->inner_color.r;
                inner_g = band->inner_color.g;
                inner_b = band->inner_color.b;
                outer_r = band->outer_color.r;
                outer_g = band->outer_color.g;
                outer_b = band->outer_color.b;
            }
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->matrix.t[0];
            matrix.t[1] = state->matrix.t[1];
            matrix.t[2] = state->matrix.t[2];
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
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
            polygon = state->glyphs;
            j = 0;
            for (; j < 16; j++, polygon++) {
                depth = RotTransPers4(&band->inner[j], &band->inner[j + 1],
                                     &band->outer[j], &band->outer[j + 1],
                                     (long *)&polygon->x0, (long *)&polygon->x1,
                                     (long *)&polygon->x2, (long *)&polygon->x3,
                                     &interpolation, &flag);
                polygon->r0 = inner_r;
                polygon->g0 = inner_g;
                polygon->b0 = inner_b;
                polygon->r1 = inner_r;
                polygon->g1 = inner_g;
                polygon->b1 = inner_b;
                polygon->r2 = outer_r;
                polygon->g2 = outer_g;
                polygon->b2 = outer_b;
                polygon->r3 = outer_r;
                polygon->g3 = outer_g;
                polygon->b3 = outer_b;
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(polygon, ot, depth);
                }
            }
        }
        if (band->size < 4096) {
            band->size += state->clock.step * 128;
            if (band->size >= 4096) {
                band->size = 4096;
                band->completed++;
            }
        }
    }
}
