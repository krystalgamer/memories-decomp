#include "../../types.h"

#include "model_variant.h"

/* Draws a variable set of sheets: the count and how many follow the variant's
 * own slots come from the timing record at work + 0x2E7C. The first sheets sit on
 * 0x20-byte slots at work + 0x2C50 and grow with the timing phases; the rest sit
 * on the VECTOR table at work + 0x2D3C and follow the 0x2E8-byte objects that
 * start at ctx. */
void func_8013C808(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *obj;
    GsOT *ot;
    s32 count;
    s32 first;
    u8 *work;
    u8 *rec;
    POLY_GT4 *poly;
    ModelVariantSheetSet *sheet;
    s32 i;
    s32 k;
    SVECTOR *v;
    s32 bias;
    s32 otz;
    s32 off;

    sheet = (ModelVariantSheetSet *)(ctx + 0x1740);
    poly = (POLY_GT4 *)(ctx + 0x2B18);
    obj = work = ctx;
    ot = func_80058F10();
    rec = (u8 *G32)MODEL_VARIANT_WORD(obj, 0x2E7C);
    if (MODEL_VARIANT_WORD(rec, 0x24) == 0) {
        if (MODEL_VARIANT_WORD(rec, 0x28) == 0) {
            count = 2;
            first = 1;
        } else if (MODEL_VARIANT_WORD(rec, 0x28) == 2) {
            count = 3;
            first = 2;
        } else {
            count = MODEL_VARIANT_WORD(rec, 0x20) + 1;
            first = MODEL_VARIANT_WORD(rec, 0x20);
        }
    } else if (MODEL_VARIANT_WORD(rec, 0x28) == 0) {
        first = 1;
        count = MODEL_VARIANT_WORD(rec, 0x20) + 1;
    } else if (MODEL_VARIANT_WORD(rec, 0x28) == 2) {
        first = 2;
        count = MODEL_VARIANT_WORD(rec, 0x20) + 2;
    } else {
        count = MODEL_VARIANT_WORD(rec, 0x20) * 2;
        first = MODEL_VARIANT_WORD(rec, 0x20);
    }
    poly = (POLY_GT4 *)((u8 *)poly + 0x34);
    for (i = 0; i < count; i++, sheet++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x2E68) & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i < first) {
            m.t[0] = MODEL_VARIANT_WORD(work + i * 0x20, 0x2C50);
            m.t[1] = MODEL_VARIANT_WORD(work + i * 0x20, 0x2C54);
            m.t[2] = MODEL_VARIANT_WORD(work + i * 0x20, 0x2C58);
        } else {
            off = (i - first) * 16;
            m.t[0] = MODEL_VARIANT_WORD(work + off, 0x2D3C);
            m.t[1] = MODEL_VARIANT_WORD(work + off, 0x2D40);
            m.t[2] = MODEL_VARIANT_WORD(work + off, 0x2D44);
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
            otz = otz * 7 / 10;
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
        if (i < first) {
            if (MODEL_VARIANT_WORD(work, 0x2EC4) == 0) {
                if (sheet->size < 0x1000) {
                    sheet->size = (u32)((MODEL_VARIANT_WORD(work, 0x2E6C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2E7C), 0x2C)) << 12) /
                                  (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2E7C), 0x30) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2E7C), 0x2C));
                    if (sheet->size >= 0x1000) {
                        sheet->size = 0x1000;
                        MODEL_VARIANT_WORD(work, 0x2EC4) = 1;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x2EC4) == 3 && sheet->size > 0) {
                if ((u32)MODEL_VARIANT_WORD(work, 0x2E6C) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2E7C), 0x38)) {
                    sheet->size = 0x1000 - (u32)((MODEL_VARIANT_WORD(work, 0x2E6C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2E7C), 0x38)) << 12) /
                                  (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2E7C), 0x3C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2E7C), 0x38));
                }
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else {
            if (MODEL_VARIANT_WORD(obj, 0x248) == 1) {
                if (sheet->shown == 0) {
                    sheet->size = 0x4000;
                    sheet->shown = 1;
                } else {
                    sheet->size = MODEL_VARIANT_WORD(obj, 0x24C) * 16;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        if (MODEL_VARIANT_WORD(work, 0x2EC4) < 4) {
                            sheet->shown = 0;
                        }
                    }
                }
            } else {
                sheet->size = 0;
            }
            obj += 0x2E8;
        }
    }
}
