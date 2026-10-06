#include "../../types.h"
#include "variant437_sheet.h"
#include "../../game/gpu_packets.h"

void func_8013C4A8(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    /* Unused; retains the target's 16-byte stack gap after the coordinate. */
    VECTOR unused;
    PSXLONG interpolation, flag;
    Sheet437State *state = (Sheet437State *)context;
    Sheet437 *sheet;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j, extra, depth;
    s32 outer_r, outer_g, outer_b;
    s32 inner_r, inner_g, inner_b;

    ot = func_80058F10();
    ratan2(state->direction[2], state->direction[0]);
    sheet = state->sheets;
    ratan2(state->direction[1], state->direction[0]);
    polygon = state->polygons;
    if ((state->frame & 1) == 0) {
        extra = 0;
    } else {
        extra = state->size / 8;
    }
    polygon++;
    for (i = 0; i < 1; i++, sheet++) {
        matrix.t[0] = state->origin[0] + state->path[0] * state->progress / 1024;
        matrix.t[1] = state->origin[1] + state->path[1] * state->progress / 1024;
        matrix.t[2] = state->origin[2] + state->path[2] * state->progress / 1024;
        if (state->phase < 6) {
            scale.vx = state->size + extra;
            scale.vy = state->size + extra;
            scale.vz = state->size + extra;
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
            outer_r = sheet->outer[0] * (32768 - state->size) / 16384;
            outer_g = sheet->outer[1] * (32768 - state->size) / 16384;
            outer_b = sheet->outer[2] * (32768 - state->size) / 16384;
            inner_r = sheet->inner[0] * (32768 - state->size) / 16384;
            inner_g = sheet->inner[1] * (32768 - state->size) / 16384;
            inner_b = sheet->inner[2] * (32768 - state->size) / 16384;
            scale.vx = state->size;
            scale.vy = state->size;
            scale.vz = state->size;
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
                                 (PSXLONG *)&polygon->x1, (PSXLONG *)&polygon->x0,
                                 (PSXLONG *)&polygon->x3, (PSXLONG *)&polygon->x2,
                                 &interpolation, &flag);
            depth = depth * 9 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
    }
    if (state->phase < 2) {
        if (state->size < 4096) {
            state->size = (state->time - state->timing->grow_start) * 4096 /
                          (state->timing->grow_end - state->timing->grow_start);
            if (state->size >= 4096) {
                state->size = 4096;
                state->phase = 2;
            }
        }
    } else if (state->phase == 2) {
        state->size = 4096 + (state->time - state->timing->grow_end) * 1024 /
                      (state->timing->move_start - state->timing->grow_end);
        if (state->size >= 5120) {
            state->size = 5120;
            state->phase = 4;
        }
    } else if (state->phase == 4) {
        state->progress = (state->time - state->timing->move_start) * 1024 /
                          (state->timing->move_end - state->timing->move_start);
        if (state->progress >= 1024) {
            state->progress = 1024;
            state->phase = 5;
        }
    } else if (state->phase == 5) {
        if (state->size < 16384) {
            state->size += state->step * 128;
            if (state->size >= 16384) {
                state->size = 16384;
                state->phase = 6;
            }
        }
    } else if (state->phase == 6) {
        if (state->size < 32767) {
            state->size += state->step * 1024;
            if (state->size >= 32767) {
                state->size = 32767;
                state->phase = 7;
            }
        }
    }
}
