#include "../../types.h"
#include "variant425_sheets.h"

void func_8013D6DC(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheet425State *state = (Sheet425State *)context;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j;
    s32 extra;
    s32 x, y, z;
    s32 depth;

    ot = func_80058F10();
    polygon = &state->polygon;
    for (i = 0; i < 2; i++, sheet++) {
        extra = 0;
        if (state->frame & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = state->origin[0];
            matrix.t[1] = state->origin[1];
            matrix.t[2] = state->origin[2];
        } else {
            x = state->direction[0] * state->progress / 1024;
            y = state->direction[1] * state->progress / 1024;
            z = state->direction[2] * state->progress / 1024;
            matrix.t[0] = state->origin[0] + x;
            matrix.t[1] = state->origin[1] + y;
            matrix.t[2] = state->origin[2] + z;
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
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (i == 0) {
            if (state->phase == 0 && sheet->size < 4096) {
                sheet->size = (state->time - state->descriptor->grow_start) * 4096
                    / (state->descriptor->grow_end - state->descriptor->grow_start);
                if (sheet->size >= 4096) {
                    sheet->size = 4096;
                    state->phase = 1;
                }
            }
            if (state->time >= state->descriptor->shrink_start && sheet->size > 0) {
                sheet->size = 4096 - (state->time - state->descriptor->shrink_start) * 4096
                    / (state->descriptor->shrink_end - state->descriptor->shrink_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else {
            if (state->phase == 2) {
                sheet->size = 4096;
            } else if (state->phase == 3) {
                if (sheet->size < 16384) {
                    sheet->size += state->step * 2048;
                    if (sheet->size >= 16384) {
                        sheet->size = 16384;
                        state->phase = 4;
                    }
                }
            }
            if (state->time >= state->descriptor->shrink_start && sheet->size > 0) {
                sheet->size = 16384 - (state->time - state->descriptor->shrink_start) * 16384
                    / (state->descriptor->shrink_end - state->descriptor->shrink_start);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        }
    }
}
