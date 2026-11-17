#include "../../types.h"
#include "variant474_entry.h"

void FrenchModelVariant474_DrawPairedFunnels(Variant474EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant474EntryState *state = context;
    Variant474EntryRibbon *trigger;
    ModelVariantCurtain *funnel = state->funnels;
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

    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(state->direction.vz, state->direction.vx) + 2048;
    pitch = ratan2(state->direction.vy, state->direction.vz);
    trigger = &state->ribbon;
    quad = &state->glow;
    pitch += 2048;
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw += 1024;
    }
    if (state->clock.frame_count & 1) {
        flicker = 1024;
    } else {
        flicker = 0;
    }
    rsin(2048);
    bias = flicker;
    for (i = 0, phi = 2048, beta = pitch + phi;
         i < 2; i++, funnel++, beta += 2048, phi += 2048) {
        if (funnel->scale > 0) {
            angle = state->spin;
            for (k = 0; k < 17; k++, angle = state->spin + k * 256) {
                setVector(&funnel->a[k], rcos(angle) * 64 >> 12,
                          rsin(angle) * 64 >> 12, 0);
                setVector(&funnel->b[k], rcos(angle) * 256 >> 12,
                          rsin(angle) * 256 >> 12,
                          -(rcos(phi) * 128 >> 12));
            }
            if (state->slot == 0) {
                rotation.vx = -pitch;
                rotation.vy = yaw + 192;
                rotation.vz = 0;
                matrix.t[0] = state->target.vx -
                              (rsin(yaw + phi + 192) * 128 >> 12);
                matrix.t[1] = state->target.vy - (rsin(beta) * 128 >> 12);
                matrix.t[2] = state->target.vz -
                              (rcos(beta + yaw + 192) * 128 >> 12);
                scale.vx = funnel->scale + bias;
                scale.vy = funnel->scale + bias;
                scale.vz = funnel->scale + bias;
            } else {
                rotation.vx = -pitch;
                rotation.vy = yaw - 192;
                rotation.vz = 0;
                matrix.t[0] = state->target.vx -
                              (rsin(yaw + phi - 192) * 128 >> 12);
                matrix.t[1] = state->target.vy - (rsin(beta) * 128 >> 12);
                matrix.t[2] = state->target.vz -
                              (rcos(beta + yaw - 192) * 128 >> 12);
                scale.vx = funnel->scale + bias;
                scale.vy = funnel->scale + bias;
                scale.vz = funnel->scale + bias;
            }
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            if (funnel->scale > 6144) {
                r = state->inner_color.r * (8192 - funnel->scale) / 2048;
                g = state->inner_color.g * (8192 - funnel->scale) / 2048;
                b = state->inner_color.b * (8192 - funnel->scale) / 2048;
                outer_r = state->outer_color.r * (8192 - funnel->scale) / 2048;
                outer_g = state->outer_color.g * (8192 - funnel->scale) / 2048;
                outer_b = state->outer_color.b * (8192 - funnel->scale) / 2048;
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
                    &funnel->a[k], &funnel->a[k + 1],
                    &funnel->b[k], &funnel->b[k + 1],
                    (long *)&quad->x0, (long *)&quad->x1,
                    (long *)&quad->x2, (long *)&quad->x3,
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
            if (funnel->scale < 8192 && trigger->amp[0] > 900) {
                funnel->scale += state->clock.step << 8;
                if (funnel->scale > 8192) {
                    funnel->scale = 8192;
                }
            }
        } else {
            if (funnel->scale < 8192 && trigger->amp[0] > 1100) {
                funnel->scale += state->clock.step << 8;
                if (funnel->scale > 8192) {
                    funnel->scale = 8192;
                }
            }
        }
    }
    state->spin += state->clock.step * 80;
}
