#include "../../types.h"
#include "variant402_strip.h"

void func_8013BA5C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
#ifdef MODEL_VARIANT412_STRIP
    PSXLONG interpolation;
    PSXLONG flags;
#else
    PSXLONG flags[1][2];
    PSXLONG interpolation;
#endif
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
#ifdef MODEL_VARIANT412_STRIP
    strip = (Family402Strip *)(work + STRIP_RECORD_BASE);
#endif
    angle = ratan2(MODEL_VARIANT_HALF(work, STRIP_WORK_084E), MODEL_VARIANT_HALF(work, STRIP_WORK_084C));
    quad = (POLY_GT4 *)(work + STRIP_WORK_06A8);
    angle += 2048;
    if (MODEL_VARIANT_WORD(work, STRIP_WORK_08AC) > 0) {
#ifndef MODEL_VARIANT412_STRIP
        strip = (Family402Strip *)(work + STRIP_RECORD_BASE);
#endif
        width = MODEL_VARIANT_HALF(work, STRIP_WORK_08A4);
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
                    matrix.t[0] = MODEL_VARIANT_WORD(work, STRIP_WORK_0828);
                    matrix.t[1] = MODEL_VARIANT_WORD(work, STRIP_WORK_082C);
                    matrix.t[2] = MODEL_VARIANT_WORD(work, STRIP_WORK_0830);
                } else {
                    matrix.t[0] = MODEL_VARIANT_WORD(work, STRIP_WORK_0828) +
                        MODEL_VARIANT_WORD(work, STRIP_WORK_083C) * MODEL_VARIANT_HALF(work, STRIP_WORK_08A6) / 1024;
                    matrix.t[1] = MODEL_VARIANT_WORD(work, STRIP_WORK_082C) +
                        MODEL_VARIANT_WORD(work, STRIP_WORK_0840) * MODEL_VARIANT_HALF(work, STRIP_WORK_08A6) / 1024;
                    matrix.t[2] = MODEL_VARIANT_WORD(work, STRIP_WORK_0830) +
                        MODEL_VARIANT_WORD(work, STRIP_WORK_0844) * MODEL_VARIANT_HALF(work, STRIP_WORK_08A6) / 1024;
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
                                                &interpolation,
#ifdef MODEL_VARIANT412_STRIP
                                                &flags);
#else
                                                &flags[i][j]);
#endif
            }
        }
        strip = (Family402Strip *)(work + STRIP_RECORD_BASE);
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
#ifdef MODEL_VARIANT412_STRIP
                if (strip->depth[j] > 0) {
                    if (strip->depth[j] < 2048) {
                        GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF);
                    }
                }
#else
                if (strip->depth[j] >= 0 && flags[i][j] >= 0) {
                    GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF);
                }
#endif
                setXY4(quad, (u16)strip->projected[2][j], (strip->projected[2][j] >> 16),
                       (u16)strip->projected[2][j + 1], (strip->projected[2][j + 1] >> 16),
                       (u16)strip->projected[1][j], (strip->projected[1][j] >> 16),
                       (u16)strip->projected[1][j + 1], (strip->projected[1][j + 1] >> 16));
                setRGB0(quad, 0, 64, 192);
                setRGB1(quad, 0, 64, 192);
                setRGB2(quad, 128, 128, 128);
                setRGB3(quad, 128, 128, 128);
#ifdef MODEL_VARIANT412_STRIP
                if (strip->depth[j] > 0) {
                    if (strip->depth[j] < 2048) {
                        GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF);
                    }
                }
#else
                if (strip->depth[j] >= 0 && flags[i][j] >= 0) {
                    GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF);
                }
#endif
            }
        }
        if (MODEL_VARIANT_WORD(work, STRIP_WORK_0890) + 1 ==
            ((Family402RingView *)work)->config->part_count) {
            if (MODEL_VARIANT_WORD(work, STRIP_WORK_08AC) == 1) {
                if (MODEL_VARIANT_HALF(work, STRIP_WORK_08A6) <= 1024) {
                    MODEL_VARIANT_HALF(work, STRIP_WORK_08A6) += MODEL_VARIANT_WORD(work, STRIP_WORK_0874) * 64;
                    if (MODEL_VARIANT_HALF(work, STRIP_WORK_08A6) >= 1024) {
                        MODEL_VARIANT_HALF(work, STRIP_WORK_08A6) = 1024;
                        MODEL_VARIANT_WORD(work, STRIP_WORK_08AC) = 2;
                    }
                }
            }
            if (MODEL_VARIANT_WORD(work, STRIP_WORK_08AC) == 5 && MODEL_VARIANT_HALF(work, STRIP_WORK_08A4) > 0) {
                MODEL_VARIANT_HALF(work, STRIP_WORK_08A4) -= MODEL_VARIANT_WORD(work, STRIP_WORK_0874) * 8;
                if (MODEL_VARIANT_HALF(work, STRIP_WORK_08A4) <= 0) {
                    MODEL_VARIANT_HALF(work, STRIP_WORK_08A4) = 0;
                }
            }
        }
    }
}
