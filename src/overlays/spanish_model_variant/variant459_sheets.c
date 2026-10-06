#include "../../types.h"
#include "variant459_sheets.h"
#include "../../game/gpu_packets.h"

void func_8013EA24(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheet459State *state = (Sheet459State *)context;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, j;
    s32 bias;
    s32 depth;

    ot = func_80058F10();
    quad = &state->quad;
    for (i = 0; i < state->timing->count * 2 + 1; i++, sheet++) {
        bias = 0;
        if (state->frame & 1) {
            bias = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i + 1 == state->timing->count * 2 + 1) {
            matrix.t[0] = state->center.vx;
            matrix.t[1] = state->center.vy;
            matrix.t[2] = state->center.vz;
        } else if (i < state->timing->count) {
            matrix.t[0] = state->origins[i].vx + state->paths[i].vx * state->progress / 1024;
            matrix.t[1] = state->origins[i].vy + state->paths[i].vy * state->progress / 1024;
            matrix.t[2] = state->origins[i].vz + state->paths[i].vz * state->progress / 1024;
        } else {
            matrix.t[0] = state->origins[i - state->timing->count].vx;
            matrix.t[1] = state->origins[i - state->timing->count].vy;
            matrix.t[2] = state->origins[i - state->timing->count].vz;
        }
        scale.vx = sheet->size + bias;
        scale.vy = sheet->size + bias;
        scale.vz = sheet->size + bias;
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
            quad->r0 = sheet->inner[0];
            quad->g0 = sheet->inner[1];
            quad->b0 = sheet->inner[2];
            quad->r1 = sheet->inner[0];
            quad->g1 = sheet->inner[1];
            quad->b1 = sheet->inner[2];
            quad->r2 = sheet->inner[0];
            quad->g2 = sheet->inner[1];
            quad->b2 = sheet->inner[2];
            quad->r3 = sheet->outer[0];
            quad->g3 = sheet->outer[1];
            quad->b3 = sheet->outer[2];
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, (u16)depth);
            }
        }
        if (i + 1 == state->timing->count * 2 + 1) {
            if (state->phase == 0) {
                sheet->size = 0;
            } else if (state->progress >= 1024 && state->phase == 2) {
                if (sheet->size < 8192) {
                    sheet->size += state->step * 1024;
                    if (sheet->size >= 8192) {
                        sheet->size = 8192;
                    }
                }
            } else if (state->phase == 3) {
                if (sheet->size > 0) {
                    sheet->size -= state->step * 64;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        state->phase = 4;
                    }
                }
            }
        } else if (state->phase == 0) {
            if (sheet->size < state->timing->limit) {
                sheet->size = state->timing->limit * (state->time - state->timing->grow_start)
                    / (state->timing->grow_end - state->timing->grow_start);
                if (sheet->size >= state->timing->limit) {
                    sheet->size = state->timing->limit;
                    state->phase = 1;
                }
            }
        } else {
            sheet->size = state->timing->limit * state->scale / 4096;
        }
    }
}
