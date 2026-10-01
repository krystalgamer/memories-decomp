#include "../../types.h"

#include "model_variant.h"

void func_8013D448(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    u8 *work;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *object;
    GsOT *ot;
    POLY_GT4 *poly;
    ModelVariantSheet *sheet;
    s32 i;
    s32 k;
    s32 bias;
    s32 level;
    s32 otz;

    work = ctx;
    object = ctx + 0x4E0;
    sheet = (ModelVariantSheet *)(ctx + 0xA80);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(ctx + 0x1668);
    for (i = 0; i < 6; i++, sheet++, object += 0x120) {
        level = 0;
        if (MODEL_VARIANT_WORD(object, 0xF4) > 0) {
            level = MODEL_VARIANT_WORD(object, 0xF4);
        }
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x189C) & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i == 5) {
            m.t[0] = MODEL_VARIANT_HALF(work, 0x1764);
            m.t[1] = MODEL_VARIANT_HALF(work, 0x1766);
            m.t[2] = MODEL_VARIANT_HALF(work, 0x1768);
            scale.vx = sheet->size + bias;
            scale.vy = sheet->size + bias;
            scale.vz = sheet->size + bias;
        } else {
            m.t[0] = MODEL_VARIANT_WORD(work + i * 0x20, 0x1794) + MODEL_VARIANT_WORD(work + i * 0x10, 0x1820) * level / 1024;
            m.t[1] = MODEL_VARIANT_WORD(work + i * 0x20, 0x1798) + MODEL_VARIANT_WORD(work + i * 0x10, 0x1824) * level / 1024;
            m.t[2] = MODEL_VARIANT_WORD(work + i * 0x20, 0x179C) + MODEL_VARIANT_WORD(work + i * 0x10, 0x1828) * level / 1024;
            scale.vx = 0x400;
            scale.vy = 0x400;
            scale.vz = 0x400;
        }
        RotMatrix(&rot, &m);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        ReadRotMatrix(&ls);
        RotMatrix(&rot, &ls);
        ScaleMatrix(&ls, &scale);
        SetRotMatrix(&ls);
        for (k = 0; k < 4; k++) {
            otz = RotTransPers4(&sheet->v0[k], &sheet->v1[k], &sheet->v2[k], &sheet->v3[k],
                                (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
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
            otz = otz * 8 / 10;
            if (otz >= 0 && flag >= 0) {
                if (i == 5) {
                    GsSortPoly(poly, ot, otz);
                } else if (MODEL_VARIANT_WORD(object, 0x104) < 0x400) {
                    GsSortPoly(poly, ot, otz);
                }
            }
        }
        if (i == 5) {
            if (MODEL_VARIANT_WORD(work, 0x18E0) == 2) {
                if (sheet->size < 0x1000) {
                    sheet->size += MODEL_VARIANT_WORD(work, 0x18A8) << 9;
                    if (sheet->size >= 0x1000) {
                        sheet->size = 0x1000;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x18E0) == 3) {
                if (sheet->size > 0) {
                    sheet->size -= MODEL_VARIANT_WORD(work, 0x18A8) << 5;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        MODEL_VARIANT_WORD(work, 0x18E0) = 5;
                    }
                }
            }
        }
    }
}
