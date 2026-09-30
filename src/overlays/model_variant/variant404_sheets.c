#include "../../types.h"

#include "model_variant.h"

/* Draws two sheets of four POLY_GT4 quads, the second placed along the
 * variant's path, and grows or shrinks each sheet's size through the
 * variant's phases. */
void func_8013DCA4(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    u8 *work;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    GsOT *ot;
    POLY_GT4 *poly;
    ModelVariantSheet *sheet;
    s32 i;
    s32 k;
    SVECTOR *v;
    s32 bias;
    s32 otz;

    work = ctx;
    sheet = (ModelVariantSheet *)(ctx + 0xFD8);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(ctx + 0x17AC);
    for (i = 0; i < 2; i++, sheet++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x191C) & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i == 0) {
            m.t[0] = MODEL_VARIANT_WORD(work, 0x189C);
            m.t[1] = MODEL_VARIANT_WORD(work, 0x18A0);
            m.t[2] = MODEL_VARIANT_WORD(work, 0x18A4);
        } else if (MODEL_VARIANT_WORD(work, 0x1954) < 0x400) {
            m.t[0] = MODEL_VARIANT_WORD(work, 0x189C) + MODEL_VARIANT_WORD(work, 0x18F0) * MODEL_VARIANT_WORD(work, 0x1954) / 1024;
            m.t[1] = MODEL_VARIANT_WORD(work, 0x18A0) + MODEL_VARIANT_WORD(work, 0x18F4) * MODEL_VARIANT_WORD(work, 0x1954) / 1024;
            m.t[2] = MODEL_VARIANT_WORD(work, 0x18A4) + MODEL_VARIANT_WORD(work, 0x18F8) * MODEL_VARIANT_WORD(work, 0x1954) / 1024;
        } else {
            m.t[0] = MODEL_VARIANT_HALF(work, 0x18E8);
            m.t[1] = MODEL_VARIANT_HALF(work, 0x18EA);
            m.t[2] = MODEL_VARIANT_HALF(work, 0x18EC);
        }
        scale.vx = sheet->size + bias;
        scale.vy = sheet->size + bias;
        scale.vz = sheet->size + bias;
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
        for (k = 0, v = sheet->v0; k < 4; k++, v++) {
            otz = RotTransPers4(v, &sheet->v1[k], &sheet->v2[k], &sheet->v3[k],
                                (long *)&poly->x0, (long *)&poly->x1,
                                (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
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
                GsSortPoly(poly, ot, otz);
            }
        }
        if (i == 0) {
            if (MODEL_VARIANT_WORD(work, 0x1960) == 0) {
                if (sheet->size < 0x1000) {
                    sheet->size = (u32)((MODEL_VARIANT_WORD(work, 0x1920) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1938), 0x20)) << 12) /
                                  (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1938), 0x24) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1938), 0x20));
                    if (sheet->size >= 0x1000) {
                        sheet->size = 0x1000;
                        MODEL_VARIANT_WORD(work, 0x1960) = 1;
                    }
                }
            } else {
                sheet->size = MODEL_VARIANT_HALF(work, 0x1950);
            }
        } else if (MODEL_VARIANT_WORD(work, 0x1960) == 0) {
            sheet->size = 0;
        } else if (MODEL_VARIANT_WORD(work, 0x1954) >= 0x400 && MODEL_VARIANT_WORD(work, 0x1960) == 2) {
            if (sheet->size < 0x2000) {
                sheet->size += MODEL_VARIANT_WORD(work, 0x1928) << 10;
                if (sheet->size >= 0x2000) {
                    sheet->size = 0x2000;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0x1960) < 2) {
            sheet->size = 0x1000;
        } else if (MODEL_VARIANT_WORD(work, 0x1960) == 3) {
            if (sheet->size > 0) {
                sheet->size -= MODEL_VARIANT_WORD(work, 0x1928) << 6;
                if (sheet->size <= 0) {
                    sheet->size = 0;
                    MODEL_VARIANT_WORD(work, 0x1960) = 4;
                }
            }
        }
    }
}
