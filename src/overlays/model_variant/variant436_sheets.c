#include "../../types.h"
#include "../model_variant/variant436_entry.h"
#include "../../game/gpu_packets.h"

void SpanishModelVariant436_DrawSheets(EntryState436 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    /* Unused; retains the target's 16-byte stack gap after the coordinate. */
    VECTOR unused;
    long interpolation;
    long flag;
    EntryState436 *state = context;
    Sheet436 *sheet = state->sheets;
    POLY_GT4 *quad = state->packets.textured;
    GsOT *ot;
    s32 i, j;
    s32 bias;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    ratan2(state->view_delta.vz, state->view_delta.vx);
    ratan2(state->view_delta.vy, state->view_delta.vx);
    if ((state->frame_count & 1) == 0) {
        bias = 0;
    } else {
        bias = state->scale / 8;
    }
    quad++;
    for (i = 0; i < 1; i++, sheet++) {
        matrix.t[0] = state->origin.vx + state->path.vx * state->factor / 1024;
        matrix.t[1] = state->origin.vy + state->path.vy * state->factor / 1024;
        matrix.t[2] = state->origin.vz + state->path.vz * state->factor / 1024;
        scale.vx = state->scale + bias;
        scale.vy = state->scale + bias;
        scale.vz = state->scale + bias;
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
            depth = RotTransPers4(&sheet->v0[j], &sheet->v1[j],
                                 &sheet->v2[j], &sheet->v3[j],
                                 (long *)&quad->x0, (long *)&quad->x1,
                                 (long *)&quad->x2, (long *)&quad->x3,
                                 &interpolation, &flag) * 8 / 10;
            quad->r0 = sheet->inner.r;
            quad->g0 = sheet->inner.g;
            quad->b0 = sheet->inner.b;
            quad->r1 = sheet->inner.r;
            quad->g1 = sheet->inner.g;
            quad->b1 = sheet->inner.b;
            quad->r2 = sheet->inner.r;
            quad->g2 = sheet->inner.g;
            quad->b2 = sheet->inner.b;
            quad->r3 = sheet->outer.r;
            quad->g3 = sheet->outer.g;
            quad->b3 = sheet->outer.b;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, (u16)depth);
            }
        }
    }
    if (state->tail.phase == 0) {
        if (state->scale < 4096) {
            state->scale = (state->elapsed - state->config->scale_start) * 4096
                / (state->config->scale_end - state->config->scale_start);
            if (state->scale >= 4096) {
                state->scale = 4096;
                state->tail.phase = 1;
            }
        }
    } else if (state->tail.phase == 2) {
        state->lift = (state->elapsed - state->config->scale_end) * 1024
            / (state->config->phase2_end - state->config->scale_end);
        if (state->lift >= 1024) {
            state->lift = 1024;
            state->tail.phase = 3;
        }
    } else if (state->tail.phase == 3) {
        state->spread = (state->elapsed - state->config->phase2_end) * 1024
            / (state->config->phase3_end - state->config->phase2_end);
        if (state->spread >= 1024) {
            state->spread = 1024;
            state->tail.phase = 4;
        }
    } else if (state->tail.phase == 4) {
        state->factor = (state->elapsed - state->config->phase3_end) * 1024
            / (state->config->travel_end - state->config->phase3_end);
        if (state->factor >= 1024) {
            state->factor = 1024;
            state->tail.phase = 5;
        }
    } else if (state->tail.phase == 5) {
        if (state->scale < 16384) {
            state->scale += state->step * 2048;
            if (state->scale >= 16384) {
                state->scale = 16384;
                state->tail.phase = 6;
            }
        }
    } else if (state->tail.phase == 6) {
        if (state->scale > 0) {
            state->scale -= state->step * 128;
            if (state->scale <= 0) {
                state->scale = 0;
                state->tail.phase = 7;
            }
        }
    }
}
