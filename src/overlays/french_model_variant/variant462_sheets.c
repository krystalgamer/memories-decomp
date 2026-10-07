#include "../../types.h"
#include "variant462_sheets.h"

void func_8013CEF8(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheets462State *state = (Sheets462State *)context;
    Sheets462Primary *primary = state->primary;
    ModelVariantSheetSet *sheet = state->sheets;
    POLY_GT4 *polygon = state->polygons;
    Sheets462Timing *timing;
    GsOT *ot;
    s32 count, first;
    s32 i, j;
    s32 extra;
    s32 depth;

    ot = func_80058F10();
    timing = state->timing;
    if (timing->paired == 0) {
        count = timing->first_count + 1;
        first = timing->first_count;
    } else {
        count = timing->first_count * 2;
        first = timing->first_count;
    }
    polygon++;
    for (i = 0; i < count; i++, sheet++) {
        extra = 0;
        if (state->frame & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i < first) {
            matrix.t[0] = state->positions[i].vx;
            matrix.t[1] = state->positions[i].vy;
            matrix.t[2] = state->positions[i].vz;
            scale.vx = sheet->size + extra;
            scale.vy = sheet->size + extra;
            scale.vz = sheet->size + extra;
        } else {
            matrix.t[0] = state->other_positions[i - first].vx;
            matrix.t[1] = state->other_positions[i - first].vy;
            matrix.t[2] = state->other_positions[i - first].vz;
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
                                 (PSXLONG *)&polygon->x0, (PSXLONG *)&polygon->x1,
                                 (PSXLONG *)&polygon->x2, (PSXLONG *)&polygon->x3,
                                 &interpolation, &flag);
            polygon->r0 = sheet->inner[0];
            polygon->g0 = sheet->inner[1];
            polygon->b0 = sheet->inner[2];
            polygon->r1 = sheet->inner[0];
            polygon->g1 = sheet->inner[1];
            polygon->b1 = sheet->inner[2];
            polygon->r2 = sheet->inner[0];
            polygon->g2 = sheet->inner[1];
            polygon->b2 = sheet->inner[2];
            polygon->r3 = sheet->outer[0];
            polygon->g3 = sheet->outer[1];
            polygon->b3 = sheet->outer[2];
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (i < first) {
            if (state->phase == 0) {
                if (sheet->size < 4096) {
                    sheet->size = ((state->time - state->timing->grow_begin) << 12) /
                                  (state->timing->grow_end - state->timing->grow_begin);
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        if (i + 1 == first) {
                            state->phase = 1;
                        }
                    }
                }
            } else if (state->phase == 5 && sheet->size > 0) {
                if (state->time >= state->timing->fade_begin) {
                    sheet->size = 4096 - ((state->time - state->timing->fade_begin) << 12) /
                                        (state->timing->fade_end - state->timing->fade_begin);
                }
                if (sheet->size <= 0) {
                    sheet->size = 0;
                    state->phase = 6;
                }
            }
        } else {
            if (primary->active == 1) {
                if (sheet->shown == 0) {
                    sheet->size = 16384;
                    sheet->shown = 1;
                } else {
                    sheet->size = primary->size * 16;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        if (state->phase < 5) {
                            sheet->shown = 0;
                        }
                    }
                }
            } else {
                sheet->size = 0;
            }
            primary++;
        }
    }
}
