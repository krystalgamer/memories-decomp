#include "../../types.h"
#include "variant427_curtains.h"

void func_8013E5BC(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Curtain427State *state = (Curtain427State *)context;
    Curtain427Primary *primary = state->primary;
    ModelVariantCurtain *curtain = state->curtains;
    POLY_GT4 *polygon = &state->polygon;
    GsOT *ot;
    s32 i, j;
    s32 x, y;
    s32 extra;
    s32 depth;
    u8 inner_r, inner_g, inner_b;
    u8 outer_r, outer_g, outer_b;

    ot = func_80058F10();
    rsin(1300);
    for (i = 0; i < 9; i++, curtain++) {
        if (state->size > 0) {
            if (i < 3) {
                x = y = primary[i % 3].scale;
            } else {
                extra = 0;
                if (curtain->scale >= 0) {
                    extra = curtain->scale;
                }
                y = primary[i % 3].scale;
                x = y + extra;
            }
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->origins[i % 3].vx;
            matrix.t[1] = state->origins[i % 3].vy;
            matrix.t[2] = state->origins[i % 3].vz;
            scale.vx = x;
            scale.vy = y;
            scale.vz = x;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            if (state->phase >= 5) {
                inner_r = state->inner[0] * state->intensity / 1024;
                inner_g = state->inner[1] * state->intensity / 1024;
                inner_b = state->inner[2] * state->intensity / 1024;
                outer_r = state->outer[0] * state->intensity / 1024;
                outer_g = state->outer[1] * state->intensity / 1024;
                outer_b = state->outer[2] * state->intensity / 1024;
            } else {
                inner_r = state->inner[0];
                inner_g = state->inner[1];
                inner_b = state->inner[2];
                outer_r = state->outer[0];
                outer_g = state->outer[1];
                outer_b = state->outer[2];
            }
            inner_r -= inner_r * curtain->scale / 8192;
            inner_g -= inner_g * curtain->scale / 8192;
            inner_b -= inner_b * curtain->scale / 8192;
            outer_r -= outer_r * curtain->scale / 8192;
            outer_g -= outer_g * curtain->scale / 8192;
            outer_b -= outer_b * curtain->scale / 8192;
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
            for (j = 0; j < 16; j++) {
                depth = RotTransPers4(&curtain->a[j], &curtain->a[j + 1],
                                     &curtain->b[j], &curtain->b[j + 1],
                                     (PSXLONG *)&polygon->x0, (PSXLONG *)&polygon->x1,
                                     (PSXLONG *)&polygon->x2, (PSXLONG *)&polygon->x3,
                                     &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(polygon, ot, depth);
                }
            }
        }
        if (curtain->scale < 8192) {
            curtain->scale += state->step * 128;
            if (curtain->scale >= 8192) {
                curtain->scale -= 8192;
            }
        }
    }
    state->angle += state->step * 80;
}
