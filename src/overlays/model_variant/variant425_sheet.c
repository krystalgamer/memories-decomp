#include "../../types.h"

#include "model_variant.h"

/* Draws the four-quad sheet and updates its timed scale. After growth,
 * phase 1 also advances the value at +0x2734 to 0x600 before phase 2. */
void func_8013C178(u8 *ctx)
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
    sheet = (ModelVariantSheet *)(ctx + 0x1E68);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(ctx + 0x25A4);
    for (i = 0; i < 1; i++, sheet++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x26F4) & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work, 0x26A8);
        m.t[1] = MODEL_VARIANT_WORD(work, 0x26AC);
        m.t[2] = MODEL_VARIANT_WORD(work, 0x26B0);
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
        if (sheet->size < 0x1000 && MODEL_VARIANT_WORD(work, 0x2748) == 0) {
            sheet->size = (u32)((MODEL_VARIANT_WORD(work, 0x26F8) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x1C)) << 12) /
                          (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x20) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x1C));
            if (sheet->size >= 0x1000) {
                sheet->size = 0x1000;
                MODEL_VARIANT_WORD(work, 0x2748) = 1;
            }
        } else if ((u32)MODEL_VARIANT_WORD(work, 0x26F8) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x2C)) {
            sheet->size = 0x1000 - (u32)((MODEL_VARIANT_WORD(work, 0x26F8) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x2C)) << 12) /
                          (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x30) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x2C));
            if (sheet->size <= 0) {
                sheet->size = 0;
                if (MODEL_VARIANT_WORD(work, 0x2748) == 3) {
                    MODEL_VARIANT_WORD(work, 0x2748) = 4;
                }
            }
        }
        if (MODEL_VARIANT_WORD(work, 0x2748) == 1) {
            if (MODEL_VARIANT_WORD(work, 0x2734) < 0x600) {
                MODEL_VARIANT_WORD(work, 0x2734) += MODEL_VARIANT_WORD(work, 0x2700) * 16;
                if (MODEL_VARIANT_WORD(work, 0x2734) >= 0x600) {
                    MODEL_VARIANT_WORD(work, 0x2734) = 0x600;
                    MODEL_VARIANT_WORD(work, 0x2748) = 2;
                }
            }
        }
    }
}
