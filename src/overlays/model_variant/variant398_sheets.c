#include "../../types.h"

#include "model_variant.h"

/* Draws two sheets of four POLY_GT4 quads, the second placed along the
 * variant's path, and grows or shrinks each sheet's size through the
 * variant's phases. */
void func_8013D02C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    u8 *work;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    GsOT *ot;
    POLY_GT4 *poly;
    ModelVariantSheet *sheet;
    s32 i;
    s32 k;
    SVECTOR *v;
    s32 bias;
    s32 otz;

    work = ctx;
    sheet = (ModelVariantSheet *)(ctx + 0x6A8);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(ctx + 0x1888);
    for (i = 0; i < 2; i++, sheet++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x19EC) & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i == 0) {
            m.t[0] = MODEL_VARIANT_WORD(work, 0x19AC);
            m.t[1] = MODEL_VARIANT_WORD(work, 0x19B0);
            m.t[2] = MODEL_VARIANT_WORD(work, 0x19B4);
        } else if (MODEL_VARIANT_WORD(work, 0x1A1C) < 0x400) {
            m.t[0] = MODEL_VARIANT_WORD(work, 0x19AC) + MODEL_VARIANT_WORD(work, 0x19C0) * MODEL_VARIANT_WORD(work, 0x1A1C) / 1024;
            m.t[1] = MODEL_VARIANT_WORD(work, 0x19B0) + MODEL_VARIANT_WORD(work, 0x19C4) * MODEL_VARIANT_WORD(work, 0x1A1C) / 1024;
            m.t[2] = MODEL_VARIANT_WORD(work, 0x19B4) + MODEL_VARIANT_WORD(work, 0x19C8) * MODEL_VARIANT_WORD(work, 0x1A1C) / 1024;
        } else {
            m.t[0] = MODEL_VARIANT_HALF(work, 0x19B8);
            m.t[1] = MODEL_VARIANT_HALF(work, 0x19BA);
            m.t[2] = MODEL_VARIANT_HALF(work, 0x19BC);
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
                GsSortPoly(poly, ot, otz);
            }
        }
        if (i == 0) {
            if (MODEL_VARIANT_WORD(work, 0x1A30) == 0) {
                if (sheet->size < 0x1000) {
                    sheet->size = (u32)((MODEL_VARIANT_WORD(work, 0x19F0) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1A00), 0x1C)) << 12) /
                                  (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1A00), 0x20) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1A00), 0x1C));
                    if (sheet->size >= 0x1000) {
                        sheet->size = 0x1000;
                        MODEL_VARIANT_WORD(work, 0x1A30) = 1;
                    }
                }
            } else {
                sheet->size = MODEL_VARIANT_HALF(work, 0x1A18);
            }
        } else if (MODEL_VARIANT_WORD(work, 0x1A30) == 0) {
            sheet->size = 0;
        } else if (MODEL_VARIANT_WORD(work, 0x1A1C) >= 0x400 && MODEL_VARIANT_WORD(work, 0x1A30) == 2) {
            if (sheet->size < 0x2000) {
                sheet->size += MODEL_VARIANT_WORD(work, 0x19F8) << 10;
                if (sheet->size >= 0x2000) {
                    sheet->size = 0x2000;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0x1A30) < 2) {
            sheet->size = 0x1000;
        } else if (MODEL_VARIANT_WORD(work, 0x1A30) == 3) {
            if (sheet->size > 0) {
                sheet->size -= MODEL_VARIANT_WORD(work, 0x19F8) << 7;
                if (sheet->size <= 0) {
                    sheet->size = 0;
                    MODEL_VARIANT_WORD(work, 0x1A30) = 4;
                }
            }
        }
    }
}
