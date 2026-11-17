#include "../../types.h"
#include "../model_variant/e338_entry.h"

void FrenchModelVariant389_DrawSheets(Model372State *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Model372State *state = context;
    Model372Ribbon *primary = state->ribbons;
    ModelVariantSheetSet *sheet = state->sheets;
    POLY_GT4 *quad = state->packets.textured;
    GsOT *ot;
    s32 count, first;
    s32 i, j;
    s32 shown;
    s32 extra;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    shown = 0;
    if (state->config->paired == 0) {
        count = 9;
        first = 8;
    } else {
        count = 16;
        first = 8;
    }
    quad++;
    for (i = 0; i < count; i++, sheet++) {
        if (sheet->size > 0) {
            extra = 0;
            if (state->frame & 1) {
                extra = sheet->size / 8;
            }
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            if (i < first) {
                matrix.t[0] = state->transforms[i].t[0];
                matrix.t[1] = state->transforms[i].t[1];
                matrix.t[2] = state->transforms[i].t[2];
            } else {
                matrix.t[0] = state->positions[i - first].vx;
                matrix.t[1] = state->positions[i - first].vy;
                matrix.t[2] = state->positions[i - first].vz;
            }
            scale.vx = sheet->size + extra;
            scale.vy = sheet->size + extra;
            scale.vz = sheet->size + extra;
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
                if (state->config->single != 0 && i != 0 && i < first) {
                    continue;
                }
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (i < first) {
            if (state->phase == 0) {
                if (sheet->size < 4096) {
                    sheet->size = ((state->time - state->config->sheet_start) << 12) /
                                  (state->config->ribbon_start - state->config->sheet_start);
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        if (i + 1 == 8) {
                            state->phase = 1;
                        }
                    }
                }
            } else if (state->config->fade_begin < state->time && sheet->size > 0) {
                sheet->size = 4096 - ((state->time - state->config->fade_begin) << 12) /
                                    (state->config->fade_end - state->config->fade_begin);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                    if (state->phase < 3) {
                        state->phase = 3;
                    }
                }
            }
            primary++;
        } else {
            if (i == first) {
                primary = state->ribbons;
            }
            if (state->config->paired == 0 && primary[first - 1].progress[0] >= 1024 && sheet->size >= 8192) {
                sheet->shown = 1;
            }
            if (primary->progress[0] >= 1024 && sheet->shown == 0) {
                if (sheet->size < 16384) {
                    sheet->size += state->step << 12;
                    if (sheet->size >= 16384) {
                        sheet->size = 16384;
                        if (state->config->paired != 0) {
                            sheet->shown = 1;
                        }
                    }
                }
            } else {
                if (sheet->size > 0) {
                    sheet->size -= state->step * 768;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                    }
                }
                if (sheet->size <= 0 && state->phase == 4) {
                    sheet->shown = 2;
                }
            }
            shown += sheet->shown;
            primary++;
            if (shown == first * 2 && state->phase == 4) {
                state->phase = 5;
            }
        }
    }
}
