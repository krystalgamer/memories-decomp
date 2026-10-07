#include "../../types.h"
#include "variant471_quads.h"
#include "../../game/gpu_packets.h"

void func_8013CA34(u8 *context)
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
    Quads471State *state = (Quads471State *)context;
    QuadGroup471 *group = state->groups;
    POLY_FT4 *polygon;
    s16 i, j;
    s16 angle, ripple;
    s32 complete;
    u8 red, green, blue;
    s32 depth;

    ot = func_80058F10();
    complete = 1;
    i = 0;
    ratan2(state->direction.vy, state->direction.vz);
    polygon = &state->polygon;
    ratan2(state->projected.vy, state->projected.vx);
    for (; i < 1; i++, group++) {
        j = 0;
        angle = 0;
        ripple = 0;
        for (; j < 10; j++, angle += 1300, ripple += 1700) {
            if (group->size[j] >= 0) {
                if (group->size[j] > 3072) {
                    red = group->color[0] * (4096 - group->size[j]) / 1024;
                    green = group->color[1] * (4096 - group->size[j]) / 1024;
                    blue = group->color[2] * (4096 - group->size[j]) / 1024;
                } else {
                    red = group->color[0];
                    green = group->color[1];
                    blue = group->color[2];
                }
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = 0;
                matrix.t[0] = state->origin.vx + ((rcos(angle + ripple + group->angle[j]) * 256) >> 12);
                matrix.t[1] = state->origin.vy + ((rsin(angle + group->angle[j]) * 256) >> 12);
                matrix.t[2] = state->origin.vz + ((rsin(ripple + group->angle[j]) * 256) >> 12);
                scale.vx = group->size[j];
                scale.vy = group->size[j];
                scale.vz = group->size[j];
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
            if (group->size[j] < 4096) {
                group->size[j] += state->step * 256;
                if (group->size[j] >= 4096) {
                    if (state->timing->limit <= state->time && state->phase >= 3) {
                        group->size[j] = 4096;
                        group->done[j] = 1;
                    } else {
                        group->reset[j] = 0;
                        group->size[j] -= 4096;
                        group->angle[j] += 600;
                    }
                } else if (group->size[j] <= 0) {
                    if (state->timing->limit <= state->time) {
                        group->size[j] = 4096;
                        group->done[j] = 1;
                    }
                }
            }
            complete *= group->done[j];
            if (j + 1 == 10 && complete == 1 && state->phase == 3) {
                state->phase = 5;
            }
        }
    }
}
