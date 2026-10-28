#include "../../types.h"
#include "image134622_swarm.h"

void func_8017C490(u8 *context)
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
    Swarm134622State *state = (Swarm134622State *)context;
    Swarm134622Group *group = &state->group;
    POLY_FT4 *polygon = &state->polygon;
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

    ot = func_80058F10();
    complete = 1;
    ratan2(state->direction.vy, state->direction.vz);
    j = 0;
    spread = j;
    ratan2(state->projected.vy, state->projected.vx);
    for (; j < state->timing->count; j++, spread = j * 1024) {
        if (group->size[j] >= 0) {
            if (state->phase < 4) {
                red = group->color[0];
                green = group->color[1];
                blue = group->color[2];
            } else {
                fade = group->fade[j];
                level = fade;
                if (level >= 1024) {
                    fade = 1024;
                }
                red = group->color[0] * fade / 1024;
                green = group->color[1] * fade / 1024;
                blue = group->color[2] * fade / 1024;
            }
            rcos(group->size[j]);
            grow = group->size[j];
            size = (rsin(grow) * 3596 >> 12) + 500 + (rsin(state->spin + spread) * 192 >> 12);
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = group->origins[j].t[0] + state->directions[j].vx * group->size[j] / 1024 +
                          state->drift.vx * state->drift_scale / 1024;
            matrix.t[1] = group->origins[j].t[1] + state->directions[j].vy * group->size[j] / 1024 +
                          state->drift.vy * state->drift_scale / 1024 +
                          (rsin((s16)(grow * 2)) * 128 >> 12) -
                          (rsin(state->drift_scale * 8) * 16 >> 12);
            matrix.t[2] = group->origins[j].t[2] + state->directions[j].vz * group->size[j] / 1024 +
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
            depth = RotTransPers4(&group->a[j], &group->b[j], &group->c[j], &group->d[j],
                                 (PSXLONG *)&polygon->x0, (PSXLONG *)&polygon->x1,
                                 (PSXLONG *)&polygon->x2, (PSXLONG *)&polygon->x3,
                                 &interpolation, &flag);
            polygon->r0 = red;
            polygon->g0 = green;
            polygon->b0 = blue;
            if (depth >= 0 && flag >= 0 && group->done[j] < 2) {
                GsSortPoly(polygon, ot, (u16)depth);
            }
        }
        if (group->size[j] < 1024) {
            group->size[j] += state->timing->rate * state->step / 2;
            if (group->size[0] >= 512 && state->phase == 0) {
                state->phase = 2;
            }
            if (group->size[j] >= 1024) {
                if (state->timing->limit <= state->time) {
                    group->size[j] = 1024;
                    group->done[j] = 1;
                } else {
                    group->size[j] -= 1024;
                    group->reset[j] = 0;
                }
            }
        }
        complete *= group->done[j];
        if (j + 1 == state->timing->count && complete == 1 && state->phase == 2) {
            state->phase = 3;
        }
        if (state->phase >= 4 && group->fade[j] > 0) {
            level = state->step * 8;
            group->fade[j] -= level;
            if (group->fade[j] <= 0) {
                group->fade[j] = 0;
                if (state->phase == 4 && j + 1 == state->timing->count) {
                    state->phase = 5;
                }
            }
        }
    }
    if (state->phase >= 3 && state->drift_scale < 1024) {
        state->drift_scale += state->step * 8;
        if (state->drift_scale >= 1024) {
            state->drift_scale = 1024;
            if (state->phase == 3) {
                state->phase = 4;
            }
        }
    }
    state->spin += state->step * 64;
}
