#include "../../types.h"
#include "variant446_lines.h"

void func_8013BE00(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Lines446State *state = (Lines446State *)context;
    Lines446 *group = state->groups;
    GsGLINE *line;
    GsOT *ot;
    s16 i, j, k;
    s32 size, value, fade;
    s32 progress;
    s32 depth;
    u8 red, green, blue;

    ot = func_80058F10();
    i = 0;
    ratan2(state->direction[2], state->direction[0]);
    ratan2(state->direction[1], state->direction[0]);
    ratan2(state->direction_a[1], state->direction_a[0]);
    ratan2(state->projected.vy, state->projected.vx);
    line = &state->line;
    for (; i < 3; i++, group++) {
        if (state->phase == 0) {
            if (group->size < 4096) {
                value = group->size;
                size = group->size;
            } else {
                value = 4096;
                size = 0;
            }
            if (value > 2048) {
                fade = 4096 - value;
                red = group->color.r * fade / 2048;
                green = group->color.g * fade / 2048;
                blue = group->color.b * fade / 2048;
            } else {
                red = group->color.r;
                green = group->color.g;
                blue = group->color.b;
            }
        } else if (state->phase == 1) {
            size = 4096;
            red = 0;
            green = 0;
            blue = 0;
        } else {
            if (group->size > 0) {
                value = group->size;
                size = group->size;
            } else {
                size = value = 0;
            }
            if (value > 6144) {
                fade = 8192 - value;
                red = group->color.r * fade / 2048;
                green = group->color.g * fade / 2048;
                blue = group->color.b * fade / 2048;
            } else {
                red = group->color.r;
                green = group->color.g;
                blue = group->color.b;
            }
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (state->phase < 2) {
            matrix.t[0] = state->origin[0];
            matrix.t[1] = state->origin[1];
            matrix.t[2] = state->origin[2];
        } else {
            matrix.t[0] = state->target.vx;
            matrix.t[1] = state->target.vy;
            matrix.t[2] = state->target.vz;
        }
        scale.vx = size;
        scale.vy = size;
        scale.vz = size;
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 6; k++) {
                line->attribute = 0x50000000;
                depth = RotTransPers4(&group->points[0][j][k], &group->points[1][j][k],
                                     &group->points[0][j][k], &group->points[1][j][k],
                                     (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                                     (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                                     &interpolation, &flag);
                if (state->phase < 2) {
                    line->r0 = 0;
                    line->g0 = 0;
                    line->b0 = 0;
                    line->r1 = red;
                    line->g1 = green;
                    line->b1 = blue;
                } else {
                    line->r0 = red;
                    line->g0 = green;
                    line->b0 = blue;
                    line->r1 = 0;
                    line->g1 = 0;
                    line->b1 = 0;
                }
                if (depth > 0) {
                    GsSortGLine(line, ot, (u16)depth);
                }
            }
        }
        if (state->phase == 0) {
            if (group->size > 0) {
                progress = (state->time * 3 * 4096) / state->timing->duration - 4096;
                group->size = i * 4096 / 3 - progress;
                if (group->size <= 0) {
                    group->size += 4096;
                }
            }
        } else if (state->phase == 1) {
            group->size = -(i * 8192 / 3);
        } else if (group->size < 8192) {
            group->size += state->step * 256;
            if (group->size >= 8192) {
                if (state->phase >= 5) {
                    group->size = 8192;
                    group->finished = 1;
                } else {
                    group->size -= 8192;
                }
            }
        }
    }
}
