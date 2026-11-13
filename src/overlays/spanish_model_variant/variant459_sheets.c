#include "../../types.h"
#include "variant459_entry.h"
#include "../../game/gpu_packets.h"

void SpanishModelVariant459_DrawPairedSheets(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant459EntryState *state = (Variant459EntryState *)context;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, j;
    s32 bias;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    quad = &state->packets.textured[1];
    for (i = 0; i < state->config->count * 2 + 1; i++, sheet++) {
        bias = 0;
        if (state->clock.frame_count & 1) {
            bias = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i + 1 == state->config->count * 2 + 1) {
            matrix.t[0] = state->aim.target.vx;
            matrix.t[1] = state->aim.target.vy;
            matrix.t[2] = state->aim.target.vz;
        } else if (i < state->config->count) {
            matrix.t[0] = state->points[i].vx + state->directions[i].vx * state->progress / 1024;
            matrix.t[1] = state->points[i].vy + state->directions[i].vy * state->progress / 1024;
            matrix.t[2] = state->points[i].vz + state->directions[i].vz * state->progress / 1024;
        } else {
            matrix.t[0] = state->points[i - state->config->count].vx;
            matrix.t[1] = state->points[i - state->config->count].vy;
            matrix.t[2] = state->points[i - state->config->count].vz;
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
                                 (long *)&quad->x0, (long *)&quad->x1,
                                 (long *)&quad->x2, (long *)&quad->x3,
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
        if (i + 1 == state->config->count * 2 + 1) {
            if (state->phase == 0) {
                sheet->size = 0;
            } else if (state->progress >= 1024 && state->phase == 2) {
                if (sheet->size < 8192) {
                    sheet->size += state->clock.step * 1024;
                    if (sheet->size >= 8192) {
                        sheet->size = 8192;
                    }
                }
            } else if (state->phase == 3) {
                if (sheet->size > 0) {
                    sheet->size -= state->clock.step * 64;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        state->phase = 4;
                    }
                }
            }
        } else if (state->phase == 0) {
            if (sheet->size < state->config->limit) {
                sheet->size = state->config->limit * (state->clock.frame - state->config->start)
                    / (state->config->end - state->config->start);
                if (sheet->size >= state->config->limit) {
                    sheet->size = state->config->limit;
                    state->phase = 1;
                }
            }
        } else {
            sheet->size = state->config->limit * state->scale / 4096;
        }
    }
}
