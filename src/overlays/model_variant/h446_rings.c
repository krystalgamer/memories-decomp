#include "../../types.h"
#include "h446_entry.h"

void FrenchModelVariant463_DrawRings(Variant446EntryState *context)
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
    Variant446EntryState *state = context;
    ModelVariantQuadRing *ring;
    POLY_FT4 *polygon;
    s16 i;
    s16 angle;
    s16 wave;
    u8 red, green, blue;
    s32 size;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    red = 128;
    green = 128;
    blue = 128;
    i = 0;
    ratan2(state->direction.vy, state->direction.vz);
    ring = &state->sprites;
    ratan2(state->screen_delta.vy, state->screen_delta.vx);
    polygon = &state->sprite;
    for (angle = state->ring_angle, wave = state->ring_wave; i < 24;
         i++, wave += 768, angle += 512 - i * 128 / 24) {
        if (ring->size[i] > 0) {
            size = (ring->size[i] + (rsin(wave) * 256 >> 12)) * state->ring_scale / 1024;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->position.vx + (rcos(angle) * (rsin(i * 1024 / 24) * 512 >> 12) >> 12);
            matrix.t[1] = state->position.vy;
            matrix.t[2] = state->position.vz + (rsin(angle) * (rsin(i * 1024 / 24) * 512 >> 12) >> 12);
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
            polygon->u0 = 192;
            polygon->v0 = 64;
            polygon->u1 = 255;
            polygon->v1 = 64;
            polygon->u2 = 192;
            polygon->v2 = 127;
            polygon->u3 = 255;
            polygon->v3 = 127;
            depth = RotTransPers4(&ring->v0[i], &ring->v1[i], &ring->v2[i], &ring->v3[i],
                                 (long *)&polygon->x0, (long *)&polygon->x1,
                                 (long *)&polygon->x2, (long *)&polygon->x3,
                                 &interpolation, &flag);
            polygon->r0 = red;
            polygon->g0 = green;
            polygon->b0 = blue;
            if (depth > 0) {
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
    if (state->phase == 6 && state->ring_scale > 0) {
        state->ring_scale -= state->clock.step * 16;
        if (state->ring_scale <= 0) {
            state->ring_scale = 0;
            state->phase = 7;
        }
    }
    state->ring_angle += 32;
    state->ring_wave += 128;
}
