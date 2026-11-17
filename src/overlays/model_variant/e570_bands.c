#include "../../types.h"
#include "../model_variant/e570_entry.h"

void SpanishModelVariant432_DrawBands(Variant415EntryState *context)
{
    Variant415EntryState *work = context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    Variant415EntryStrand *band = work->strands_b;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    u8 red0, green0, blue0, red1, green1, blue1;
    s32 size;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    for (i = 0; i < 3; i++, band++) {
        if (band->scale > 0) {
            size = band->scale;
            if (size > 3072) {
                red0 = band->color[0].r * (4096 - size) / 1024;
                green0 = band->color[0].g * (4096 - size) / 1024;
                blue0 = band->color[0].b * (4096 - size) / 1024;
                red1 = band->color[1].r * (4096 - size) / 1024;
                green1 = band->color[1].g * (4096 - size) / 1024;
                blue1 = band->color[1].b * (4096 - size) / 1024;
            } else {
                red0 = band->color[0].r;
                green0 = band->color[0].g;
                blue0 = band->color[0].b;
                red1 = band->color[1].r;
                green1 = band->color[1].g;
                blue1 = band->color[1].b;
            }
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = work->target.vx;
            matrix.t[1] = work->target.vy;
            matrix.t[2] = work->target.vz;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
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
            for (j = 0, quad = work->bars; j < 16; j++, quad++) {
                depth = RotTransPers4(
                    &band->inner[j], &band->inner[j + 1],
                    &band->outer[j], &band->outer[j + 1],
                    (long *)&quad->x0, (long *)&quad->x1,
                    (long *)&quad->x2, (long *)&quad->x3,
                    (long *)&interpolation, (long *)&flag);
                quad->r0 = red0;
                quad->g0 = green0;
                quad->b0 = blue0;
                quad->r1 = red0;
                quad->g1 = green0;
                quad->b1 = blue0;
                quad->r2 = red1;
                quad->g2 = green1;
                quad->b2 = blue1;
                quad->r3 = red1;
                quad->g3 = green1;
                quad->b3 = blue1;
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, depth);
                }
            }
        }
        if (band->scale < 4096) {
            band->scale += work->clock.step * 160;
            if (band->scale >= 4096) {
                if (work->tail.phase < 4) {
                    band->scale -= 4096;
                } else {
                    band->scale = 4096;
                    if (i + 1 == 3 && work->tail.phase == 4) {
                        work->tail.phase = 6;
                    }
                }
            }
        }
    }
}
