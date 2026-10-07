#include "../../types.h"
#include "variant455_swarm.h"

void func_8013C3B0(u8 *context)
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
    Swarm455State *state = (Swarm455State *)context;
    Swarm455Group *group = &state->group;
    POLY_FT4 *polygon = &state->polygon;
    s16 j;
    s16 angle, spread;
    s32 complete;
    u8 red, green, blue;
    s16 radius, size;
    s32 depth;

    ot = func_80058F10();
    complete = 1;
    j = 0;
    ratan2(state->direction.vy, state->direction.vz);
    spread = 0;
    angle = 0;
    ratan2(state->projected.vy, state->projected.vx);
    for (; j < state->timing->count; j++, spread = j * 1024, angle += 64) {
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
            radius = 480 - (rcos(group->size[j]) * 480 >> 12);
            size = (rsin(group->size[j]) * 3596 >> 12) + 500;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = group->origins[j].t[0] + state->directions[j].vx * group->size[j] / 512 +
                          (rcos(spread - 1024 + angle) * radius >> 12);
            matrix.t[1] = group->origins[j].t[1] + state->directions[j].vy * group->size[j] / 512 +
                          (rsin(spread - 1024 + angle) * radius >> 12);
            matrix.t[2] = group->origins[j].t[2] + state->directions[j].vz * group->size[j] / 512;
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
            if (depth >= 0 && flag >= 0 && group->done[j] == 0) {
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
            } else if (group->size[j] <= 0) {
                if (state->timing->limit <= state->time) {
                    group->size[j] = 1024;
                    group->done[j] = 1;
                }
            }
        }
        complete *= group->done[j];
        if (j + 1 == state->timing->count && complete == 1 && state->phase == 2) {
            state->phase = 5;
        }
    }
}
