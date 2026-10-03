#include "../../types.h"
#include "../model_variant/model_variant.h"

void func_8013C3D4(u8 *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG projection;
    PSXLONG flag;
    u8 *node;
    ModelVariantSheet *sheet;
    GsOT *ot;
    u8 *work;
    POLY_GT4 *poly;
    s32 i;
    s32 k;
    s32 bias;
    s32 depth;

    work = context;
    node = context;
    sheet = (ModelVariantSheet *)(work + 0xF00);
    ot = func_80058F10();
    ratan2(MODEL_VARIANT_WORD(work, 0x1FB4), MODEL_VARIANT_WORD(work, 0x1FAC));
    poly = (POLY_GT4 *)(work + 0x1E3C);
    for (i = 0; i < MODEL_VARIANT_WORD(work, 0x1FEC); i++, sheet++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x1FC4) & 1) {
            bias = sheet->size / 8;
        }
        setVector(&rot, 0, 0, 0);
        if ((i & 1) == 0) {
            matrix.t[0] = MODEL_VARIANT_WORD(work, 0x1F34) + MODEL_VARIANT_WORD(node, 0x2C8);
            matrix.t[1] = MODEL_VARIANT_WORD(node, 0x2CC);
            matrix.t[2] = MODEL_VARIANT_WORD(work, 0x1F3C) + MODEL_VARIANT_WORD(node, 0x2D0);
        } else {
            matrix.t[0] = MODEL_VARIANT_WORD(work, 0x1F34) + MODEL_VARIANT_WORD(node, 0x2D8);
            matrix.t[1] = MODEL_VARIANT_WORD(node, 0x2DC);
            matrix.t[2] = MODEL_VARIANT_WORD(work, 0x1F3C) + MODEL_VARIANT_WORD(node, 0x2E0);
        }
        setVector(&scale, sheet->size + bias, sheet->size + bias, sheet->size + bias);
        RotMatrix(&rot, &matrix);
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
            poly->r0 = sheet->inner[0];
            poly->g0 = sheet->inner[1];
            poly->b0 = sheet->inner[2];
            poly->r1 = sheet->inner[0];
            poly->g1 = sheet->inner[1];
            poly->b1 = sheet->inner[2];
            poly->r2 = sheet->inner[0];
            poly->g2 = sheet->inner[1];
            poly->b2 = sheet->inner[2];
            poly->r3 = sheet->outer[0];
            poly->g3 = sheet->outer[1];
            poly->b3 = sheet->outer[2];
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, depth);
            }
        }
        if ((i & 1) == 0) {
            if (MODEL_VARIANT_WORD(node, 0x2E8) == 0) {
                if (sheet->size < 4096) {
                    sheet->size += MODEL_VARIANT_WORD(work, 0x1FD0) * 256;
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        MODEL_VARIANT_WORD(node, 0x2E8) = 1;
                        if (MODEL_VARIANT_WORD(work, 0x2000) == 0) {
                            MODEL_VARIANT_WORD(work, 0x2000) = 1;
                        }
                    }
                }
            } else if (MODEL_VARIANT_WORD(node, 0x2E8) == 2) {
                sheet->size = MODEL_VARIANT_WORD(node, 0x2F0) * 4;
            }
        } else {
            if (MODEL_VARIANT_WORD(node, 0x2E8) == 1) {
                if (sheet->size < 8192) {
                    sheet->size += MODEL_VARIANT_WORD(work, 0x1FD0) * 1024;
                    if (sheet->size >= 8192) {
                        sheet->size = 8192;
                        MODEL_VARIANT_WORD(node, 0x2E8) = 2;
                    }
                }
            } else if (MODEL_VARIANT_WORD(node, 0x2E8) == 2) {
                sheet->size = MODEL_VARIANT_WORD(node, 0x2F0) * 8;
            }
            if (i + 1 == 8 && MODEL_VARIANT_WORD(node, 0x2E8) == 1
                && MODEL_VARIANT_WORD(work, 0x2000) == 1) {
                MODEL_VARIANT_WORD(work, 0x2000) = 2;
            }
            node += 960;
        }
    }
}
