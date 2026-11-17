#include "../../types.h"
#include "../french_model_variant/variant478_entry.h"
#include "../../game/gpu_packets.h"

void SpanishModelVariant478_DrawPetalRings(EntryState478 *context)
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
    EntryState478 *state = context;
    EntryPetals478 *ring = state->petals;
    POLY_FT4 *polygon = &state->swarm_packet;
    GsOT *ot;
    s16 i;
    s16 phase;
    u8 red, green, blue;
    s16 j;
    s16 tilt;
    s16 angle;
    s16 amount;
    s16 radius;
    s16 height;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    i = 0;
    phase = 0;
    ratan2(state->aim.direction.vy, state->aim.direction.vz);
    ratan2(state->aim.screen_delta.vy, state->aim.screen_delta.vx);
    for (tilt = 0; i < 3; i++, ring++, phase += 1400, tilt = i * 1024 / 3) {
        for (j = 0, angle = phase; j < 8; j++, angle = phase + j * 512) {
            if (ring->size[j] >= 0) {
                amount = 4096;
                red = ring->color.r;
                green = ring->color.g;
                blue = ring->color.b;
                if (ring->size[j] <= 4096) {
                    amount = ring->size[j];
                }
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = 0;
                radius = rcos(tilt) * (amount * 192 / 4096) >> 12;
                height = rsin(tilt) * (amount * 128 / 4096) >> 12;
                matrix.t[0] = state->matrix.t[0] + (rcos(angle) * radius >> 12);
                matrix.t[1] = state->matrix.t[1] - height + 32;
                matrix.t[2] = state->matrix.t[2] + (rsin(angle) * radius >> 12);
                amount += rsin(state->wave + angle + phase) * 256 >> 12;
                scale.vx = amount;
                scale.vy = amount;
                scale.vz = amount;
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
                depth = RotTransPers4(
                    &ring->v0[j], &ring->v1[j], &ring->v2[j], &ring->v3[j],
                    (long *)&polygon->x0, (long *)&polygon->x1,
                    (long *)&polygon->x2, (long *)&polygon->x3,
                    &interpolation, &flag);
                polygon->r0 = red;
                polygon->g0 = green;
                polygon->b0 = blue;
                if (depth >= 0 && flag >= 0 && ring->hidden[j] == 0) {
                    GsSortPoly(polygon, ot, depth);
                }
            }
            if (state->clock.frame < state->config->ring_end) {
                ring->size[j] += state->clock.step * 48;
            } else if (state->clock.frame > state->config->fade_start) {
                ring->size[j] -= state->clock.step * 48;
                if (ring->size[j] <= 0) {
                    ring->size[j] = 0;
                }
            }
        }
    }
    state->wave += state->clock.step * 32;
}
