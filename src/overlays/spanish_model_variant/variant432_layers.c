#include "../../types.h"
#include "variant432_layers.h"

void func_8013C69C(u8 *context)
{
    ModelVariant432LayersState *work = (ModelVariant432LayersState *)context;
    ModelVariant432Source *source = (ModelVariant432Source *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    ModelVariant432Band *band;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 j, i, k;
    u8 red0, green0, blue0, red1, green1, blue1;
    s32 size;
    s32 depth;

    ot = func_80058F10();
    for (i = 0; i < 5; i++) {
        band = work->bands;
        for (j = 0, source = work->sources; j < 3; j++, band++, source++) {
            if (band->phase > 0) {
                size = band->phase;
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
                matrix.t[0] = work->positions[i].position.vx;
                matrix.t[1] = work->positions[i].position.vy;
                matrix.t[2] = work->positions[i].position.vz;
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
                for (k = 0, quad = work->quads; k < 16; k++, quad++) {
                    depth = RotTransPers4(
                        &band->points[0][k], &band->points[0][k + 1],
                        &band->points[1][k], &band->points[1][k + 1],
                        (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                        (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                        (PSXLONG *)&interpolation, (PSXLONG *)&flag);
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
                    if (band->phase < 4096 && depth >= 0 && flag >= 0) {
                        GsSortPoly(quad, ot, depth);
                    }
                }
            }
            if (i + 1 == 5) {
                s32 phase = 0;
                if (source->value >= 0) {
                    phase = source->value;
                }
                band->phase = phase * 4;
            }
        }
    }
}
