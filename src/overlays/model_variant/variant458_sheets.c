#include "../../types.h"

#include "model_variant.h"

/* Draws sixteen four-quad sheets. The first eight follow the timed growth
 * and shrink phases; the remaining eight follow the moving object records. */
void func_8013C968(u8 *ctx)
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
    u8 *work;
    POLY_GT4 *poly;
    ModelVariantSheetSet *sheet;
    s32 i;
    s32 k;
    s32 bias;
    s32 otz;
    s32 off;

    work = ctx;
    obj = ctx + 0x1E4;
    sheet = (ModelVariantSheetSet *)(ctx + 0x1904);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(ctx + 0x2CB8);
    for (i = 0; i < 16; i++, sheet++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x2FB8) & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i < 8) {
            m.t[0] = MODEL_VARIANT_WORD(work + i * 16, 0x2E0C);
            m.t[1] = MODEL_VARIANT_WORD(work + i * 16, 0x2E10);
            m.t[2] = MODEL_VARIANT_WORD(work + i * 16, 0x2E14);
        } else {
            off = (i - 8) * 16;
            m.t[0] = MODEL_VARIANT_WORD(work + off, 0x2E8C);
            m.t[1] = MODEL_VARIANT_WORD(work + off, 0x2E90);
            m.t[2] = MODEL_VARIANT_WORD(work + off, 0x2E94);
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
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
        if (i < 8) {
            if (MODEL_VARIANT_WORD(work, 0x3020) == 1) {
                if (sheet->size < 0x1000) {
                    sheet->size = (u32)((MODEL_VARIANT_WORD(work, 0x2FBC) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2FCC), 0x24)) << 12) /
                                  (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2FCC), 0x28) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2FCC), 0x24));
                    if (sheet->size >= 0x1000) {
                        sheet->size = 0x1000;
                        MODEL_VARIANT_WORD(work, 0x3020) = 2;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x3020) == 4 && sheet->size > 0) {
                if ((u32)MODEL_VARIANT_WORD(work, 0x2FBC) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2FCC), 0x30)) {
                    sheet->size = 0x1000 - (u32)((MODEL_VARIANT_WORD(work, 0x2FBC) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2FCC), 0x30)) << 12) /
                                  (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2FCC), 0x34) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2FCC), 0x30));
                }
                if (sheet->size <= 0) {
                    sheet->size = 0;
                }
            }
        } else {
            if (MODEL_VARIANT_WORD(obj, 0x1C8) == 1) {
                if (sheet->shown == 0) {
                    sheet->size = 0x4000;
                    sheet->shown = 1;
                } else {
                    sheet->size = MODEL_VARIANT_WORD(obj, 0x1CC) * 16;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        if (MODEL_VARIANT_WORD(work, 0x3020) < 4) {
                            sheet->shown = 0;
                        }
                    }
                }
            } else {
                sheet->size = 0;
            }
            obj += 0x2E4;
        }
    }
}
