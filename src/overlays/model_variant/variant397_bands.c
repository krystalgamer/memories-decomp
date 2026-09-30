#include "../../types.h"

#include "model_variant.h"

/* Draws one band of five points as pairs of POLY_GT4 quads along the variant's
 * path. The radius is scaled by the word at + 0x44 of the timing record at
 * work + 0xF54, and fades in and out with that record's two phases. */
void func_8013C22C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    u8 *work;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *poly;
    ModelVariantBandShort *band;
    ModelVariantBandShort *col;
    s16 i;
    s16 j;
    s16 r;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0xD9C);
    angle = ratan2(MODEL_VARIANT_HALF(work, 0xF26), MODEL_VARIANT_HALF(work, 0xF24)) + 0x800;
    if (!(MODEL_VARIANT_WORD(work, 0xF40) & 1)) {
        r = MODEL_VARIANT_HALF(work, 0xF6C) * MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF54), 0x44) / 4096;
    } else {
        r = MODEL_VARIANT_HALF(work, 0xF6C) * (MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF54), 0x44) * 20 / 16) / 4096;
    }
    band = (ModelVariantBandShort *)(work + 0x4E0);
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 5; j++) {
            setVector(&band->a[j], rcos(0x400) * r >> 12, rsin(0x400) * r >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(0xC00) * r >> 12, rsin(0xC00) * r >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            if (j == 0) {
                m.t[0] = MODEL_VARIANT_WORD(work, 0xEC0);
                m.t[1] = MODEL_VARIANT_WORD(work, 0xEC4);
                m.t[2] = MODEL_VARIANT_WORD(work, 0xEC8);
            } else {
                m.t[0] = MODEL_VARIANT_WORD(work, 0xEC0) + MODEL_VARIANT_WORD(work, 0xF14) * MODEL_VARIANT_WORD(work, 0xF70) / 1024 * j / 4;
                m.t[1] = MODEL_VARIANT_WORD(work, 0xEC4) + MODEL_VARIANT_WORD(work, 0xF18) * MODEL_VARIANT_WORD(work, 0xF70) / 1024 * j / 4;
                m.t[2] = MODEL_VARIANT_WORD(work, 0xEC8) + MODEL_VARIANT_WORD(work, 0xF1C) * MODEL_VARIANT_WORD(work, 0xF70) / 1024 * j / 4;
            }
            scale.vx = 0x1000;
            scale.vy = 0x1000;
            scale.vz = 0x1000;
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
            band->otz[j] = RotTransPers3(&band->a[j], &band->b[j], &band->c[j],
                                         &band->sa[j], &band->sb[j],
                                         &band->sc[j], &p, &band->flag[j]);
        }
    }
    band = (ModelVariantBandShort *)(work + 0x4E0);
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 4; j++) {
            poly->x0 = band->sa[j];
            poly->y0 = band->sa[j] >> 16;
            poly->x1 = band->sa[j + 1];
            poly->y1 = band->sa[j + 1] >> 16;
            poly->x2 = band->sb[j];
            poly->y2 = band->sb[j] >> 16;
            poly->x3 = band->sb[j + 1];
            poly->y3 = band->sb[j + 1] >> 16;
            poly->r0 = band->ca[j][0];
            poly->g0 = band->ca[j][1];
            poly->b0 = band->ca[j][2];
            poly->r1 = band->ca[j + 1][0];
            poly->g1 = band->ca[j + 1][1];
            poly->b1 = band->ca[j + 1][2];
            poly->r2 = band->cb[j][0];
            poly->g2 = band->cb[j][1];
            poly->b2 = band->cb[j][2];
            poly->r3 = band->cb[j + 1][0];
            poly->g3 = band->cb[j + 1][1];
            poly->b3 = band->cb[j + 1][2];
            if (band->otz[j] < 0) {
                band->otz[j] = 0;
            }
            band->flag[j] = 0;
            if (band->otz[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
            /* Every field is a four-byte array, so moving the record pointer
             * by j words views column j as index 0. */
            col = (ModelVariantBandShort *)((s32 *)band + j);
            poly->x0 = col->sc[0];
            poly->y0 = col->sc[0] >> 16;
            poly->x1 = col->sc[1];
            poly->y1 = col->sc[1] >> 16;
            poly->x2 = col->sb[0];
            poly->y2 = col->sb[0] >> 16;
            poly->x3 = col->sb[1];
            poly->y3 = col->sb[1] >> 16;
            poly->r0 = col->ca[0][0];
            poly->g0 = col->ca[0][1];
            poly->b0 = col->ca[0][2];
            poly->r1 = band->ca[j + 1][0];
            poly->g1 = band->ca[j + 1][1];
            poly->b1 = band->ca[j + 1][2];
            poly->r2 = col->cb[0][0];
            poly->g2 = col->cb[0][1];
            poly->b2 = col->cb[0][2];
            poly->r3 = band->cb[j + 1][0];
            poly->g3 = band->cb[j + 1][1];
            poly->b3 = band->cb[j + 1][2];
            if (col->otz[0] < 0) {
                col->otz[0] = 0;
            }
            col = (ModelVariantBandShort *)((s32 *)band + j + 1);
            band->flag[j] = 0;
            if (band->otz[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
        }
    }
    col = NULL;
    if (MODEL_VARIANT_WORD(work, 0xF78) == 1) {
        if (MODEL_VARIANT_WORD(work, 0xF70) < 0x401) {
            MODEL_VARIANT_WORD(work, 0xF70) = (u32)((MODEL_VARIANT_WORD(work, 0xF44) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF54), 0x24)) << 10) /
                             (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF54), 0x28) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF54), 0x24));
            if (MODEL_VARIANT_WORD(work, 0xF70) >= 0x400) {
                MODEL_VARIANT_WORD(work, 0xF70) = 0x400;
                MODEL_VARIANT_WORD(work, 0xF78) = 2;
            }
        }
    } else if (MODEL_VARIANT_WORD(work, 0xF78) < 3 && MODEL_VARIANT_HALF(work, 0xF6C) > 0) {
        if ((u32)MODEL_VARIANT_WORD(work, 0xF44) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF54), 0x2C)) {
            MODEL_VARIANT_HALF(work, 0xF6C) = 0x1000 - (u32)(((u32)MODEL_VARIANT_WORD(work, 0xF44) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF54), 0x2C)) << 12) /
                                          (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF54), 0x30) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF54), 0x2C));
            if (MODEL_VARIANT_HALF(work, 0xF6C) <= 0) {
                MODEL_VARIANT_HALF(work, 0xF6C) = 0;
                MODEL_VARIANT_WORD(work, 0xF78) = 3;
            }
        }
    }
}
