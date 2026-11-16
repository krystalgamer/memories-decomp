#include "../../types.h"
#include "variant470_entry.h"

void FrenchModelVariant470_DrawQuadPairs(EntryState470 *context)
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
    EntryState470 *state = context;
    EntryPair470 *group = state->pairs;
    POLY_FT4 *polygon = &state->pair_packet;
    s16 i, j;
    s32 complete;
    u8 red, green, blue;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    complete = 1;
    i = 0;
    ratan2(state->aim.direction.vy, state->aim.direction.vz);
    ratan2(state->aim.screen_delta.vy, state->aim.screen_delta.vx);
    for (; i < 2; i++, group++) {
        for (j = 0; j < 10; j++) {
            if (group->size[j] >= 0) {
                if (group->size[j] > 512) {
                    red = group->color.r * (1024 - group->size[j]) / 512;
                    green = group->color.g * (1024 - group->size[j]) / 512;
                    blue = group->color.b * (1024 - group->size[j]) / 512;
                } else {
                    red = group->color.r;
                    green = group->color.g;
                    blue = group->color.b;
                }
                rcos(group->size[j]);
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = 0;
                matrix.t[0] = group->matrices[j].t[0] + group->directions[j].vx * group->size[j] / 512;
                matrix.t[1] = group->matrices[j].t[1] + group->directions[j].vy * group->size[j] / 512;
                matrix.t[2] = group->matrices[j].t[2] + group->directions[j].vz * group->size[j] / 512;
                scale.vx = 768;
                scale.vy = 768;
                scale.vz = 768;
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
                if (depth >= 0 && flag >= 0 && group->done[j] == 0) {
                    GsSortPoly(polygon, ot, (u16)depth);
                }
            }
            if (group->size[j] < 1024) {
                group->size[j] += state->clock.step * 24;
                if (group->size[0] >= 512 && state->phase == 0) {
                    state->phase = 2;
                }
                if (group->size[j] >= 1024) {
                    if (state->config->limit <= state->clock.frame) {
                        group->size[j] = 1024;
                        group->done[j] = 1;
                    } else {
                        group->size[j] -= 1024;
                        group->initialized[j] = 0;
                    }
                } else if (group->size[j] <= 0) {
                    if (state->config->limit <= state->clock.frame) {
                        group->size[j] = 1024;
                        group->done[j] = 1;
                    }
                }
            }
            complete *= group->done[j];
            if (j + 1 == 10 && complete == 1 && state->phase == 2) {
                state->phase = 3;
            }
        }
    }
}
