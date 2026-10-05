#include "../../types.h"
#include "variant464_sheets.h"

void func_8013CE94(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Sheets464State *state = (Sheets464State *)context;
    /* The final sheet has no primary record; keep traversal in the raw context. */
    u8 *primary_cursor = context + 0x4E0;
    GsOT *ot;
    ModelVariantSheetSet *sheet = state->sheets;
    POLY_GT4 *polygon;
    SVECTOR *vertex;
    s32 i, j;
    s32 count;
    s32 extra;
    s32 depth;

    ot = func_80058F10();
    i = 0;
    count = state->timing->count;
    polygon = &state->polygon;
    for (; i <= count; i++, sheet++, primary_cursor += sizeof(Sheets464Primary)) {
        extra = 0;
        if (state->frame & 1) {
            extra = sheet->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == count) {
            matrix.t[0] = state->origin[0];
            matrix.t[1] = state->origin[1];
            matrix.t[2] = state->origin[2];
        } else {
            matrix.t[0] = state->anchor.vx;
            matrix.t[1] = state->anchor.vy;
            matrix.t[2] = state->anchor.vz;
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
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(polygon, ot, depth);
            }
        }
        if (i == count) {
            if (state->phase == 0) {
                if (sheet->size < 4096) {
                    sheet->size = ((state->time - state->timing->grow_begin) << 12) /
                                  (state->timing->grow_end - state->timing->grow_begin);
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        state->phase = 1;
                    }
                }
            } else if (state->time > state->timing->fade_begin && sheet->size > 0) {
                sheet->size = 4096 - ((state->time - state->timing->fade_begin) << 12) /
                                    (state->timing->fade_end - state->timing->fade_begin);
                if (sheet->size <= 0) {
                    sheet->size = 0;
                    state->phase = 3;
                }
            }
        } else if (((Sheets464Primary *)primary_cursor)->active == 1) {
            if (sheet->size < 16384 && sheet->shown == 0) {
                sheet->size += state->step * 4096;
                if (sheet->size >= 16384) {
                    sheet->size = 16384;
                    sheet->shown = 1;
                }
            } else if (sheet->size > 0) {
                sheet->size -= state->step * 512;
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        }
    }
}
