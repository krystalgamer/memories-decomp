#include "../../types.h"

#include "../model_variant/variant444_entry_pal.h"

/* The MODEL411 funnels with smaller rings: an inner ring of radius 64 and an
 * outer ring of radius 128 plus the flicker, set back by 64 plus the flicker, where
 * the flicker is a 128th of the first sheet's size on odd frames. The fade start
 * is read from the overlay data after the code. */
void SpanishModelVariant444_DrawFunnels(Variant444EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant444EntryState *state = context;
    ModelVariantFunnel *funnel = state->funnels;
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

    ot = Graphics_GetDefaultOrderingTable();
    sheet = state->sheets;
    turn = ratan2(state->aim.direction.vz, state->aim.direction.vx);
    yaw = turn + 0x800;
    pitch = ratan2(state->aim.direction.vy, state->aim.direction.vz);
    quad = &state->paged;
    pitch += 0x800;
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw = turn + 0xC00;
    }
    bias = (((s32)state->clock.frame_count & 1) << 5) * sheet->size / 4096;
    if (state->tail.phase < 2) {
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
            matrix.t[0] = state->matrix.t[0];
            matrix.t[1] = state->matrix.t[1];
            matrix.t[2] = state->matrix.t[2];
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
            r = state->inner.r;
            g = state->inner.g;
            b = state->inner.b;
            outer_r = state->outer.r;
            outer_g = state->outer.g;
            outer_b = state->outer.b;
            for (k = 0; k < 16; k++) {
                depth = RotTransPers4(&funnel->inner[k], &funnel->inner[k + 1],
                                     &funnel->outer[k], &funnel->outer[k + 1],
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
        if (state->tail.phase < 3) {
            if (funnel->size < 4096) {
                funnel->size = (state->clock.frame - state->config->sheet_start) * 4096 /
                               (state->config->tube_start - state->config->sheet_start);
                if (funnel->size >= 4096) {
                    funnel->size = 4096;
                }
            }
        } else if (state->config->fade_start < state->clock.frame && funnel->size > 0) {
            funnel->size = 4096 - (state->clock.frame - gFrenchModelVariant444_aConfigs[0].fade_start) * 4096 /
                                  (state->config->fade_end - gFrenchModelVariant444_aConfigs[0].fade_start);
            if (funnel->size <= 0) {
                funnel->size = 0;
            }
        }
    }
    state->spin += state->clock.step * 80;
}
