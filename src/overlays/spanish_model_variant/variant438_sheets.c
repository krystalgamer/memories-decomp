#include "../../types.h"
#include "variant438_sheets.h"

void func_8013EDE8(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheet438State *state = (Sheet438State *)context;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j;
    s32 pulse;
    s32 depth;

    ot = func_80058F10();
    polygon = &state->polygon;
    for (i = 0; i < 2; i++, sheet++) {
        pulse = 0;
        if (state->frame & 1) {
            pulse = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = state->origin[0];
            matrix.t[1] = state->origin[1];
            matrix.t[2] = state->origin[2];
        } else if (state->progress < 1024) {
            matrix.t[0] = state->origin[0] + state->direction[0] * state->progress / 1024;
            matrix.t[1] = state->origin[1] + state->direction[1] * state->progress / 1024;
            matrix.t[2] = state->origin[2] + state->direction[2] * state->progress / 1024;
        } else {
            matrix.t[0] = state->target.vx;
            matrix.t[1] = state->target.vy;
            matrix.t[2] = state->target.vz;
        }
        scale.vx = sheet->size + pulse;
        scale.vy = sheet->size + pulse;
        scale.vz = sheet->size + pulse;
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
        for (j = 0; j < 4; j++) {
            depth = RotTransPers4(&sheet->v0[j], &sheet->v1[j], &sheet->v2[j], &sheet->v3[j],
                                 (PSXLONG *)&polygon->x0, (PSXLONG *)&polygon->x1,
                                 (PSXLONG *)&polygon->x2, (PSXLONG *)&polygon->x3,
                                 &interpolation, &flag);
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (i == 0) {
            if (state->phase == 0) {
                if (sheet->size < 4096) {
                    sheet->size = (state->time - state->timing->grow_start) * 4096 /
                        (state->timing->grow_end - state->timing->grow_start);
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        state->phase = 1;
                    }
                }
            } else {
                sheet->size = state->fixed_size;
            }
        } else if (state->phase == 0) {
            sheet->size = 0;
        } else if (state->progress >= 1024 && state->phase == 2) {
            if (sheet->size < 8192) {
                sheet->size += state->step * 1024;
                if (sheet->size >= 8192) {
                    sheet->size = 8192;
                }
            }
        } else if (state->phase < 2) {
            sheet->size = 4096;
        } else if (state->phase == 3 && sheet->size > 0) {
            sheet->size -= state->step * 64;
            if (sheet->size <= 0) {
                sheet->size = 0;
                state->phase = 4;
            }
        }
    }
}
