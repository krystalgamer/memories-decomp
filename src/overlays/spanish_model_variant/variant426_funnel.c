#include "../../types.h"

#include "variant426_funnel.h"

/* One funnel of seventeen-point rings at the position at +0x20C8: an inner ring
 * of a fixed radius and an outer ring 160 plus the flicker wider, set back by 256
 * plus the flicker, drawn as sixteen POLY_GT4 quads scaled by the size word.
 * From phase 5 the colours fade with +0x2138; the rings spin by step * 80. */
void func_8013E440(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Funnel426State *state = (Funnel426State *)context;
    ModelVariantSheet *sheet;
    Funnel426 *funnel;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, k;
    s32 turn, yaw, pitch;
    s32 bias;
    s32 radius;
    s32 reach;
    s32 depth;
    s32 size;
    s32 angle;
    u8 r, g, b, outer_r, outer_g, outer_b;

    sheet = state->sheets;
    /* Empty statement block (possibly a compiled-out trace); its loop notes keep
     * the sheet pointer ahead of the OT fetch, as in the retail schedule. */
    do {
    } while (0);
    ot = func_80058F10();
    funnel = state->funnels;
    turn = ratan2(state->direction[2], state->direction[0]);
    yaw = turn + 0x800;
    pitch = ratan2(state->direction[1], state->direction[2]);
    quad = &state->quad;
    pitch += 0x800;
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw = turn + 0xC00;
    }
    bias = ((state->frame & 1) << 5) * sheet[1].size / 4096;
    radius = rsin(2048) * 512 >> 12;
    for (i = 0; i < 1; i++, funnel++) {
        if (state->size > 0) {
            size = state->size;
            k = 0;
            reach = radius;
            reach += 160 + bias;
            for (angle = state->spin; k < 17; k++, angle = state->spin + k * 0x100) {
                setVector(&funnel->inner[k], rcos(angle) * radius >> 12, 0, rsin(angle) * radius >> 12);
                setVector(&funnel->outer[k], rcos(angle) * reach >> 12, -(bias + 256),
                          rsin(angle) * reach >> 12);
            }
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->position[0];
            matrix.t[1] = state->position[1] - (rcos(2048) * 512 >> 12);
            matrix.t[2] = state->position[2];
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
            if (state->phase >= 5) {
                r = state->inner_color[0] * state->fade / 1024;
                g = state->inner_color[1] * state->fade / 1024;
                b = state->inner_color[2] * state->fade / 1024;
                outer_r = state->outer_color[0] * state->fade / 1024;
                outer_g = state->outer_color[1] * state->fade / 1024;
                outer_b = state->outer_color[2] * state->fade / 1024;
            } else {
                r = state->inner_color[0];
                g = state->inner_color[1];
                b = state->inner_color[2];
                outer_r = state->outer_color[0];
                outer_g = state->outer_color[1];
                outer_b = state->outer_color[2];
            }
            setRGB0(quad, r, g, b);
            setRGB1(quad, r, g, b);
            setRGB2(quad, outer_r, outer_g, outer_b);
            setRGB3(quad, outer_r, outer_g, outer_b);
            for (k = 0; k < 16; k++) {
                depth = RotTransPers4(&funnel->inner[k], &funnel->inner[k + 1],
                                     &funnel->outer[k], &funnel->outer[k + 1],
                                     (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                     (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                     &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
    }
    state->spin += state->step * 80;
}
