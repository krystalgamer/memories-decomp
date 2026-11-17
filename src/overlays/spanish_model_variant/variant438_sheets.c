#include "../../types.h"
#include "variant438_entry.h"

void SpanishModelVariant438_DrawTargetClampedSheets(Variant438EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant438EntryState *state = context;
    ModelVariantSheet *sheet = state->sheets;
    POLY_GT4 *polygon;
    GsOT *ot;
    s32 i, j;
    s32 pulse;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    polygon = &state->packets.textured[1];
    for (i = 0; i < 2; i++, sheet++) {
        pulse = 0;
        if (state->clock.frame_count & 1) {
            pulse = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = state->matrix.t[0];
            matrix.t[1] = state->matrix.t[1];
            matrix.t[2] = state->matrix.t[2];
        } else if (state->progress < 1024) {
            matrix.t[0] = state->matrix.t[0] + state->aim.direction.vx * state->progress / 1024;
            matrix.t[1] = state->matrix.t[1] + state->aim.direction.vy * state->progress / 1024;
            matrix.t[2] = state->matrix.t[2] + state->aim.direction.vz * state->progress / 1024;
        } else {
            matrix.t[0] = state->aim.target.vx;
            matrix.t[1] = state->aim.target.vy;
            matrix.t[2] = state->aim.target.vz;
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
        polygon->r0 = sheet->inner.r;
        polygon->g0 = sheet->inner.g;
        polygon->b0 = sheet->inner.b;
        polygon->r1 = sheet->inner.r;
        polygon->g1 = sheet->inner.g;
        polygon->b1 = sheet->inner.b;
        polygon->r2 = sheet->inner.r;
        polygon->g2 = sheet->inner.g;
        polygon->b2 = sheet->inner.b;
        polygon->r3 = sheet->outer.r;
        polygon->g3 = sheet->outer.g;
        polygon->b3 = sheet->outer.b;
        for (j = 0; j < 4; j++) {
            depth = RotTransPers4(&sheet->v0[j], &sheet->v1[j], &sheet->v2[j], &sheet->v3[j],
                                 (long *)&polygon->x0, (long *)&polygon->x1,
                                 (long *)&polygon->x2, (long *)&polygon->x3,
                                 &interpolation, &flag);
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (i == 0) {
            if (state->phase == 0) {
                if (sheet->size < 4096) {
                    sheet->size = (state->clock.frame - state->config->start) * 4096 /
                        (state->config->grow_end - state->config->start);
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        state->phase = 1;
                    }
                }
            } else {
                sheet->size = state->scale;
            }
        } else if (state->phase == 0) {
            sheet->size = 0;
        } else if (state->progress >= 1024 && state->phase == 2) {
            if (sheet->size < 8192) {
                sheet->size += state->clock.step * 1024;
                if (sheet->size >= 8192) {
                    sheet->size = 8192;
                }
            }
        } else if (state->phase < 2) {
            sheet->size = 4096;
        } else if (state->phase == 3 && sheet->size > 0) {
            sheet->size -= state->clock.step * 64;
            if (sheet->size <= 0) {
                sheet->size = 0;
                state->phase = 4;
            }
        }
    }
}
