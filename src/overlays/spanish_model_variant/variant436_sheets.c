#include "../../types.h"
#include "variant436_sheets.h"
#include "../../game/gpu_packets.h"

void func_8013C53C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    /* Unused; retains the target's 16-byte stack gap after the coordinate. */
    VECTOR unused;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheet436State *state = (Sheet436State *)context;
    Sheet436 *sheet = state->sheets;
    POLY_GT4 *quad = state->packets;
    GsOT *ot;
    s32 i, j;
    s32 bias;
    s32 depth;

    ot = func_80058F10();
    ratan2(state->direction[2], state->direction[0]);
    ratan2(state->direction[1], state->direction[0]);
    if ((state->frame & 1) == 0) {
        bias = 0;
    } else {
        bias = state->size / 8;
    }
    quad++;
    for (i = 0; i < 1; i++, sheet++) {
        matrix.t[0] = state->origin[0] + state->path[0] * state->progress / 1024;
        matrix.t[1] = state->origin[1] + state->path[1] * state->progress / 1024;
        matrix.t[2] = state->origin[2] + state->path[2] * state->progress / 1024;
        scale.vx = state->size + bias;
        scale.vy = state->size + bias;
        scale.vz = state->size + bias;
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
                                 (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                 (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
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
    if (state->phase == 0) {
        if (state->size < 4096) {
            state->size = (state->time - state->timing->grow_start) * 4096
                / (state->timing->grow_end - state->timing->grow_start);
            if (state->size >= 4096) {
                state->size = 4096;
                state->phase = 1;
            }
        }
    } else if (state->phase == 2) {
        state->phase2_progress = (state->time - state->timing->grow_end) * 1024
            / (state->timing->phase2_end - state->timing->grow_end);
        if (state->phase2_progress >= 1024) {
            state->phase2_progress = 1024;
            state->phase = 3;
        }
    } else if (state->phase == 3) {
        state->phase3_progress = (state->time - state->timing->phase2_end) * 1024
            / (state->timing->phase3_end - state->timing->phase2_end);
        if (state->phase3_progress >= 1024) {
            state->phase3_progress = 1024;
            state->phase = 4;
        }
    } else if (state->phase == 4) {
        state->progress = (state->time - state->timing->phase3_end) * 1024
            / (state->timing->travel_end - state->timing->phase3_end);
        if (state->progress >= 1024) {
            state->progress = 1024;
            state->phase = 5;
        }
    } else if (state->phase == 5) {
        if (state->size < 16384) {
            state->size += state->step * 2048;
            if (state->size >= 16384) {
                state->size = 16384;
                state->phase = 6;
            }
        }
    } else if (state->phase == 6) {
        if (state->size > 0) {
            state->size -= state->step * 128;
            if (state->size <= 0) {
                state->size = 0;
                state->phase = 7;
            }
        }
    }
}
