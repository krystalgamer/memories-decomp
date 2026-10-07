#include "../../types.h"
#include "variant461_rings.h"
#include "../../game/gpu_packets.h"

void func_8013D498(u8 *context)
{
    SVECTOR rotation;
    /* Unused; retains the target's 16-byte stack gap after the rotation. */
    VECTOR unused;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    GsOT *ot;
    Rings461State *state = (Rings461State *)context;
    Ring461 *ring;
    POLY_FT4 *polygon;
    s16 i;
    s16 angle;
    s16 wave;
    s16 ripple;
    u8 red, green, blue;
    s32 size;
    s32 depth;

    ot = func_80058F10();
    red = 128;
    green = 128;
    blue = 128;
    i = 0;
    ripple = 0;
    ratan2(state->probe[0], state->probe[1]);
    ring = &state->ring;
    ratan2(state->delta_y, state->delta_x);
    polygon = &state->polygon;
    for (angle = state->angle, wave = state->wave; i < 24;
         i++, wave += 1300, ripple += 1700, angle += 512 - i * 128 / 24) {
        if (ring->size[i] > 0) {
            size = (ring->size[i] + (rsin(wave) * 512 >> 12)) * state->scale / 1024;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->origin[0] + (rcos(angle) * (rsin(i * 1024 / 24) * 512 >> 12) >> 12);
            matrix.t[1] = state->origin[1] + (rsin(ripple) * 32 >> 12);
            matrix.t[2] = state->origin[2] + (rsin(angle) * (rsin(i * 1024 / 24) * 512 >> 12) >> 12);
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
                                 (PSXLONG *)&polygon->x0, (PSXLONG *)&polygon->x1,
                                 (PSXLONG *)&polygon->x2, (PSXLONG *)&polygon->x3,
                                 &interpolation, &flag);
            polygon->r0 = red;
            polygon->g0 = green;
            polygon->b0 = blue;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, (u16)depth);
            }
        }
        if (ring->size[i] < 4096) {
            ring->size[i] += state->step * 64;
            if (ring->size[i] >= 4096) {
                ring->size[i] = 4096;
                if (i + 1 == 24 && state->phase == 0) {
                    state->phase = 1;
                }
            }
        }
    }
    if (state->phase == 6 && state->scale > 0) {
        state->scale -= state->step * 16;
        if (state->scale <= 0) {
            state->scale = 0;
            state->phase = 7;
        }
    }
    state->angle += 32;
    state->wave += 128;
}
