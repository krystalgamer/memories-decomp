#include "../../types.h"
#include "variant397_sheets.h"

void func_8013CA9C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheets397State *state = (Sheets397State *)context;
    ModelVariantSheetSet *sheet = state->sheets;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, j;
    s32 shown;
    s32 extra;
    s32 depth;

    ot = func_80058F10();
    shown = 0;
    quad = &state->quad;
    for (i = 0; i < state->timing->count; i++, sheet++) {
        extra = 0;
        if (state->frame & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (state->phase < 4) {
            matrix.t[0] = state->anchors[i].position.vx;
            matrix.t[1] = state->anchors[i].position.vy;
            matrix.t[2] = state->anchors[i].position.vz;
            scale.vx = sheet->size / state->timing->divisor + extra;
            scale.vy = sheet->size / state->timing->divisor + extra;
            scale.vz = sheet->size / state->timing->divisor + extra;
        } else {
            matrix.t[0] = state->positions[i].vx;
            matrix.t[1] = state->positions[i].vy;
            matrix.t[2] = state->positions[i].vz;
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
        if (state->phase == 0) {
            if (sheet->size < 4096) {
                sheet->size = ((state->time - state->timing->grow_begin) << 12) /
                              (state->timing->grow_end - state->timing->grow_begin);
                if (sheet->size >= 4096) {
                    sheet->size = 4096;
                    state->phase = 1;
                }
            }
        } else if (state->phase == 1 && state->timing->fade_at < state->time) {
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
        if (shown == state->timing->count * 2 && state->phase == 5) {
            state->phase = 6;
        }
    }
}
