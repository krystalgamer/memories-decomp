#include "../../types.h"
#include "variant444_sheets.h"
#include "../../game/gpu_packets.h"

void func_8013CDC4(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheet444State *state = (Sheet444State *)context;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, j;
    s32 bias;
    s32 depth;
    s32 r, g, b, outer_r, outer_g, outer_b;

    ot = func_80058F10();
    quad = &state->quad;
    for (i = 0; i < 2; i++, sheet++) {
        bias = 0;
        if (state->frame & 1) {
            bias = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = state->origin[0];
            matrix.t[1] = state->origin[1];
            matrix.t[2] = state->origin[2];
        } else {
            matrix.t[0] = state->origin[0] + state->path[0] * state->progress / 1024;
            matrix.t[1] = state->origin[1] + state->path[1] * state->progress / 1024;
            matrix.t[2] = state->origin[2] + state->path[2] * state->progress / 1024;
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
                                 &interpolation, &flag);
            if (state->phase >= 3 && i != 0 && state->fade < 512) {
                r = sheet->inner[0] * state->fade / 512;
                g = sheet->inner[1] * state->fade / 512;
                b = sheet->inner[2] * state->fade / 512;
                outer_r = sheet->outer[0] * state->fade / 512;
                outer_g = sheet->outer[1] * state->fade / 512;
                outer_b = sheet->outer[2] * state->fade / 512;
                quad->r0 = r;
                quad->g0 = g;
                quad->b0 = b;
                quad->r1 = r;
                quad->g1 = g;
                quad->b1 = b;
                quad->r2 = r;
                quad->g2 = g;
                quad->b2 = b;
                quad->r3 = outer_r;
                quad->g3 = outer_g;
                quad->b3 = outer_b;
            } else {
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
            }
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, (u16)depth);
            }
        }
        if (i == 0) {
            if (state->phase == 0 && sheet->size < 4096) {
                sheet->size = (state->time - state->timing->grow_start) * 4096
                    / (state->timing->grow_end - state->timing->grow_start);
                if (sheet->size >= 4096) {
                    sheet->size = 4096;
                    state->phase = 1;
                }
            }
            if (state->phase == 3 && state->time >= state->timing->fade_start && sheet->size > 0) {
                sheet->size = 4096 - (state->time - state->timing->fade_start) * 4096
                    / (state->timing->fade_end - state->timing->fade_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else if (state->phase == 1) {
            sheet->size = 4096;
        } else if (state->phase == 2) {
            if (sheet->size < 8192) {
                sheet->size += state->step * 512;
                if (sheet->size >= 8192) {
                    sheet->size = 8192;
                    state->phase = 3;
                }
            }
        } else if (state->phase == 3) {
            if (state->time >= state->timing->fade_start && sheet->size < 16384) {
                sheet->size = 8192 + (state->time - state->timing->fade_start) * 16384
                    / (state->timing->fade_end - state->timing->fade_start);
                if (sheet->size >= 24576) {
                    sheet->size = 24576;
                }
            }
        }
    }
}
