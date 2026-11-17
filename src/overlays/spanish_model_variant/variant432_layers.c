#include "../../types.h"
#include "../model_variant/variant415_entry.h"

void SpanishModelVariant432_DrawBandLayers(Variant415EntryState *context)
{
    Variant415EntryState *work = context;
    Variant415EntryRibbon *source = context->ribbons;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    Variant415EntryStrand *band;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 j, i, k;
    u8 red0, green0, blue0, red1, green1, blue1;
    s32 size;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    for (i = 0; i < 5; i++) {
        band = work->strands_a;
        for (j = 0, source = work->ribbons; j < 3; j++, band++, source++) {
            if (band->scale > 0) {
                size = band->scale;
                if (size > 2048) {
                    red0 = band->color[0].r * (4096 - size) / 2048;
                    green0 = band->color[0].g * (4096 - size) / 2048;
                    blue0 = band->color[0].b * (4096 - size) / 2048;
                    red1 = band->color[1].r * (4096 - size) / 2048;
                    green1 = band->color[1].g * (4096 - size) / 2048;
                    blue1 = band->color[1].b * (4096 - size) / 2048;
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
                matrix.t[0] = work->matrices[i].t[0];
                matrix.t[1] = work->matrices[i].t[1];
                matrix.t[2] = work->matrices[i].t[2];
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
                for (k = 0, quad = work->bars; k < 16; k++, quad++) {
                    depth = RotTransPers4(
                        &band->inner[k], &band->inner[k + 1],
                        &band->outer[k], &band->outer[k + 1],
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
                    if (band->scale < 4096 && depth >= 0 && flag >= 0) {
                        GsSortPoly(quad, ot, depth);
                    }
                }
            }
            if (i + 1 == 5) {
                s32 phase = 0;
                if (source->amount[0] >= 0) {
                    phase = source->amount[0];
                }
                band->scale = phase * 4;
            }
        }
    }
}
