#include "../../types.h"
#include "../french_model_variant/variant437_entry.h"
#include "../../game/gpu_packets.h"

void SpanishModelVariant437_DrawSheet(Variant437EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    /* Unused; retains the target's 16-byte stack gap after the coordinate. */
    VECTOR unused;
    long interpolation, flag;
    Variant437EntryState *state = context;
    Sheet437 *sheet;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j, extra, depth;
    s32 outer_r, outer_g, outer_b;
    s32 inner_r, inner_g, inner_b;

    ot = Graphics_GetDefaultOrderingTable();
    ratan2(state->view_delta.vz, state->view_delta.vx);
    sheet = state->sheets;
    ratan2(state->view_delta.vy, state->view_delta.vx);
    polygon = state->packets.textured;
    if ((state->frame_count & 1) == 0) {
        extra = 0;
    } else {
        extra = state->scale / 8;
    }
    polygon++;
    for (i = 0; i < 1; i++, sheet++) {
        matrix.t[0] = state->origin.vx + state->path.vx * state->factor / 1024;
        matrix.t[1] = state->origin.vy + state->path.vy * state->factor / 1024;
        matrix.t[2] = state->origin.vz + state->path.vz * state->factor / 1024;
        if (state->tail.phase < 6) {
            scale.vx = state->scale + extra;
            scale.vy = state->scale + extra;
            scale.vz = state->scale + extra;
            polygon->r0 = sheet->inner[0];
            polygon->g0 = sheet->inner[1];
            polygon->b0 = sheet->inner[2];
            polygon->r1 = sheet->inner[0];
            polygon->g1 = sheet->inner[1];
            polygon->b1 = sheet->inner[2];
            polygon->r2 = sheet->outer[0];
            polygon->g2 = sheet->outer[1];
            polygon->b2 = sheet->outer[2];
            polygon->r3 = sheet->inner[0];
            polygon->g3 = sheet->inner[1];
            polygon->b3 = sheet->inner[2];
        } else {
            outer_r = sheet->outer[0] * (32768 - state->scale) / 16384;
            outer_g = sheet->outer[1] * (32768 - state->scale) / 16384;
            outer_b = sheet->outer[2] * (32768 - state->scale) / 16384;
            inner_r = sheet->inner[0] * (32768 - state->scale) / 16384;
            inner_g = sheet->inner[1] * (32768 - state->scale) / 16384;
            inner_b = sheet->inner[2] * (32768 - state->scale) / 16384;
            scale.vx = state->scale;
            scale.vy = state->scale;
            scale.vz = state->scale;
            polygon->r0 = inner_r;
            polygon->g0 = inner_g;
            polygon->b0 = inner_b;
            polygon->r1 = inner_r;
            polygon->g1 = inner_g;
            polygon->b1 = inner_b;
            polygon->r2 = outer_r;
            polygon->g2 = outer_g;
            polygon->b2 = outer_b;
            polygon->r3 = inner_r;
            polygon->g3 = inner_g;
            polygon->b3 = inner_b;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
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
            depth = RotTransPers4(&sheet->v0[j], &sheet->v1[j], &sheet->v2[j], &sheet->v3[j],
                                 (long *)&polygon->x1, (long *)&polygon->x0,
                                 (long *)&polygon->x3, (long *)&polygon->x2,
                                 &interpolation, &flag);
            depth = depth * 9 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
    }
    if (state->tail.phase < 2) {
        if (state->scale < 4096) {
            state->scale = (state->elapsed - state->config->scale_start) * 4096 /
                          (state->config->scale_end - state->config->scale_start);
            if (state->scale >= 4096) {
                state->scale = 4096;
                state->tail.phase = 2;
            }
        }
    } else if (state->tail.phase == 2) {
        state->scale = 4096 + (state->elapsed - state->config->scale_end) * 1024 /
                      (state->config->move_start - state->config->scale_end);
        if (state->scale >= 5120) {
            state->scale = 5120;
            state->tail.phase = 4;
        }
    } else if (state->tail.phase == 4) {
        state->factor = (state->elapsed - state->config->move_start) * 1024 /
                          (state->config->move_end - state->config->move_start);
        if (state->factor >= 1024) {
            state->factor = 1024;
            state->tail.phase = 5;
        }
    } else if (state->tail.phase == 5) {
        if (state->scale < 16384) {
            state->scale += state->step * 128;
            if (state->scale >= 16384) {
                state->scale = 16384;
                state->tail.phase = 6;
            }
        }
    } else if (state->tail.phase == 6) {
        if (state->scale < 32767) {
            state->scale += state->step * 1024;
            if (state->scale >= 32767) {
                state->scale = 32767;
                state->tail.phase = 7;
            }
        }
    }
}
