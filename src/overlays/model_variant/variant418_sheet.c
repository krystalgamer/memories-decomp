#include "../../types.h"

#include "model_variant.h"

/* Draws one sheet of four POLY_GT4 quads at the variant's origin and grows or
 * shrinks its size along the two phases of the pointed-to timing record. */
void func_8013CAA4(u8 *ctx)
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
    sheet = (ModelVariantSheet *)(ctx + 0x12DC);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(ctx + 0x1A18);
    for (i = 0; i < 1; i++, sheet++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x1B58) & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work, 0x1B08);
        m.t[1] = MODEL_VARIANT_WORD(work, 0x1B0C);
        m.t[2] = MODEL_VARIANT_WORD(work, 0x1B10);
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
        if (sheet->size < 0x1000 && MODEL_VARIANT_WORD(work, 0x1B9C) == 0) {
            sheet->size = (u32)((MODEL_VARIANT_WORD(work, 0x1B5C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B74), 0x28)) << 12) /
                          (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B74), 0x2C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B74), 0x28));
            if (sheet->size >= 0x1000) {
                sheet->size = 0x1000;
                if (MODEL_VARIANT_WORD(work, 0x1B9C) == 0) {
                    MODEL_VARIANT_WORD(work, 0x1B9C) = 1;
                }
            }
        } else if ((u32)MODEL_VARIANT_WORD(work, 0x1B5C) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B74), 0x38)) {
            sheet->size = 0x1000 - (u32)((MODEL_VARIANT_WORD(work, 0x1B5C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B74), 0x38)) << 12) /
                          (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B74), 0x3C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B74), 0x38));
            if (sheet->size <= 0) {
                sheet->size = 0;
                if (MODEL_VARIANT_WORD(work, 0x1B9C) == 3) {
                    MODEL_VARIANT_WORD(work, 0x1B9C) = 4;
                }
            }
        }
    }
}
