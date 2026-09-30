#include "../../types.h"
#include "variant402_bands.h"

void func_8013CB38(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    u8 *work;
    Family402Band *band;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    s32 direction;
    s32 angle;
    s32 bearing;
    s32 yaw;
    s32 outer_radius;
    s32 outer_depth;
    s32 size;
    s32 depth;
    s32 z;
    s16 fade;
    u8 r0, g0, b0, r1, g1, b1;

    work = context;
    band = (Family402Band *)(work + 0x438);
    ot = func_80058F10();
    direction = -1;
    bearing = ratan2(MODEL_VARIANT_WORD(work, 0x844), MODEL_VARIANT_WORD(work, 0x83C));
    yaw = bearing + 2048;
    ratan2(MODEL_VARIANT_WORD(work, 0x840), MODEL_VARIANT_WORD(work, 0x844));
    quad = (POLY_GT4 *)(work + 0x744);
    if (MODEL_VARIANT_HALF(work, 0x8C0) == 0) {
        direction = 1;
    }
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw = bearing + 3072;
    }
    outer_radius = (MODEL_VARIANT_WORD(work, 0x868) & 1) * 64 + 512;
    outer_depth = (MODEL_VARIANT_WORD(work, 0x868) & 1) * 64 + 192;
    for (i = 0; i < 2; i++, band++) {
        if (band->scale > 0) {
            size = band->scale;
            z = outer_depth * direction;
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                band->inner[j].vx = (u32)rcos(angle) >> 4;
                band->inner[j].vy = (u32)rsin(angle) >> 4;
                band->inner[j].vz = 0;
                setVector(&band->outer[j], rcos(angle) * outer_radius >> 12,
                          rsin(angle) * outer_radius >> 12, z);
            }
            rotation.vx = 0;
            rotation.vy = yaw;
            rotation.vz = 0;
            matrix.t[0] = MODEL_VARIANT_HALF(work, 0x834);
            matrix.t[1] = MODEL_VARIANT_HALF(work, 0x836);
            matrix.t[2] = MODEL_VARIANT_HALF(work, 0x838);
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
                r0 = work[0x894];
                g0 = work[0x895];
                b0 = work[0x896];
                r1 = work[0x898];
                g1 = work[0x899];
                b1 = work[0x89A];
            } else {
                fade = 4096 - band->scale;
                r0 = work[0x894] * fade / 2048;
                g0 = work[0x895] * fade / 2048;
                b0 = work[0x896] * fade / 2048;
                r1 = work[0x898] * fade / 2048;
                g1 = work[0x899] * fade / 2048;
                b1 = work[0x89A] * fade / 2048;
            }
            for (j = 0; j < 16; j++) {
                depth = RotTransPers4(&band->inner[j], &band->inner[j + 1],
                                     &band->outer[j], &band->outer[j + 1],
                                     (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                     (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                     &interpolation, &flag);
                setRGB0(quad, r0, g0, b0);
                setRGB1(quad, r0, g0, b0);
                setRGB2(quad, r1, g1, b1);
                setRGB3(quad, r1, g1, b1);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, depth & 0xFFFF);
                }
            }
        }
        if (band->scale < 4096) {
            band->scale += MODEL_VARIANT_WORD(work, 0x874) * 256;
            if (band->scale >= 4096) {
                band->scale = 4096;
                band->cycles++;
                if (i + 1 == 2 && MODEL_VARIANT_WORD(work, 0x8AC) == 2) {
                    MODEL_VARIANT_WORD(work, 0x8AC) = 3;
                }
            }
        }
    }
}
