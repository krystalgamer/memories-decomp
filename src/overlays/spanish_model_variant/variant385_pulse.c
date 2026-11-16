#include "../../types.h"
#include "variant385_entry.h"

void SpanishModelVariant385_DrawPulsingQuads(Variant385EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant385EntryState *state = context;
    Variant385EntrySheet *group = state->sheets;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, j;
    s32 pulse;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    quad = &state->packets.textured[1];
    for (i = 0; i < 2; i++, group++) {
        pulse = 0;
        if (state->clock.frame_count & 1) {
            pulse = group->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = state->matrix.t[0];
            matrix.t[1] = state->matrix.t[1];
            matrix.t[2] = state->matrix.t[2];
        } else {
            matrix.t[0] = state->matrix.t[0] + state->aim.direction.vx * state->progress / 1024;
            matrix.t[1] = state->matrix.t[1] + state->aim.direction.vy * state->progress / 1024;
            matrix.t[2] = state->matrix.t[2] + state->aim.direction.vz * state->progress / 1024;
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
            depth = RotTransPers4(&group->v0[j], &group->v1[j],
                                 &group->v2[j], &group->v3[j],
                                 (long *)&quad->x0, (long *)&quad->x1,
                                 (long *)&quad->x2, (long *)&quad->x3,
                                 &interpolation, &flag) * 8 / 10;
            setRGB0(quad, 0, 64, 192);
            setRGB1(quad, 0, 64, 192);
            setRGB2(quad, 0, 64, 192);
            setRGB3(quad, 192, 192, 192);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, (u16)depth);
            }
        }
        if (state->index + 1 == state->config->count) {
            if (i == 0) {
                if (state->phase == 0) {
                    if (group->size < 4096) {
                        group->size = (state->clock.frame - state->config->start) * 4096 /
                                      (state->config->grow_end - state->config->start);
                        if (group->size >= 4096) {
                            group->size = 4096;
                            state->phase = 1;
                        }
                    }
                } else if (state->config->fade_start <= state->clock.frame && group->size > 0) {
                    group->size = 4096 - (state->clock.frame - state->config->fade_start) * 4096 /
                                        (state->config->fade_end - state->config->fade_start);
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
                    group->size += state->clock.step * 1024;
                    if (group->size >= 8192) {
                        group->size = 8192;
                        state->phase = 3;
                    }
                }
            } else if (state->phase == 4) {
                if (group->size > 0) {
                    group->size -= state->clock.step * 64;
                    if (group->size <= 0) {
                        group->size = 0;
                        state->phase = 5;
                    }
                }
            }
        }
    }
}
