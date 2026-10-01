#include "../../types.h"
#include "variant402_strip.h"

void func_8013BA5C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG flags[1][2];
    PSXLONG interpolation;
    u8 *work;
    Family402Strip *strip;
    POLY_GT4 *quad;
    GsOT *ot;
    s16 i, j;
    s32 angle;
    s32 size;
    s32 width;

    work = context;
    ot = func_80058F10();
    angle = ratan2(MODEL_VARIANT_HALF(work, 0x84E), MODEL_VARIANT_HALF(work, 0x84C));
    quad = (POLY_GT4 *)(work + 0x6A8);
    angle += 2048;
    if (MODEL_VARIANT_WORD(work, 0x8AC) > 0) {
        strip = (Family402Strip *)work;
        width = MODEL_VARIANT_HALF(work, 0x8A4);
        if (width < 0) {
            width += 31;
        }
        i = 0;
        size = width >> 5;
        for (; i < 1; i++, strip++) {
            for (j = 0; j < 2; j++) {
                j[strip->points[0]].vx = rcos(1024) * size >> 12;
                j[strip->points[0]].vy = rsin(1024) * size >> 12;
                j[strip->points[0]].vz = 0;
                setVector(&strip->points[1][j], 0, 0, 0);
                setVector(&strip->points[2][j], rcos(3072) * size >> 12,
                          rsin(3072) * size >> 12, 0);
                setVector(&rotation, 0, 0, angle);
                if (j == 0) {
                    matrix.t[0] = MODEL_VARIANT_WORD(work, 0x828);
                    matrix.t[1] = MODEL_VARIANT_WORD(work, 0x82C);
                    matrix.t[2] = MODEL_VARIANT_WORD(work, 0x830);
                } else {
                    matrix.t[0] = MODEL_VARIANT_WORD(work, 0x828) +
                        MODEL_VARIANT_WORD(work, 0x83C) * MODEL_VARIANT_HALF(work, 0x8A6) / 1024;
                    matrix.t[1] = MODEL_VARIANT_WORD(work, 0x82C) +
                        MODEL_VARIANT_WORD(work, 0x840) * MODEL_VARIANT_HALF(work, 0x8A6) / 1024;
                    matrix.t[2] = MODEL_VARIANT_WORD(work, 0x830) +
                        MODEL_VARIANT_WORD(work, 0x844) * MODEL_VARIANT_HALF(work, 0x8A6) / 1024;
                }
                scale.vz = scale.vy = scale.vx = 4096;
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
                strip->depth[j] = RotTransPers3(&strip->points[0][j], &strip->points[1][j],
                                                &strip->points[2][j], &strip->projected[0][j],
                                                &strip->projected[1][j],
                                                &strip->projected[2][j],
                                                &interpolation, &flags[i][j]);
            }
        }
        strip = (Family402Strip *)work;
        for (i = 0; i < 1; i++, strip++) {
            for (j = 0; j < 1; j++) {
                setXY4(quad, (u16)strip->projected[0][j], (strip->projected[0][j] >> 16),
                       (u16)strip->projected[0][j + 1], (strip->projected[0][j + 1] >> 16),
                       (u16)strip->projected[1][j], (strip->projected[1][j] >> 16),
                       (u16)strip->projected[1][j + 1], (strip->projected[1][j + 1] >> 16));
                setRGB0(quad, 0, 64, 192);
                setRGB1(quad, 0, 64, 192);
                setRGB2(quad, 128, 128, 128);
                setRGB3(quad, 128, 128, 128);
                if (strip->depth[j] >= 0 && flags[i][j] >= 0) {
                    GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF);
                }
                setXY4(quad, (u16)strip->projected[2][j], (strip->projected[2][j] >> 16),
                       (u16)strip->projected[2][j + 1], (strip->projected[2][j + 1] >> 16),
                       (u16)strip->projected[1][j], (strip->projected[1][j] >> 16),
                       (u16)strip->projected[1][j + 1], (strip->projected[1][j + 1] >> 16));
                setRGB0(quad, 0, 64, 192);
                setRGB1(quad, 0, 64, 192);
                setRGB2(quad, 128, 128, 128);
                setRGB3(quad, 128, 128, 128);
                if (strip->depth[j] >= 0 && flags[i][j] >= 0) {
                    GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF);
                }
            }
        }
        if (MODEL_VARIANT_WORD(work, 0x890) + 1 ==
            ((Family402RingView *)work)->config->part_count) {
            if (MODEL_VARIANT_WORD(work, 0x8AC) == 1) {
                if (MODEL_VARIANT_HALF(work, 0x8A6) <= 1024) {
                    MODEL_VARIANT_HALF(work, 0x8A6) += MODEL_VARIANT_WORD(work, 0x874) * 64;
                    if (MODEL_VARIANT_HALF(work, 0x8A6) >= 1024) {
                        MODEL_VARIANT_HALF(work, 0x8A6) = 1024;
                        MODEL_VARIANT_WORD(work, 0x8AC) = 2;
                    }
                }
            }
            if (MODEL_VARIANT_WORD(work, 0x8AC) == 5 && MODEL_VARIANT_HALF(work, 0x8A4) > 0) {
                MODEL_VARIANT_HALF(work, 0x8A4) -= MODEL_VARIANT_WORD(work, 0x874) * 8;
                if (MODEL_VARIANT_HALF(work, 0x8A4) <= 0) {
                    MODEL_VARIANT_HALF(work, 0x8A4) = 0;
                }
            }
        }
    }
}
