#include "../../types.h"
#include "../model_variant/variant446_entry_pal.h"
#include "../../game/gpu_packets.h"

void SpanishModelVariant446_DrawPulse(EntryState446 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    EntryState446 *state = context;
    EntryPulse446 *group = state->pulses;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, j;
    s32 pulse;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    quad = &state->textured[1];
    for (i = 0; i < 2; i++, group++) {
        pulse = 0;
        if (state->frame_count & 1) {
            pulse = group->scale.vx / 8;
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
        scale.vx = group->scale.vx + pulse;
        scale.vy = group->scale.vx + pulse;
        scale.vz = group->scale.vx + pulse;
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
                                 (long *)&quad->x0, (long *)&quad->x1,
                                 (long *)&quad->x2, (long *)&quad->x3,
                                 &interpolation, &flag);
            setRGB0(quad, 0, 64, 192);
            setRGB1(quad, 0, 64, 192);
            setRGB2(quad, 0, 64, 192);
            setRGB3(quad, 192, 192, 192);
            if (depth > 0) {
                GsSortPoly(quad, ot, (u16)(depth - 32));
            }
        }
        if (state->part_index + 1 == state->config->count) {
            if (i == 0) {
                if (state->phase == 0) {
                    if (group->scale.vx < 4096) {
                        group->scale.vx = (state->elapsed - state->config->draw_start) * 4096 /
                                      (state->config->duration - state->config->draw_start);
                        if (group->scale.vx >= 4096) {
                            group->scale.vx = 4096;
                            state->phase = 1;
                        }
                    }
                } else if (state->config->fade <= state->elapsed && group->scale.vx > 0) {
                    group->scale.vx = 1024 - (state->elapsed - state->config->fade) * 1024 /
                                        (state->config->end - state->config->fade);
                    if (group->scale.vx <= 0) {
                        group->scale.vx = 0;
                        if (state->phase == 3) {
                            state->phase = 4;
                        }
                    }
                }
            } else if (state->phase <= 0) {
                group->scale.vx = 0;
            } else if (state->phase == 1) {
                group->scale.vx = 4096;
            } else if (state->phase == 2) {
                if (group->scale.vx < 8192) {
                    group->scale.vx += state->step * 1024;
                    if (group->scale.vx >= 8192) {
                        group->scale.vx = 8192;
                        state->phase = 3;
                    }
                }
            } else if (state->phase == 4) {
                if (group->scale.vx > 0) {
                    group->scale.vx -= state->step * 64;
                    if (group->scale.vx <= 0) {
                        group->scale.vx = 0;
                        state->phase = 5;
                    }
                }
            }
        }
    }
}
