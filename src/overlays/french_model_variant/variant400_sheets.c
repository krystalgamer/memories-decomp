#include "../../types.h"
#include "../model_variant/model_variant.h"

void func_8013C838(u8 *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG projection;
    PSXLONG flag;
    u8 *node;
    ModelVariantSheet *quad;
    GsOT *ot;
    u8 *work;
    POLY_GT4 *poly;
    s32 i;
    s32 k;
    s32 bias;
    s32 depth;

    work = context;
    node = context;
    quad = (ModelVariantSheet *)(work + 0x1130);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x1F4C);
    for (i = 0; i < 7; i++, quad++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x2044) & 1) {
            bias = quad->size / 8;
        }
        setVector(&rot, 0, 0, 0);
        if (i == 0) {
            matrix.t[0] = MODEL_VARIANT_WORD(work, 0x1FF4);
            matrix.t[1] = MODEL_VARIANT_WORD(work, 0x1FF8);
            matrix.t[2] = MODEL_VARIANT_WORD(work, 0x1FFC);
        } else if (i < 6) {
            matrix.t[0] = MODEL_VARIANT_WORD(node, 0x2C8);
            matrix.t[1] = MODEL_VARIANT_WORD(node, 0x2CC);
            matrix.t[2] = MODEL_VARIANT_WORD(node, 0x2D0);
            node += 880;
        } else {
            matrix.t[0] = MODEL_VARIANT_HALF(work, 0x2000);
            matrix.t[1] = MODEL_VARIANT_HALF(work, 0x2002);
            matrix.t[2] = MODEL_VARIANT_HALF(work, 0x2004);
        }
        setVector(&scale, quad->size + bias, quad->size + bias, quad->size + bias);
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
                &quad->v0[k], &quad->v1[k], &quad->v2[k], &quad->v3[k],
                (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3,
                &projection, &flag);
            poly->r0 = quad->inner[0];
            poly->g0 = quad->inner[1];
            poly->b0 = quad->inner[2];
            poly->r1 = quad->inner[0];
            poly->g1 = quad->inner[1];
            poly->b1 = quad->inner[2];
            poly->r2 = quad->inner[0];
            poly->g2 = quad->inner[1];
            poly->b2 = quad->inner[2];
            poly->r3 = quad->outer[0];
            poly->g3 = quad->outer[1];
            poly->b3 = quad->outer[2];
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, depth);
            }
        }
        if (i == 0) {
            if (MODEL_VARIANT_WORD(work, 0x2090) == 0) {
                if (quad->size < 4096) {
                    quad->size = (((u32)MODEL_VARIANT_WORD(work, 0x2048)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x205C), 0x0C)) << 12)
                        / ((u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x205C), 0x10)
                        - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x205C), 0x0C));
                    if (quad->size >= 4096) {
                        quad->size = 4096;
                        MODEL_VARIANT_WORD(work, 0x2090) = 1;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x2090) == 1 || MODEL_VARIANT_WORD(work, 0x2090) == 2) {
                if (quad->size >= 0) {
                    quad->size -= MODEL_VARIANT_WORD(work, 0x2050) * 64;
                    if (quad->size <= 0) {
                        quad->size = 0;
                        if (MODEL_VARIANT_WORD(work, 0x206C) == 16) {
                            MODEL_VARIANT_WORD(work, 0x2090) = 3;
                        }
                    }
                }
            }
        } else if (i < 6) {
            if (MODEL_VARIANT_WORD(work, 0x2090) == 2 || MODEL_VARIANT_WORD(work, 0x2090) == 3) {
                if (quad->size <= 4096) {
                    quad->size += MODEL_VARIANT_WORD(work, 0x2050) * 64;
                    if (quad->size >= 4096) {
                        quad->size = 4096;
                        if (MODEL_VARIANT_WORD(work, 0x2090) == 3) {
                            MODEL_VARIANT_WORD(work, 0x2090) = 4;
                        }
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x2090) >= 5) {
                if ((u32)MODEL_VARIANT_WORD(work, 0x2048)
                    > (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x205C), 0x14)
                    && quad->size > 0) {
                    quad->size -= MODEL_VARIANT_WORD(work, 0x2050) * 128;
                    if (quad->size <= 0) {
                        quad->size = 0;
                        if (MODEL_VARIANT_WORD(work, 0x2090) == 5) {
                            MODEL_VARIANT_WORD(work, 0x2090) = 6;
                        }
                    }
                }
            }
        } else {
            if (MODEL_VARIANT_WORD(work, 0x2090) == 5 || MODEL_VARIANT_WORD(work, 0x2090) == 6) {
                if (quad->size < 8192) {
                    quad->size += MODEL_VARIANT_WORD(work, 0x2050) * 512;
                    if (quad->size >= 8192) {
                        quad->size = 8192;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x2090) == 7 && quad->size > 0) {
                quad->size -= MODEL_VARIANT_WORD(work, 0x2050) * 64;
                if (quad->size <= 0) {
                    quad->size = 0;
                    MODEL_VARIANT_WORD(work, 0x2090) = 8;
                }
            }
        }
    }
}
