#include "../../types.h"
#include "variant402_bands.h"

void FrenchModelVariant402_DrawBands(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    u8 *work;
    ModelVariantCurtain *band;
    GsOT *ot;
    Family402BandPacket *quad;
    s32 i, j;
#ifndef MODEL_VARIANT412_BANDS
    s32 direction;
#endif
    s32 angle;
    s32 bearing;
#ifdef MODEL_VARIANT412_BANDS
    s32 pulse;
#endif
    s32 yaw;
#ifndef MODEL_VARIANT412_BANDS
    s32 outer_radius;
    s32 outer_depth;
#endif
    s32 size;
    s32 depth;
#ifndef MODEL_VARIANT412_BANDS
    s32 z;
#endif
    s16 fade;
    u8 r0, g0, b0, r1, g1, b1;

    work = context;
    band = (ModelVariantCurtain *)(work + BAND_WORK_0438);
    ot = Graphics_GetDefaultOrderingTable();
#ifndef MODEL_VARIANT412_BANDS
    direction = -1;
#endif
    bearing = ratan2(MODEL_VARIANT_WORD(work, BAND_WORK_0844), MODEL_VARIANT_WORD(work, BAND_WORK_083C));
    yaw = bearing + 2048;
    ratan2(MODEL_VARIANT_WORD(work, BAND_WORK_0840), MODEL_VARIANT_WORD(work, BAND_WORK_0844));
    quad = (Family402BandPacket *)(work + BAND_WORK_0744);
#ifndef MODEL_VARIANT412_BANDS
    if (MODEL_VARIANT_HALF(work, 0x8C0) == 0) {
        direction = 1;
    }
#endif
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw = bearing + 3072;
    }
#ifdef MODEL_VARIANT412_BANDS
    pulse = (MODEL_VARIANT_WORD(work, BAND_WORK_0868) & 1) * 64;
#else
    outer_radius = (MODEL_VARIANT_WORD(work, BAND_WORK_0868) & 1) * 64 + 512;
    outer_depth = (MODEL_VARIANT_WORD(work, BAND_WORK_0868) & 1) * 64 + 192;
#endif
    for (i = 0; i < 2; i++, band++) {
        if (band->scale > 0) {
            size = band->scale;
#ifndef MODEL_VARIANT412_BANDS
            z = outer_depth * direction;
#endif
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                band->a[j].vx = (u32)rcos(angle) >> 4;
                band->a[j].vy = (u32)rsin(angle) >> 4;
                band->a[j].vz = 0;
#ifdef MODEL_VARIANT412_BANDS
                setVector(&band->b[j], rcos(angle) * (pulse + 512) >> 12,
                          rsin(angle) * (pulse + 512) >> 12, pulse + 192);
#else
                setVector(&band->b[j], rcos(angle) * outer_radius >> 12,
                          rsin(angle) * outer_radius >> 12, z);
#endif
            }
            rotation.vx = 0;
            rotation.vy = yaw;
            rotation.vz = 0;
            matrix.t[0] = MODEL_VARIANT_HALF(work, BAND_WORK_0834);
            matrix.t[1] = MODEL_VARIANT_HALF(work, BAND_WORK_0836);
            matrix.t[2] = MODEL_VARIANT_HALF(work, BAND_WORK_0838);
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            if (band->scale < 2048) {
                r0 = work[BAND_WORK_0894];
                g0 = work[BAND_WORK_0895];
                b0 = work[BAND_WORK_0896];
                r1 = work[BAND_WORK_0898];
                g1 = work[BAND_WORK_0899];
                b1 = work[BAND_WORK_089A];
            } else {
                fade = 4096 - band->scale;
                r0 = work[BAND_WORK_0894] * fade / 2048;
                g0 = work[BAND_WORK_0895] * fade / 2048;
                b0 = work[BAND_WORK_0896] * fade / 2048;
                r1 = work[BAND_WORK_0898] * fade / 2048;
                g1 = work[BAND_WORK_0899] * fade / 2048;
                b1 = work[BAND_WORK_089A] * fade / 2048;
            }
            for (j = 0; j < 16; j++) {
                depth = RotTransPers4(&band->a[j], &band->a[j + 1],
                                     &band->b[j], &band->b[j + 1],
                                     (long *)&quad->x0, (long *)&quad->x1,
                                     (long *)&quad->x2, (long *)&quad->x3,
                                     &interpolation, &flag);
                setRGB0(quad, r0, g0, b0);
#ifndef MODEL_VARIANT412_BANDS
                setRGB1(quad, r0, g0, b0);
                setRGB2(quad, r1, g1, b1);
                setRGB3(quad, r1, g1, b1);
#endif
#ifdef MODEL_VARIANT412_BANDS
                if (depth > 0) {
#else
                if (depth >= 0 && flag >= 0) {
#endif
                    GsSortPoly(quad, ot, depth & 0xFFFF);
                }
            }
        }
        if (band->scale < 4096) {
            band->scale += MODEL_VARIANT_WORD(work, BAND_WORK_0874) * 256;
            if (band->scale >= 4096) {
                band->scale = 4096;
                band->count++;
                if (i + 1 == 2 && MODEL_VARIANT_WORD(work, BAND_WORK_08AC) == 2) {
                    MODEL_VARIANT_WORD(work, BAND_WORK_08AC) = 3;
                }
            }
        }
    }
}
