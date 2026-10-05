#include "../../types.h"
#include "variant479_curtains.h"

void func_8013E860(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Curtains479State *state = (Curtains479State *)context;
    Curtains479Primary *primary = state->primary;
    ModelVariantCurtain *curtain = state->curtains;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j;
    s32 extra;
    s32 radius;
    s32 angle;
    s32 base_radius;
    s32 outer_radius;
    s32 size;
    s32 intensity;
    s32 depth;
    u8 inner_r, inner_g, inner_b;
    u8 outer_r, outer_g, outer_b;

    ot = func_80058F10();
    polygon = &state->polygon;
    extra = ((state->frame & 1) << 5) * state->pulse_scale / 4096;
    radius = (rsin(1300) * 256) >> 12;
    for (i = 0; i < 5; i++, curtain++, primary++) {
        if (state->gate >= 0) {
            size = primary->size;
            base_radius = radius + 160;
            outer_radius = base_radius + extra;
            for (j = 0, angle = state->angle; j < 17; j++, angle = state->angle + j * 256) {
                setVector(&curtain->a[j], (rcos(angle) * radius) >> 12, 0,
                          (rsin(angle) * radius) >> 12);
                setVector(&curtain->b[j], (rcos(angle) * outer_radius) >> 12,
                          -(256 + extra), (rsin(angle) * outer_radius) >> 12);
            }
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->positions[i].vx;
            matrix.t[1] = state->positions[i].vy - ((rcos(1300) * 256) >> 12);
            matrix.t[2] = state->positions[i].vz;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            if (primary->size < 4096) {
                inner_r = state->inner[0];
                inner_g = state->inner[1];
                inner_b = state->inner[2];
                outer_r = state->outer[0];
                outer_g = state->outer[1];
                outer_b = state->outer[2];
            } else {
                inner_r = state->inner[0] * (8192 - primary->size) / 4096;
                intensity = 8192 - primary->size;
                inner_g = state->inner[1] * intensity / 4096;
                inner_b = state->inner[2] * intensity / 4096;
                outer_r = state->outer[0] * intensity / 4096;
                outer_g = state->outer[1] * intensity / 4096;
                outer_b = state->outer[2] * intensity / 4096;
            }
            for (j = 0; j < 16; j++) {
                depth = RotTransPers4(&curtain->a[j], &curtain->a[j + 1],
                                     &curtain->b[j], &curtain->b[j + 1],
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
    }
    state->angle += state->step * 80;
}
