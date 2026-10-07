#include "../../types.h"

#include "variant444_funnels.h"

/* The MODEL411 funnels with smaller rings: an inner ring of radius 64 and an
 * outer ring of radius 128 plus the flicker, set back by 64 plus the flicker, where
 * the flicker is a 128th of the first sheet's size on odd frames. The fade start
 * is read from the overlay data after the code. */
void func_8013D40C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Funnel411State *state = (Funnel411State *)context;
    Funnel411 *funnel = state->funnels;
    ModelVariantSheet *sheet;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, k;
    s32 start;
    s32 yaw, pitch, turn;
    s32 bias;
    s32 size;
    s32 angle;
    s32 depth;
    u8 r, g, b, outer_r, outer_g, outer_b;

    ot = func_80058F10();
    sheet = &state->sheet;
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
    bias = ((state->frame & 1) << 5) * sheet->size / 4096;
    if (state->phase < 2) {
        start = 1;
        funnel++;
    } else {
        start = 0;
        funnel = state->funnels;
    }
    for (i = start; i < 2; i++, funnel++) {
        if (funnel->size > 0) {
            size = funnel->size;
            angle = state->spin;
            for (k = 0; k < 17; k++, angle = state->spin + k * 0x100) {
                setVector(&funnel->inner[k], (u32)rcos(angle) >> 6, (u32)rsin(angle) >> 6, 0);
                setVector(&funnel->outer[k], rcos(angle) * (bias + 128) >> 12, rsin(angle) * (bias + 128) >> 12,
                          -(bias + 64));
            }
            rotation.vx = -pitch;
            rotation.vy = yaw;
            rotation.vz = 0;
            matrix.t[0] = state->origin[0];
            matrix.t[1] = state->origin[1];
            matrix.t[2] = state->origin[2];
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
            r = state->inner_color[0];
            g = state->inner_color[1];
            b = state->inner_color[2];
            outer_r = state->outer_color[0];
            outer_g = state->outer_color[1];
            outer_b = state->outer_color[2];
            for (k = 0; k < 16; k++) {
                depth = RotTransPers4(&funnel->inner[k], &funnel->inner[k + 1],
                                     &funnel->outer[k], &funnel->outer[k + 1],
                                     (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                     (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                     &interpolation, &flag);
                setRGB0(quad, r, g, b);
                setRGB1(quad, r, g, b);
                setRGB2(quad, outer_r, outer_g, outer_b);
                setRGB3(quad, outer_r, outer_g, outer_b);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (state->phase < 3) {
            if (funnel->size < 4096) {
                funnel->size = (state->time - state->timing->grow_start) * 4096 /
                               (state->timing->grow_end - state->timing->grow_start);
                if (funnel->size >= 4096) {
                    funnel->size = 4096;
                }
            }
        } else if (state->timing->fade_after < state->time && funnel->size > 0) {
            funnel->size = 4096 - (state->time - *(u32 *)(D_8013D91C + 0x114)) * 4096 /
                                  (state->timing->fade_end - *(u32 *)(D_8013D91C + 0x114));
            if (funnel->size <= 0) {
                funnel->size = 0;
            }
        }
    }
    state->spin += state->step * 80;
}
