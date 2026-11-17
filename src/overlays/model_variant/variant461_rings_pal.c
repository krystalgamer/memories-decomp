#include "../../types.h"
#include "../french_model_variant/variant461_entry.h"
#include "../../game/gpu_packets.h"

void SpanishModelVariant461_DrawRing(EntryState461 *context)
{
    SVECTOR rotation;
    /* Unused; retains the target's 16-byte stack gap after the rotation. */
    VECTOR unused;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    GsOT *ot;
    EntryState461 *state = context;
    ModelVariantQuadRing *ring;
    POLY_FT4 *polygon;
    s16 i;
    s16 angle;
    s16 wave;
    s16 ripple;
    u8 red, green, blue;
    s32 size;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    red = 128;
    green = 128;
    blue = 128;
    i = 0;
    ripple = 0;
    ratan2(state->direction.vy, state->direction.vz);
    ring = &state->ring;
    ratan2(state->screen_delta.vy, state->screen_delta.vx);
    polygon = &state->ring_packet;
    for (angle = state->angle, wave = state->wave; i < 24;
         i++, wave += 1300, ripple += 1700, angle += 512 - i * 128 / 24) {
        if (ring->size[i] > 0) {
            size = (ring->size[i] + (rsin(wave) * 512 >> 12)) * state->scale / 1024;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->origin.vx + (rcos(angle) * (rsin(i * 1024 / 24) * 512 >> 12) >> 12);
            matrix.t[1] = state->origin.vy + (rsin(ripple) * 32 >> 12);
            matrix.t[2] = state->origin.vz + (rsin(angle) * (rsin(i * 1024 / 24) * 512 >> 12) >> 12);
            scale.vx = (s16)size;
            scale.vy = (s16)size;
            scale.vz = (s16)size;
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
            depth = RotTransPers4(&ring->v0[i], &ring->v1[i], &ring->v2[i], &ring->v3[i],
                                 (long *)&polygon->x0, (long *)&polygon->x1,
                                 (long *)&polygon->x2, (long *)&polygon->x3,
                                 &interpolation, &flag);
            polygon->r0 = red;
            polygon->g0 = green;
            polygon->b0 = blue;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, (u16)depth);
            }
        }
        if (ring->size[i] < 4096) {
            ring->size[i] += state->clock.step * 64;
            if (ring->size[i] >= 4096) {
                ring->size[i] = 4096;
                if (i + 1 == 24 && state->phase == 0) {
                    state->phase = 1;
                }
            }
        }
    }
    if (state->phase == 6 && state->scale > 0) {
        state->scale -= state->clock.step * 16;
        if (state->scale <= 0) {
            state->scale = 0;
            state->phase = 7;
        }
    }
    state->angle += 32;
    state->wave += 128;
}
