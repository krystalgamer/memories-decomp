#include "../../types.h"

#include "model_variant.h"

/* Draws a four-quad sheet at the variant origin. It grows to 0x1000 on
 * the first timing interval, expands to 0x2000, then shrinks to zero. */
void func_8013C360(u8 *ctx)
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
    sheet = (ModelVariantSheet *)(ctx + 0xF60);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(ctx + 0x22E8);
    for (i = 0; i < 1; i++, sheet++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x27AC) & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work, 0x274C);
        m.t[1] = MODEL_VARIANT_WORD(work, 0x2750);
        m.t[2] = MODEL_VARIANT_WORD(work, 0x2754);
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
        if (sheet->size < 0x1000 && MODEL_VARIANT_WORD(work, 0x284C) == 0) {
            sheet->size = (u32)((MODEL_VARIANT_WORD(work, 0x27B0) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x27C0), 0x1C)) << 12) /
                          (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x27C0), 0x20) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x27C0), 0x1C));
            if (sheet->size >= 0x1000) {
                sheet->size = 0x1000;
                MODEL_VARIANT_WORD(work, 0x284C) = 1;
            }
        } else if ((u32)MODEL_VARIANT_WORD(work, 0x27B0) < (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x27C0), 0x24)) {
            sheet->size += MODEL_VARIANT_WORD(work, 0x27B8) * 512;
            if (sheet->size >= 0x2000) {
                sheet->size = 0x2000;
            }
        } else {
            sheet->size = 0x2000 - (u32)((MODEL_VARIANT_WORD(work, 0x27B0) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x27C0), 0x24)) << 13) /
                          (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x27C0), 0x28) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x27C0), 0x24));
            if (sheet->size <= 0) {
                sheet->size = 0;
            }
        }
    }
}
