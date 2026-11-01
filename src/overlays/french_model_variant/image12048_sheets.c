#include "../../types.h"
#include "image12048_sheets.h"

void FrenchModelVariant403_DrawSheets(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheets12048State *state = (Sheets12048State *)context;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *quad = state->quads;
    GsOT *ot;
    s32 i, j;
    s32 count;
    s32 extra;
    s32 depth;

    ot = func_80058F10();
    if (state->phase < 4) {
        count = state->timing->count;
    } else {
        count = 2;
    }
    quad++;
    for (i = 0; i < count; i++, sheet++) {
        extra = 0;
        if (state->frame & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (state->phase < 4) {
            matrix.t[0] = state->transforms[i].t[0];
            matrix.t[1] = state->transforms[i].t[1];
            matrix.t[2] = state->transforms[i].t[2];
            scale.vx = sheet->size / state->timing->divisor + extra;
            scale.vy = sheet->size / state->timing->divisor + extra;
            scale.vz = sheet->size / state->timing->divisor + extra;
        } else {
            /* The scale stores are repeated in both arms; GCC merges them, but
             * the extra arm keeps the quad pointer out of s2. */
            if (i == 0) {
                matrix.t[0] = state->origin[0];
                matrix.t[1] = state->origin[1];
                matrix.t[2] = state->origin[2];
                scale.vx = sheet->size + extra;
                scale.vy = sheet->size + extra;
                scale.vz = sheet->size + extra;
            } else {
                matrix.t[0] = state->offset.vx;
                matrix.t[1] = state->offset.vy;
                matrix.t[2] = state->offset.vz;
                scale.vx = sheet->size + extra;
                scale.vy = sheet->size + extra;
                scale.vz = sheet->size + extra;
            }
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
        for (j = 0; j < 4; j++) {
            depth = RotTransPers4(&sheet->v0[j], &sheet->v1[j], &sheet->v2[j], &sheet->v3[j],
                                 (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                 (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                 &interpolation, &flag) * 8 / 10;
            if (depth >= 0 && flag >= 0 && sheet->size > 0) {
                GsSortPoly(quad, ot, (u16)depth);
            }
        }
        if (state->phase == 0) {
            if (sheet->size < 4096) {
                sheet->size = (state->time << 12) / state->timing->grow_end;
                if (sheet->size >= 4096) {
                    sheet->size = 4096;
                    state->phase = 1;
                }
            }
        } else if (state->phase < 4) {
            sheet->size = 4096 - ((state->time - state->timing->grow_end) << 12) / state->fade_span;
        } else if (state->phase == 4) {
            if (i == 0) {
                if (sheet->size < 8192) {
                    sheet->size += state->step << 11;
                    if (sheet->size >= 8192) {
                        sheet->size = 8192;
                    }
                }
            } else if (sheet->size < 16384) {
                sheet->size += state->step << 12;
                if (sheet->size >= 16384) {
                    sheet->size = 16384;
                    state->phase = 5;
                }
            }
        } else if (state->phase == 5) {
            if (state->time < state->timing->shrink_end) {
                if (i == 0) {
                    sheet->size = 8192 - ((state->time - state->timing->shrink_begin) << 13) / state->shrink_span;
                } else {
                    sheet->size = 16384 - ((state->time - state->timing->shrink_begin) << 14) / state->shrink_span;
                }
            } else {
                state->phase = 6;
            }
        }
    }
}
