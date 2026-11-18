#include "../../types.h"
#ifdef MODEL_VARIANT425_FUNNEL
#include "e591_entry_pal.h"
#define FUNNEL_ENTRY_STATE EntryState425
#else
#include "e589_entry_pal.h"
#define FUNNEL_ENTRY_STATE EntryState423
#endif

#ifdef MODEL_VARIANT425_FUNNEL
#define MODEL_VARIANT_FUNNEL_RADIUS_SCALE 512
#else
#define MODEL_VARIANT_FUNNEL_RADIUS_SCALE 384
#endif

void FrenchModelVariant423_DrawFunnel(FUNNEL_ENTRY_STATE *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    FUNNEL_ENTRY_STATE *state = context;
    ModelVariantSheet *sheet;
    ModelVariantFunnel *funnel;
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
    ot = Graphics_GetDefaultOrderingTable();
    funnel = state->funnels;
    turn = ratan2(state->aim.direction.vz, state->aim.direction.vx);
    yaw = turn + 0x800;
    pitch = ratan2(state->aim.direction.vy, state->aim.direction.vz);
    quad = &state->funnel_packet;
    pitch += 0x800;
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw = turn + 0xC00;
    }
    bias = (((s32)state->clock.frame_count & 1) << 5) * sheet[1].size / 4096;
    radius = rsin(1300) * MODEL_VARIANT_FUNNEL_RADIUS_SCALE >> 12;
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
            matrix.t[0] = state->aim.target.vx;
            matrix.t[1] = state->aim.target.vy -
                          ((rcos(1300) * MODEL_VARIANT_FUNNEL_RADIUS_SCALE >> 12) +
                           (rsin(276) * MODEL_VARIANT_FUNNEL_RADIUS_SCALE >> 12));
            matrix.t[2] = state->aim.target.vz;
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
                r = state->inner.r * state->intensity / 1024;
                g = state->inner.g * state->intensity / 1024;
                b = state->inner.b * state->intensity / 1024;
                outer_r = state->outer.r * state->intensity / 1024;
                outer_g = state->outer.g * state->intensity / 1024;
                outer_b = state->outer.b * state->intensity / 1024;
            } else {
                r = state->inner.r;
                g = state->inner.g;
                b = state->inner.b;
                outer_r = state->outer.r;
                outer_g = state->outer.g;
                outer_b = state->outer.b;
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
                                     (long *)&quad->x0, (long *)&quad->x1,
                                     (long *)&quad->x2, (long *)&quad->x3,
                                     &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (funnel->size < 8192) {
            funnel->size += state->clock.step << 7;
            if (funnel->size >= 8192) {
                funnel->size -= 8192;
            }
        }
    }
    state->spin += state->clock.step * 80;
}
