#include "../../types.h"
#include "image22822_blooms.h"

void FrenchModelVariant454_DrawBlooms(u8 *context)
{
    SVECTOR rotation;
    /* Unused; retains the target's 16-byte stack gap after the rotation. */
    /* Unused; retains the target's 16-byte stack gap after the rotation. */
    VECTOR unused;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Bloom22822State *state = (Bloom22822State *)context;
    Bloom22822Group *group = state->groups;
    POLY_FT4 *polygon = &state->polygon;
    GsOT *ot;
    s16 i, j;
    s32 complete;
    u8 red, green, blue;
    s16 size;
    s32 grow, spread, extra;
    s32 depth;
    s16 angle;

    ot = func_80058F10();
    complete = 1;
    ratan2(state->direction.vy, state->direction.vz);
    ratan2(state->projected.vy, state->projected.vx);
    for (i = 0; i < 8; i++, group++) {
        for (j = 0; j < 12; j++) {
            if (group->size[j] > 0 && group->size[j] < 3072) {
                if (group->size[j] > 2560) {
                    red = group->color[0] * (3072 - group->size[j]) / 512;
                    green = group->color[1] * (3072 - group->size[j]) / 512;
                    blue = group->color[2] * (3072 - group->size[j]) / 512;
                } else {
                    red = group->color[0];
                    green = group->color[1];
                    blue = group->color[2];
                }
                if (group->size[j] < 1024) {
                    grow = group->size[j];
                    spread = 0;
                    extra = 0;
                } else if (group->size[j] < 2048) {
                    grow = 1024;
                    spread = group->size[j] - 1024;
                    extra = 0;
                } else {
                    grow = 1024;
                    spread = 1024;
                    extra = group->size[j] - 2048;
                }
                if (group->size[j] < 1024) {
                    size = (rsin(group->size[j]) * 3596 >> 12) + 500;
                } else {
                    size = 4096;
                }
                rcos(group->size[j]);
                if (i < 5) {
                    angle = i * 4096 / 5;
                    rotation.vx = 0;
                    rotation.vy = 0;
                    rotation.vz = 0;
                    matrix.t[0] = group->origin.vx;
                    matrix.t[1] = group->origin.vy + group->direction.vy * grow / 1024 +
                                  extra * -4096 / 1024;
                    matrix.t[2] = group->origin.vz +
                                  (group->direction.vz + (rcos(angle) * 128 >> 12)) *
                                      spread / 1024;
                    scale.vx = size;
                    scale.vy = size;
                    scale.vz = size;
                } else {
                    rotation.vx = 0;
                    rotation.vy = 0;
                    rotation.vz = 0;
                    matrix.t[0] = group->origin.vx;
                    matrix.t[1] = group->origin.vy + group->direction.vy * grow / 1024;
                    if (state->mirror == 0) {
                        matrix.t[2] = group->origin.vz + (spread + extra) / 2;
                    } else {
                        matrix.t[2] = group->origin.vz - (spread + extra) / 2;
                    }
                    scale.vx = size;
                    scale.vy = size;
                    scale.vz = size;
                }
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
            if (group->size[j] < 3072) {
                group->size[j] += state->step * 16;
                if (group->size[j] >= 3072) {
                    if (state->timing->limit <= state->time) {
                        group->size[j] = 3072;
                        group->done[j] = 1;
                    } else {
                        group->size[j] -= 3072;
                    }
                }
                if (group->size[0] >= 2048 && state->phase == 0) {
                    state->phase = 1;
                }
            }
            if (state->timing->limit <= state->time && group->size[j] < 0) {
                group->done[j] = 1;
            }
            complete *= group->done[j];
            if (j + 1 == 12 && complete == 1 && state->phase == 1) {
                state->phase = 5;
            }
        }
    }
}
