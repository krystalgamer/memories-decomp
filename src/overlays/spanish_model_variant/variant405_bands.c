#include "../../types.h"
#include "variant405_bands.h"

void func_8013C774(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    GsOT *ot;
    Bands405State *state = (Bands405State *)context;
    Bands405Band *band = state->bands;
    POLY_GT4 *polygon;
    s32 i, j;
    s32 size;
    s32 depth;
    u8 inner_r, inner_g, inner_b;
    u8 outer_r, outer_g, outer_b;

    ot = func_80058F10();
    for (i = 0; i < state->descriptor->count; i++, band++) {
        if (band->size > 0) {
            size = band->size;
            if (size > 3072) {
                inner_r = band->inner_color[0] * (4096 - size) / 1024;
                inner_g = band->inner_color[1] * (4096 - size) / 1024;
                inner_b = band->inner_color[2] * (4096 - size) / 1024;
                outer_r = band->outer_color[0] * (4096 - size) / 1024;
                outer_g = band->outer_color[1] * (4096 - size) / 1024;
                outer_b = band->outer_color[2] * (4096 - size) / 1024;
            } else {
                inner_r = band->inner_color[0];
                inner_g = band->inner_color[1];
                inner_b = band->inner_color[2];
                outer_r = band->outer_color[0];
                outer_g = band->outer_color[1];
                outer_b = band->outer_color[2];
            }
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->translation[0];
            matrix.t[1] = state->translation[1];
            matrix.t[2] = state->translation[2];
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
            polygon = state->polygons;
            j = 0;
            for (; j < 16; j++, polygon++) {
                depth = RotTransPers4(&band->inner[j], &band->inner[j + 1],
                                     &band->outer[j], &band->outer[j + 1],
                                     (PSXLONG *)&polygon->x0, (PSXLONG *)&polygon->x1,
                                     (PSXLONG *)&polygon->x2, (PSXLONG *)&polygon->x3,
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
            band->size += state->step * 128;
            if (band->size >= 4096) {
                band->size = 4096;
                band->completed++;
            }
        }
    }
}
