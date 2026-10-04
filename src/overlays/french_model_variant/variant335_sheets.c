#include "../../types.h"
#include "../model_variant/model_variant.h"

void func_8013C534(u8 *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG projection;
    PSXLONG flag;
    ModelVariantSheet *sheet;
    GsOT *ot;
    u8 *work;
    POLY_GT4 *poly;
    s32 i;
    s32 k;
    s32 bias;
    s32 depth;
    s32 turn;
    s32 angle;
    s32 radius;
    s32 rotation_bias;

    work = context;
    sheet = (ModelVariantSheet *)(work + 0xABC);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x2CB0);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x2E3C), MODEL_VARIANT_WORD(work, 0x2E34));
    rotation_bias = MODEL_VARIANT_WORD(work, 0x2E74) + 3072;
    turn += rotation_bias;
    for (i = 0; i < 4; i++, sheet++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x2E4C) & 1) {
            bias = sheet->size / 8;
        }
        setVector(&rot, 0, 0, 0);
        if (i == 3) {
            matrix.t[0] = MODEL_VARIANT_HALF(work, 0x2E18);
            matrix.t[1] = MODEL_VARIANT_HALF(work, 0x2E1A);
            matrix.t[2] = MODEL_VARIANT_HALF(work, 0x2E1C);
        } else {
            angle = turn + (i << 12) / 3;
            radius = MODEL_VARIANT_WORD(work, 0x2E78) / 2;
            matrix.t[0] = MODEL_VARIANT_WORD(work, 0x2DE8) + (rcos(angle) * radius >> 12);
            matrix.t[1] = MODEL_VARIANT_WORD(work, 0x2DEC);
            matrix.t[2] = MODEL_VARIANT_WORD(work, 0x2DF0) + (rsin(angle) * radius >> 12);
        }
        setVector(&scale, sheet->size + bias, sheet->size + bias, sheet->size + bias);
        RotMatrix(&rot, &matrix);
        ScaleMatrix(&matrix, &scale);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        ReadRotMatrix(&light);
        RotMatrix(&rot, &light);
        ScaleMatrix(&light, &scale);
        SetRotMatrix(&light);
        for (k = 0; k < 4; k++) {
            depth = RotTransPers4(
                &sheet->v0[k], &sheet->v1[k], &sheet->v2[k], &sheet->v3[k],
                (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3,
                &projection, &flag);
            setRGB0(poly, sheet->inner[0], sheet->inner[1], sheet->inner[2]);
            setRGB1(poly, sheet->inner[0], sheet->inner[1], sheet->inner[2]);
            setRGB2(poly, sheet->inner[0], sheet->inner[1], sheet->inner[2]);
            setRGB3(poly, sheet->outer[0], sheet->outer[1], sheet->outer[2]);
            if ((MODEL_VARIANT_WORD(work, 0x2EA0) != 0 || i == 0)
                && depth >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, depth);
            }
        }
        if (i == 3) {
            if (MODEL_VARIANT_WORD(work, 0x2EA0) == 5) {
                if (sheet->size < 16384) {
                    sheet->size += MODEL_VARIANT_WORD(work, 0x2E58) * 2048;
                    if (sheet->size >= 16384) {
                        sheet->size = 16384;
                        MODEL_VARIANT_WORD(work, 0x2EA0) = 6;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x2EA0) >= 6) {
                if ((u32)MODEL_VARIANT_WORD(work, 0x2E50)
                    > (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x24)
                    && sheet->size > 0) {
                    sheet->size = 16384 - ((((u32)MODEL_VARIANT_WORD(work, 0x2E50)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x24)) << 14)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x28)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x24)));
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                    }
                }
            }
        } else {
            if (MODEL_VARIANT_WORD(work, 0x2EA0) == 0) {
                if (sheet->size < 4096) {
                    sheet->size = (((u32)MODEL_VARIANT_WORD(work, 0x2E50)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x0C)) << 12)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x10)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x0C));
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        MODEL_VARIANT_WORD(work, 0x2EA0) = 1;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x2EA0) == 1) {
                if (MODEL_VARIANT_WORD(work, 0x2E78) < 1024) {
                    MODEL_VARIANT_WORD(work, 0x2E78) = (((u32)MODEL_VARIANT_WORD(work, 0x2E50)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x10)) << 10)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x14)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x10));
                    if (MODEL_VARIANT_WORD(work, 0x2E78) >= 1024) {
                        MODEL_VARIANT_WORD(work, 0x2E78) = 1024;
                        MODEL_VARIANT_WORD(work, 0x2EA0) = 2;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x2EA0) == 2) {
                if (MODEL_VARIANT_WORD(work, 0x2E70) < 256) {
                    MODEL_VARIANT_WORD(work, 0x2E70) = (((u32)MODEL_VARIANT_WORD(work, 0x2E50)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x14)) << 8)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x18)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x14));
                    if (MODEL_VARIANT_WORD(work, 0x2E70) >= 256) {
                        MODEL_VARIANT_WORD(work, 0x2E70) = 256;
                        MODEL_VARIANT_WORD(work, 0x2EA0) = 3;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x2EA0) == 3) {
                if (MODEL_VARIANT_WORD(work, 0x2E6C) < 1024) {
                    MODEL_VARIANT_WORD(work, 0x2E6C) = (((u32)MODEL_VARIANT_WORD(work, 0x2E50)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x18)) << 10)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x1C)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x18));
                    if (MODEL_VARIANT_WORD(work, 0x2E6C) >= 1024) {
                        MODEL_VARIANT_WORD(work, 0x2E6C) = 1024;
                        MODEL_VARIANT_WORD(work, 0x2EA0) = 4;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x2EA0) >= 6) {
                if ((u32)MODEL_VARIANT_WORD(work, 0x2E50)
                    > (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x24)
                    && sheet->size > 0) {
                    sheet->size = 4096 - ((((u32)MODEL_VARIANT_WORD(work, 0x2E50)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x24)) << 12)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x28)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x24)));
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        MODEL_VARIANT_WORD(work, 0x2EA0) = 7;
                    }
                }
            }
        }
    }
}
