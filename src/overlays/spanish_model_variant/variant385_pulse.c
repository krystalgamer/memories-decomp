#include "../../types.h"
#include "variant385_pulse.h"

void func_8013D1B8(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Pulse385State *state = (Pulse385State *)context;
    Pulse385 *group = state->groups;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, j;
    s32 pulse;
    s32 depth;

    ot = func_80058F10();
    quad = &state->quad;
    for (i = 0; i < 2; i++, group++) {
        pulse = 0;
        if (state->frame & 1) {
            pulse = group->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = state->origin[0];
            matrix.t[1] = state->origin[1];
            matrix.t[2] = state->origin[2];
        } else {
            matrix.t[0] = state->origin[0] + state->direction[0] * state->progress / 1024;
            matrix.t[1] = state->origin[1] + state->direction[1] * state->progress / 1024;
            matrix.t[2] = state->origin[2] + state->direction[2] * state->progress / 1024;
        }
        scale.vx = group->size + pulse;
        scale.vy = group->size + pulse;
        scale.vz = group->size + pulse;
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
        for (j = 0; j < 4; j++) {
            depth = RotTransPers4(&group->points[0][j], &group->points[1][j],
                                 &group->points[2][j], &group->points[3][j],
                                 (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                 (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                 &interpolation, &flag) * 8 / 10;
            setRGB0(quad, 0, 64, 192);
            setRGB1(quad, 0, 64, 192);
            setRGB2(quad, 0, 64, 192);
            setRGB3(quad, 192, 192, 192);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, (u16)depth);
            }
        }
        if (state->index + 1 == state->timing->count) {
            if (i == 0) {
                if (state->phase == 0) {
                    if (group->size < 4096) {
                        group->size = (state->time - state->timing->start) * 4096 /
                                      (state->timing->end - state->timing->start);
                        if (group->size >= 4096) {
                            group->size = 4096;
                            state->phase = 1;
                        }
                    }
                } else if (state->timing->fade_start <= state->time && group->size > 0) {
                    group->size = 4096 - (state->time - state->timing->fade_start) * 4096 /
                                        (state->timing->fade_end - state->timing->fade_start);
                    if (group->size <= 0) {
                        group->size = 0;
                        if (state->phase == 3) {
                            state->phase = 4;
                        }
                    }
                }
            } else if (state->phase <= 0) {
                group->size = 0;
            } else if (state->phase == 1) {
                group->size = 4096;
            } else if (state->phase == 2) {
                if (group->size < 8192) {
                    group->size += state->step * 1024;
                    if (group->size >= 8192) {
                        group->size = 8192;
                        state->phase = 3;
                    }
                }
            } else if (state->phase == 4) {
                if (group->size > 0) {
                    group->size -= state->step * 64;
                    if (group->size <= 0) {
                        group->size = 0;
                        state->phase = 5;
                    }
                }
            }
        }
    }
}
