#include "../../types.h"
#include "variant389_sheets.h"

void func_8013C864(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheets389State *state = (Sheets389State *)context;
    Sheets389Primary *primary = state->primary;
    ModelVariantSheetSet *sheet = state->sheets;
    POLY_GT4 *quad = state->quads;
    GsOT *ot;
    s32 count, first;
    s32 i, j;
    s32 shown;
    s32 extra;
    s32 depth;

    ot = func_80058F10();
    shown = 0;
    if (state->timing->paired == 0) {
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
                if (state->timing->single != 0 && i != 0 && i < first) {
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
                    sheet->size = ((state->time - state->timing->grow_begin) << 12) /
                                  (state->timing->grow_end - state->timing->grow_begin);
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        if (i + 1 == 8) {
                            state->phase = 1;
                        }
                    }
                }
            } else if (state->timing->fade_begin < state->time && sheet->size > 0) {
                sheet->size = 4096 - ((state->time - state->timing->fade_begin) << 12) /
                                    (state->timing->fade_end - state->timing->fade_begin);
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
                primary = state->primary;
            }
            if (state->timing->paired == 0 && primary[first - 1].level >= 1024 && sheet->size >= 8192) {
                sheet->shown = 1;
            }
            if (primary->level >= 1024 && sheet->shown == 0) {
                if (sheet->size < 16384) {
                    sheet->size += state->step << 12;
                    if (sheet->size >= 16384) {
                        sheet->size = 16384;
                        if (state->timing->paired != 0) {
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
