#include "../../types.h"
#include "variant472_sheets.h"

void func_8013D8B0(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheets472State *state = (Sheets472State *)context;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j;
    SVECTOR *point;
    s32 extra;
    s32 depth;

    ot = func_80058F10();
    polygon = &state->polygon;
    for (i = 0; i < 1; i++, sheet++) {
        extra = 0;
        if (state->frame & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        matrix.t[0] = state->world_matrix.t[0];
        matrix.t[1] = state->world_matrix.t[1];
        matrix.t[2] = state->world_matrix.t[2];
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
        for (j = 0, point = sheet->v0; j < 4; j++, point++) {
            depth = RotTransPers4(point, &sheet->v1[j],
                                 &sheet->v2[j], &sheet->v3[j],
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
            depth = depth * 7 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (sheet->size < 4096 && state->phase == 0) {
            sheet->size = ((state->time - state->timing->grow_begin) << 12) /
                          (state->timing->grow_end - state->timing->grow_begin);
            if (sheet->size >= 4096) {
                sheet->size = 4096;
                state->phase = 1;
            }
        } else if (state->time >= state->timing->fade_begin) {
            sheet->size = 4096 - ((state->time - state->timing->fade_begin) << 12) /
                                (state->timing->fade_end - state->timing->fade_begin);
            if (sheet->size <= 0) {
                sheet->size = 0;
                if (state->phase == 3) {
                    state->phase = 4;
                }
            }
        }
    }
}
