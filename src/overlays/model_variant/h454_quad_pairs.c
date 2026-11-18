#include "../../types.h"
#include "../model_variant/h454_entry.h"

void FrenchModelVariant471_DrawQuadGroup(Variant454EntryState *context)
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
    Variant454EntryState *state = context;
    Variant454EntryGroup *group = state->groups;
    POLY_FT4 *polygon = &state->glow;
    s16 i, j;
    s32 complete;
    u8 red, green, blue;
    s32 depth;
    s16 progress, size;

    ot = Graphics_GetDefaultOrderingTable();
    complete = 1;
    i = 0;
    ratan2(state->aim.direction.vy, state->aim.direction.vz);
    ratan2(state->aim.screen_delta.vy, state->aim.screen_delta.vx);
    for (; i < 1; i++, group++) {
        for (j = 0; j < 16; j++) {
            if (group->scale[j] <= 1024) {
                if (group->scale[j] > 512) {
                    red = group->color.r * (1024 - group->scale[j]) / 512;
                    green = group->color.g * (1024 - group->scale[j]) / 512;
                    blue = group->color.b * (1024 - group->scale[j]) / 512;
                } else {
                    red = group->color.r;
                    green = group->color.g;
                    blue = group->color.b;
                }
                rcos(group->scale[j]);
                if (group->scale[j] < 0) {
                    progress = 0;
                } else {
                    progress = group->scale[j];
                }
                if (group->scale[j] < -1024) {
                    size = 0;
                } else if (group->scale[j] >= 0) {
                    size = 1024;
                } else {
                    size = group->scale[j] + 1024;
                }
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = 0;
                matrix.t[0] = group->origins[j].t[0] + group->offsets[j].vx * progress / 512;
                matrix.t[1] = group->origins[j].t[1] + group->offsets[j].vy * progress / 512;
                matrix.t[2] = group->origins[j].t[2] + group->offsets[j].vz * progress / 512;
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
                if (depth >= 0 && flag >= 0 && group->done[j] == 0) {
                    GsSortPoly(polygon, ot, (u16)depth);
                }
            }
            if (group->scale[j] < 1024) {
                group->scale[j] += state->clock.step * 24;
                if (group->scale[0] >= 512 && state->phase == 0) {
                    state->phase = 2;
                }
                if (group->scale[j] >= 1024) {
                    if (state->config->limit <= state->clock.frame) {
                        group->scale[j] = 1024;
                        group->done[j] = 1;
                    } else {
                        group->scale[j] -= 2048;
                        group->initialized[j] = 0;
                    }
                } else if (group->scale[j] <= -1024) {
                    if (state->config->limit <= state->clock.frame) {
                        group->scale[j] = 1024;
                        group->done[j] = 1;
                    }
                }
            }
            complete *= group->done[j];
            if (j + 1 == 16 && complete == 1 && state->phase == 2) {
                state->phase = 3;
            }
        }
    }
}
