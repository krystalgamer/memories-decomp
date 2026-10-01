#include "../../types.h"

#include "model_variant.h"

/* Draws two sheets of four POLY_GT4 quads, the second placed along the
 * variant's path, and grows or shrinks each sheet's size through the
 * variant's phases. */
void func_8013D410(u8 *ctx)
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
    s32 bias;
    s32 otz;

    work = ctx;
    sheet = (ModelVariantSheet *)(ctx + 0x147C);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(ctx + 0x1C84);
    for (i = 0; i < 2; i++, sheet++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x1DB4) & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i == 0) {
            m.t[0] = MODEL_VARIANT_WORD(work, 0x1D74);
            m.t[1] = MODEL_VARIANT_WORD(work, 0x1D78);
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1D7C);
        } else if (MODEL_VARIANT_WORD(work, 0x1E14) < 0x400) {
            m.t[0] = MODEL_VARIANT_WORD(work, 0x1D74) + MODEL_VARIANT_WORD(work, 0x1D88) * MODEL_VARIANT_WORD(work, 0x1E14) / 1024;
            m.t[1] = MODEL_VARIANT_WORD(work, 0x1D78) + MODEL_VARIANT_WORD(work, 0x1D8C) * MODEL_VARIANT_WORD(work, 0x1E14) / 1024;
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1D7C) + MODEL_VARIANT_WORD(work, 0x1D90) * MODEL_VARIANT_WORD(work, 0x1E14) / 1024;
        } else {
            m.t[0] = MODEL_VARIANT_HALF(work, 0x1D80);
            m.t[1] = MODEL_VARIANT_HALF(work, 0x1D82);
            m.t[2] = MODEL_VARIANT_HALF(work, 0x1D84);
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
            if (otz > 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
        if (i == 0) {
            if (MODEL_VARIANT_WORD(work, 0x1E1C) == 0) {
                if (sheet->size < 0x1000) {
                    sheet->size = (u32)((MODEL_VARIANT_WORD(work, 0x1DB8) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1DC8), 0x1C)) << 12) /
                                  (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1DC8), 0x20) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1DC8), 0x1C));
                    if (sheet->size >= 0x1000) {
                        sheet->size = 0x1000;
                        MODEL_VARIANT_WORD(work, 0x1E1C) = 1;
                    }
                }
            } else {
                sheet->size = MODEL_VARIANT_HALF(work, 0x1E10);
            }
        } else if (MODEL_VARIANT_WORD(work, 0x1E1C) == 0) {
            sheet->size = 0;
        } else if (MODEL_VARIANT_WORD(work, 0x1E14) >= 0x400 && MODEL_VARIANT_WORD(work, 0x1E1C) == 2) {
            if (sheet->size < 0x2000) {
                sheet->size += MODEL_VARIANT_WORD(work, 0x1DC0) << 10;
                if (sheet->size >= 0x2000) {
                    sheet->size = 0x2000;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0x1E1C) < 2) {
            sheet->size = 0x1000;
        } else if (MODEL_VARIANT_WORD(work, 0x1E1C) == 3) {
            if (sheet->size > 0) {
                sheet->size -= MODEL_VARIANT_WORD(work, 0x1DC0) << 6;
                if (sheet->size <= 0) {
                    sheet->size = 0;
                    MODEL_VARIANT_WORD(work, 0x1E1C) = 4;
                }
            }
        }
    }
}
