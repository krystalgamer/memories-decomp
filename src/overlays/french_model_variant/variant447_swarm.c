#include "../../types.h"
#include "../model_variant/variant430_entry.h"

void FrenchModelVariant447_DrawSwarm(Variant430EntryState *context)
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
    Variant430EntryState *state = context;
    Variant430EntrySwarm *group = &state->swarm;
    POLY_FT4 *polygon = &state->sprite;
    GsOT *ot;
    s16 j;
    s16 angle, spread;
    s32 complete;
    u8 red, green, blue;
    s32 lift;
    s16 radius, size;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    complete = 1;
    ratan2(state->aim.direction.vy, state->aim.direction.vz);
    ratan2(state->aim.screen_delta.vy, state->aim.screen_delta.vx);
    for (j = 0, angle = 0, spread = 0; j < 32; j++, spread = j * 1024, angle += 96) {
        if (group->size[j] > 0) {
            if (state->tail.phase < 2) {
                if (group->size[j] > 128 && group->size[j] < 384) {
                    red = group->color.r * (384 - group->size[j]) / 256;
                    green = group->color.g * (384 - group->size[j]) / 256;
                    blue = group->color.b * (384 - group->size[j]) / 256;
                } else if (group->size[j] >= 384) {
                    blue = green = red = 0;
                } else {
                    red = group->color.r;
                    green = group->color.g;
                    blue = group->color.b;
                }
                lift = 40;
            } else {
                if (group->size[j] > 512) {
                    red = group->color.r * (1024 - group->size[j]) / 512;
                    green = group->color.g * (1024 - group->size[j]) / 512;
                    blue = group->color.b * (1024 - group->size[j]) / 512;
                } else {
                    red = group->color.r;
                    green = group->color.g;
                    blue = group->color.b;
                }
                lift = 0;
            }
            radius = 512 - (rcos(group->size[j]) * 512 >> 12);
            size = (rsin(group->size[j]) * 3596 >> 12) + 500;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->matrix.t[0] + state->aim.direction.vx * group->size[j] / 512 +
                          (rcos(spread - 1024 + angle) * radius >> 12);
            matrix.t[1] = state->matrix.t[1] + state->aim.direction.vy * group->size[j] / 512 +
                          (rsin(spread - 1024 + angle) * radius >> 12) - lift;
            matrix.t[2] = state->matrix.t[2] + state->aim.direction.vz * group->size[j] / 512;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
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
            depth = RotTransPers4(&group->a[j], &group->b[j], &group->c[j], &group->d[j],
                                 (long *)&polygon->x0, (long *)&polygon->x1,
                                 (long *)&polygon->x2, (long *)&polygon->x3,
                                 &interpolation, &flag);
            polygon->r0 = red;
            polygon->g0 = green;
            polygon->b0 = blue;
            if (group->done[j] == 0 && depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, (u16)depth);
            }
        }
        if (group->size[j] < 1024 && state->tail.phase > 0) {
            group->size[j] += state->clock.step * 8;
            if (group->size[0] >= 512 && state->tail.phase < 3) {
                state->tail.phase = 3;
            }
            if (group->size[j] >= 1024) {
                if (state->config->limit <= state->clock.frame) {
                    group->size[j] = 1024;
                    group->done[j] = 1;
                } else {
                    group->size[j] -= 1024;
                }
            } else if (group->size[j] <= 0) {
                if (state->config->limit <= state->clock.frame) {
                    group->size[j] = 1024;
                    group->done[j] = 1;
                }
            }
        } else {
            group->size[j] -= state->clock.step * 24;
            if (j + 1 == 32 && group->size[j] <= 0) {
                state->tail.phase = 1;
            }
        }
        complete *= group->done[j];
        if (j + 1 == 32 && complete == 1 && state->tail.phase == 3) {
            state->tail.phase = 5;
        }
    }
    if (state->tail.phase == 1) {
        for (j = 0; j < 32; j++) {
            group->size[j] = -(j * 32);
        }
        state->tail.phase = 2;
    }
}
