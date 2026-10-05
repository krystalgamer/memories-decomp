#include "../../types.h"
#include "variant479_sheets.h"

void func_8013DB90(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    GsOT *ot;
    Sheets479State *state = (Sheets479State *)context;
    Sheets479Primary *primary = state->primary;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *polygon;
    SVECTOR *vertex;
    s32 i, j;
    s32 extra;
    s32 depth;

    ot = func_80058F10();
    polygon = &state->polygon;
    for (i = 0; i < 8; i++, sheet++) {
        extra = 0;
        if (state->frame & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i < 5) {
            matrix.t[0] = state->positions[i % 3].translation.vx + primary->position.vx;
            matrix.t[1] = state->positions[i % 3].translation.vy + primary->position.vy;
            matrix.t[2] = state->positions[i % 3].translation.vz + primary->position.vz;
        } else {
            matrix.t[0] = state->positions[i - 5].translation.vx;
            matrix.t[1] = state->positions[i - 5].translation.vy;
            matrix.t[2] = state->positions[i - 5].translation.vz;
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
        j = 0;
        vertex = sheet->v0;
        for (; j < 4; j++, vertex++) {
            depth = RotTransPers4(vertex, &sheet->v1[j], &sheet->v2[j], &sheet->v3[j],
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
            depth = depth * 8 / 10;
            if ((i >= 5 || primary->active != 0) && depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (i < 5) {
            if (state->phase == 2) {
                sheet->size = 4096;
            } else if (state->phase == 3 && sheet->size < 8192) {
                sheet->size += state->step * 512;
                if (sheet->size >= 8192) {
                    sheet->size = 8192;
                    state->phase = 4;
                }
            }
            if (state->time >= state->timing->fade_begin && sheet->size > 0) {
                sheet->size = 8192 - ((state->time - state->timing->fade_begin) << 13) /
                                    (state->timing->fade_end - state->timing->fade_begin);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
            primary++;
        } else {
            if (state->phase == 0 && sheet->size < 4096) {
                sheet->size = ((state->time - state->timing->grow_begin) << 12) /
                              (state->timing->grow_end - state->timing->grow_begin);
                if (sheet->size >= 4096) {
                    sheet->size = 4096;
                    state->phase = 1;
                }
            }
            if (state->time >= state->timing->fade_begin && sheet->size > 0) {
                sheet->size = 4096 - ((state->time - state->timing->fade_begin) << 12) /
                                    (state->timing->fade_end - state->timing->fade_begin);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        }
    }
}
