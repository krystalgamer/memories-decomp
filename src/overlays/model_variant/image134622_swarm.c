#include "../../types.h"
#include "../model_variant/variant451_entry.h"

void FrenchModelVariant468_DrawSwarm(Variant451EntryState *context)
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
    Variant451EntryState *state = context;
    Variant451EntryGroup *group = &state->group;
    POLY_FT4 *polygon = &state->sprite;
    s16 j;
    s16 spread;
    s32 complete;
    u8 red, green, blue;
    s32 fade;
    /* One temporary for the fade clamp test and the fade step keeps the load copy. */
    s32 level;
    s16 size;
    s32 depth;
    s32 grow;

    ot = Graphics_GetDefaultOrderingTable();
    complete = 1;
    ratan2(state->direction.vy, state->direction.vz);
    j = 0;
    spread = j;
    ratan2(state->screen_delta.vy, state->screen_delta.vx);
    for (; j < state->config->count; j++, spread = j * 1024) {
        if (group->scale[j] >= 0) {
            if (state->tail.phase < 4) {
                red = group->color.r;
                green = group->color.g;
                blue = group->color.b;
            } else {
                fade = group->fade[j];
                level = fade;
                if (level >= 1024) {
                    fade = 1024;
                }
                red = group->color.r * fade / 1024;
                green = group->color.g * fade / 1024;
                blue = group->color.b * fade / 1024;
            }
            rcos(group->scale[j]);
            grow = group->scale[j];
            size = (rsin(grow) * 3596 >> 12) + 500 + (rsin(state->spin + spread) * 192 >> 12);
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = group->origins[j].t[0] + state->offsets[j].vx * group->scale[j] / 1024 +
                          state->drift.vx * state->drift_scale / 1024;
            matrix.t[1] = group->origins[j].t[1] + state->offsets[j].vy * group->scale[j] / 1024 +
                          state->drift.vy * state->drift_scale / 1024 +
                          (rsin((s16)(grow * 2)) * 128 >> 12) -
                          (rsin(state->drift_scale * 8) * 16 >> 12);
            matrix.t[2] = group->origins[j].t[2] + state->offsets[j].vz * group->scale[j] / 1024 +
                          state->drift.vz * state->drift_scale / 1024;
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
            depth = RotTransPers4(&group->v0[j], &group->v1[j], &group->v2[j], &group->v3[j],
                                 (long *)&polygon->x0, (long *)&polygon->x1,
                                 (long *)&polygon->x2, (long *)&polygon->x3,
                                 &interpolation, &flag);
            polygon->r0 = red;
            polygon->g0 = green;
            polygon->b0 = blue;
            if (depth >= 0 && flag >= 0 && group->done[j] < 2) {
                GsSortPoly(polygon, ot, (u16)depth);
            }
        }
        if (group->scale[j] < 1024) {
            group->scale[j] += state->config->rate * state->clock.step / 2;
            if (group->scale[0] >= 512 && state->tail.phase == 0) {
                state->tail.phase = 2;
            }
            if (group->scale[j] >= 1024) {
                if (state->config->limit <= state->clock.frame) {
                    group->scale[j] = 1024;
                    group->done[j] = 1;
                } else {
                    group->scale[j] -= 1024;
                    group->initialized[j] = 0;
                }
            }
        }
        complete *= group->done[j];
        if (j + 1 == state->config->count && complete == 1 && state->tail.phase == 2) {
            state->tail.phase = 3;
        }
        if (state->tail.phase >= 4 && group->fade[j] > 0) {
            level = state->clock.step * 8;
            group->fade[j] -= level;
            if (group->fade[j] <= 0) {
                group->fade[j] = 0;
                if (state->tail.phase == 4 && j + 1 == state->config->count) {
                    state->tail.phase = 5;
                }
            }
        }
    }
    if (state->tail.phase >= 3 && state->drift_scale < 1024) {
        state->drift_scale += state->clock.step * 8;
        if (state->drift_scale >= 1024) {
            state->drift_scale = 1024;
            if (state->tail.phase == 3) {
                state->tail.phase = 4;
            }
        }
    }
    state->spin += state->clock.step * 64;
}
