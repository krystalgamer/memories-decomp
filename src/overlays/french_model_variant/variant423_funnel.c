#include "../../types.h"
#include "variant423_funnel.h"

void func_8013E214(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Funnel423State *state = (Funnel423State *)context;
    ModelVariantSheet *sheet;
    Funnel423 *funnel;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, k;
    s32 turn, yaw, pitch;
    s32 bias;
    s32 radius;
    s32 reach;
    s32 depth;
    s32 size;
    s32 size_y, extra;
    s32 height;
    s32 angle;
    u8 r, g, b, outer_r, outer_g, outer_b;

    sheet = state->sheets;
    /* Retain the accepted funnel template's pointer-initialization loop notes. */
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
    radius = rsin(1300) * 384 >> 12;
    height = -(bias + 512);
    for (i = 0; i < 4; i++, funnel++) {
        if (state->size > 0) {
            if (i == 0) {
                size = state->size;
                size_y = size;
            } else {
                extra = 0;
                if (funnel->size >= 0) {
                    extra = funnel->size;
                }
                size_y = state->size;
                size = size_y + extra;
            }
            k = 0;
            angle = state->spin;
            reach = radius + (bias + 160);
            for (; k < 17; k++, angle = state->spin + k * 0x100) {
                setVector(&funnel->inner[k], rcos(angle) * radius >> 12, 0, rsin(angle) * radius >> 12);
                setVector(&funnel->outer[k], rcos(angle) * reach >> 12, height,
                          rsin(angle) * reach >> 12);
            }
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->position[0];
            matrix.t[1] = state->position[1] - ((rcos(1300) * 384 >> 12) + (rsin(276) * 384 >> 12));
            matrix.t[2] = state->position[2];
            scale.vx = size;
            scale.vy = size_y;
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
            r -= r * funnel->size / 8192;
            g -= g * funnel->size / 8192;
            b -= b * funnel->size / 8192;
            outer_r -= outer_r * funnel->size / 8192;
            outer_g -= outer_g * funnel->size / 8192;
            outer_b -= outer_b * funnel->size / 8192;
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
        if (funnel->size < 8192) {
            funnel->size += state->step << 7;
            if (funnel->size >= 8192) {
                funnel->size -= 8192;
            }
        }
    }
    state->spin += state->step * 80;
}
