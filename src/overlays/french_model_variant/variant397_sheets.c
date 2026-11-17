#include "../../types.h"
#include "../model_variant/variant380_entry.h"

void FrenchModelVariant397_DrawSheets(Variant380EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant380EntryState *state = context;
    ModelVariantSheetSet *sheet = state->sheets;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, j;
    s32 shown;
    s32 extra;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    shown = 0;
    quad = &state->packets.textured[1];
    for (i = 0; i < state->config->count; i++, sheet++) {
        extra = 0;
        if (state->frame_count & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (state->phase < 4) {
            matrix.t[0] = state->matrices[i].t[0];
            matrix.t[1] = state->matrices[i].t[1];
            matrix.t[2] = state->matrices[i].t[2];
            scale.vx = sheet->size / state->config->divisor + extra;
            scale.vy = sheet->size / state->config->divisor + extra;
            scale.vz = sheet->size / state->config->divisor + extra;
        } else {
            matrix.t[0] = state->targets[i].vx;
            matrix.t[1] = state->targets[i].vy;
            matrix.t[2] = state->targets[i].vz;
            scale.vx = sheet->size + extra;
            scale.vy = sheet->size + extra;
            scale.vz = sheet->size + extra;
        }
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
        if (state->phase == 0) {
            if (sheet->size < 4096) {
                sheet->size = ((state->frame - state->config->start) << 12) /
                              (state->config->end - state->config->start);
                if (sheet->size >= 4096) {
                    sheet->size = 4096;
                    state->phase = 1;
                }
            }
        } else if (state->phase == 1 && state->config->fade_at < state->frame) {
            if (sheet->size > 0) {
                sheet->size -= state->step << 9;
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else if (state->phase == 4) {
            if (sheet->size < 8192 && sheet->shown == 0) {
                sheet->size += 4096;
                if (sheet->size >= 8192) {
                    sheet->size = 8192;
                    sheet->shown = 1;
                }
            } else if (sheet->size >= 0) {
                sheet->size -= 2048;
                if (sheet->size <= 0) {
                    sheet->size = 0;
                    sheet->shown = 0;
                }
            }
        } else if (state->phase == 5) {
            if (sheet->size >= 0) {
                sheet->size -= state->step << 8;
                if (sheet->size <= 0) {
                    sheet->size = 0;
                    sheet->shown = 2;
                }
            }
        }
        shown += sheet->shown;
        if (shown == state->config->count * 2 && state->phase == 5) {
            state->phase = 6;
        }
    }
}
