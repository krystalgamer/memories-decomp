#include "../../types.h"
#include "variant474_funnels.h"

void func_8013C738(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Funnels474State *state = (Funnels474State *)context;
    Funnel474TriggerView *trigger;
    Funnel474 *funnel = state->funnels;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, k;
    s32 phi;
    s32 yaw, pitch;
    s32 bias;
    s32 beta;
    s32 angle;
    s32 depth;
    s16 flicker;
    u8 r, g, b, outer_r, outer_g, outer_b;

    ot = func_80058F10();
    yaw = ratan2(state->direction[2], state->direction[0]) + 2048;
    pitch = ratan2(state->direction[1], state->direction[2]);
    trigger = &state->trigger;
    quad = &state->quad;
    pitch += 2048;
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw += 1024;
    }
    if (state->frame & 1) {
        flicker = 1024;
    } else {
        flicker = 0;
    }
    rsin(2048);
    bias = flicker;
    for (i = 0, phi = 2048, beta = pitch + phi;
         i < 2; i++, funnel++, beta += 2048, phi += 2048) {
        if (funnel->size > 0) {
            angle = state->spin;
            for (k = 0; k < 17; k++, angle = state->spin + k * 256) {
                setVector(&funnel->inner[k], rcos(angle) * 64 >> 12,
                          rsin(angle) * 64 >> 12, 0);
                setVector(&funnel->outer[k], rcos(angle) * 256 >> 12,
                          rsin(angle) * 256 >> 12,
                          -(rcos(phi) * 128 >> 12));
            }
            if (state->side == 0) {
                rotation.vx = -pitch;
                rotation.vy = yaw + 192;
                rotation.vz = 0;
                matrix.t[0] = state->target.vx -
                              (rsin(yaw + phi + 192) * 128 >> 12);
                matrix.t[1] = state->target.vy - (rsin(beta) * 128 >> 12);
                matrix.t[2] = state->target.vz -
                              (rcos(beta + yaw + 192) * 128 >> 12);
                scale.vx = funnel->size + bias;
                scale.vy = funnel->size + bias;
                scale.vz = funnel->size + bias;
            } else {
                rotation.vx = -pitch;
                rotation.vy = yaw - 192;
                rotation.vz = 0;
                matrix.t[0] = state->target.vx -
                              (rsin(yaw + phi - 192) * 128 >> 12);
                matrix.t[1] = state->target.vy - (rsin(beta) * 128 >> 12);
                matrix.t[2] = state->target.vz -
                              (rcos(beta + yaw - 192) * 128 >> 12);
                scale.vx = funnel->size + bias;
                scale.vy = funnel->size + bias;
                scale.vz = funnel->size + bias;
            }
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            if (funnel->size > 6144) {
                r = state->inner_color.r * (8192 - funnel->size) / 2048;
                g = state->inner_color.g * (8192 - funnel->size) / 2048;
                b = state->inner_color.b * (8192 - funnel->size) / 2048;
                outer_r = state->outer_color.r * (8192 - funnel->size) / 2048;
                outer_g = state->outer_color.g * (8192 - funnel->size) / 2048;
                outer_b = state->outer_color.b * (8192 - funnel->size) / 2048;
            } else {
                r = state->inner_color.r;
                g = state->inner_color.g;
                b = state->inner_color.b;
                outer_r = state->outer_color.r;
                outer_g = state->outer_color.g;
                outer_b = state->outer_color.b;
            }
            for (k = 0; k < 16; k++) {
                depth = RotTransPers4(
                    &funnel->inner[k], &funnel->inner[k + 1],
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
        if (i == 0) {
            if (funnel->size < 8192 && trigger->size > 900) {
                funnel->size += state->step << 8;
                if (funnel->size > 8192) {
                    funnel->size = 8192;
                }
            }
        } else {
            if (funnel->size < 8192 && trigger->size > 1100) {
                funnel->size += state->step << 8;
                if (funnel->size > 8192) {
                    funnel->size = 8192;
                }
            }
        }
    }
    state->spin += state->step * 80;
}
