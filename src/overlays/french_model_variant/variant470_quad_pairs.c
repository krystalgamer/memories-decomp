#include "../../types.h"
#include "variant470_quad_pairs.h"

void func_8013C510(u8 *context)
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
    QuadPairState470 *state = (QuadPairState470 *)context;
    QuadPairGroup470 *group = state->groups;
    POLY_FT4 *polygon = &state->polygon;
    s16 i, j;
    s32 complete;
    u8 red, green, blue;
    s32 depth;

    ot = func_80058F10();
    complete = 1;
    i = 0;
    ratan2(state->direction.vy, state->direction.vz);
    ratan2(state->projected.vy, state->projected.vx);
    for (; i < 2; i++, group++) {
        for (j = 0; j < 10; j++) {
            if (group->size[j] >= 0) {
                if (group->size[j] > 512) {
                    red = group->color[0] * (1024 - group->size[j]) / 512;
                    green = group->color[1] * (1024 - group->size[j]) / 512;
                    blue = group->color[2] * (1024 - group->size[j]) / 512;
                } else {
                    red = group->color[0];
                    green = group->color[1];
                    blue = group->color[2];
                }
                rcos(group->size[j]);
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = 0;
                matrix.t[0] = group->anchor[j].position.vx + group->direction[j].vx * group->size[j] / 512;
                matrix.t[1] = group->anchor[j].position.vy + group->direction[j].vy * group->size[j] / 512;
                matrix.t[2] = group->anchor[j].position.vz + group->direction[j].vz * group->size[j] / 512;
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
                                     (PSXLONG *)&polygon->x0, (PSXLONG *)&polygon->x1,
                                     (PSXLONG *)&polygon->x2, (PSXLONG *)&polygon->x3,
                                     &interpolation, &flag);
                polygon->r0 = red;
                polygon->g0 = green;
                polygon->b0 = blue;
                if (depth >= 0 && flag >= 0 && group->done[j] == 0) {
                    GsSortPoly(polygon, ot, (u16)depth);
                }
            }
            if (group->size[j] < 1024) {
                group->size[j] += state->step * 24;
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
                } else if (group->size[j] <= 0) {
                    if (state->timing->limit <= state->time) {
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
