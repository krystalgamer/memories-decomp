#include "../../types.h"
#include "variant432_bands.h"

void func_8013CAC4(u8 *context)
{
    ModelVariant432BandsState *work = (ModelVariant432BandsState *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    ModelVariant432Band *band = work->bands;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    u8 red0, green0, blue0, red1, green1, blue1;
    s32 size;
    s32 depth;

    ot = func_80058F10();
    for (i = 0; i < 3; i++, band++) {
        if (band->phase > 0) {
            size = band->phase;
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
            matrix.t[0] = work->position.vx;
            matrix.t[1] = work->position.vy;
            matrix.t[2] = work->position.vz;
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
            for (j = 0, quad = work->quads; j < 16; j++, quad++) {
                depth = RotTransPers4(
                    &band->points[0][j], &band->points[0][j + 1],
                    &band->points[1][j], &band->points[1][j + 1],
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
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, depth);
                }
            }
        }
        if (band->phase < 4096) {
            band->phase += work->step * 160;
            if (band->phase >= 4096) {
                if (work->state < 4) {
                    band->phase -= 4096;
                } else {
                    band->phase = 4096;
                    if (i + 1 == 3 && work->state == 4) {
                        work->state = 6;
                    }
                }
            }
        }
    }
}
