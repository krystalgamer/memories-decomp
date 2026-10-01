#include "../../types.h"

#include "model_variant.h"

/* Draws one band of nine segments as pairs of POLY_GT4 quads along the variant's
 * path, fading its radius in and out with the two phases of the timing record at
 * work + 0x194C. */
void func_8013C70C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag[1][9];
    u8 *work;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *poly;
    ModelVariantBand *band;
    ModelVariantBand *col;
    s16 i;
    s16 j;
    s16 r;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x17A0);
    angle = ratan2(MODEL_VARIANT_HALF(work, 0x191E), MODEL_VARIANT_HALF(work, 0x191C)) + 0x800;
    if (!(MODEL_VARIANT_WORD(work, 0x1938) & 1)) {
        r = MODEL_VARIANT_HALF(work, 0x1964) / 256;
    } else {
        r = MODEL_VARIANT_HALF(work, 0x1964) * 24 / 4096;
    }
    band = (ModelVariantBand *)(work + 0x4E0);
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 9; j++) {
            setVector(&band->a[j], rcos(0x400) * r >> 12, rsin(0x400) * r >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(0xC00) * r >> 12, rsin(0xC00) * r >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            if (j == 0) {
                m.t[0] = MODEL_VARIANT_WORD(work, 0x18F8);
                m.t[1] = MODEL_VARIANT_WORD(work, 0x18FC);
                m.t[2] = MODEL_VARIANT_WORD(work, 0x1900);
            } else {
                m.t[0] = MODEL_VARIANT_WORD(work, 0x18F8) + MODEL_VARIANT_WORD(work, 0x190C) * MODEL_VARIANT_WORD(work, 0x1968) / 1024 * j / 8;
                m.t[1] = MODEL_VARIANT_WORD(work, 0x18FC) + MODEL_VARIANT_WORD(work, 0x1910) * MODEL_VARIANT_WORD(work, 0x1968) / 1024 * j / 8;
                m.t[2] = MODEL_VARIANT_WORD(work, 0x1900) + MODEL_VARIANT_WORD(work, 0x1914) * MODEL_VARIANT_WORD(work, 0x1968) / 1024 * j / 8;
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
                                         &band->sc[j], &p, &flag[i][j]);
        }
    }
    band = (ModelVariantBand *)(work + 0x4E0);
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 8; j++) {
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
            flag[i][j] = 0;
            if (band->otz[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
            /* Every field is a four-byte array, so moving the record pointer
             * by j words views column j as index 0. */
            col = (ModelVariantBand *)((s32 *)band + j);
            poly->x0 = col->sc[0];
            poly->y0 = col->sc[0] >> 16;
#ifdef VERSION_FRENCH
            poly->x1 = band->sc[j + 1];
            poly->y1 = band->sc[j + 1] >> 16;
#else
            poly->x1 = col->sc[1];
            poly->y1 = col->sc[1] >> 16;
#endif
            poly->x2 = col->sb[0];
            poly->y2 = col->sb[0] >> 16;
#ifdef VERSION_FRENCH
            poly->x3 = band->sb[j + 1];
            poly->y3 = band->sb[j + 1] >> 16;
#else
            poly->x3 = col->sb[1];
            poly->y3 = col->sb[1] >> 16;
#endif
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
            col = (ModelVariantBand *)((s32 *)band + j + 1);
            flag[i][j] = 0;
            if (band->otz[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
        }
    }
    col = NULL;
    if (MODEL_VARIANT_WORD(work, 0x197C) == 1) {
        if (MODEL_VARIANT_WORD(work, 0x1968) < 0x401) {
            MODEL_VARIANT_WORD(work, 0x1968) = (u32)((MODEL_VARIANT_WORD(work, 0x193C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x194C), 0x20)) << 10) /
                             (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x194C), 0x24) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x194C), 0x20));
            if (MODEL_VARIANT_WORD(work, 0x1968) >= 0x400) {
                MODEL_VARIANT_WORD(work, 0x1968) = 0x400;
                MODEL_VARIANT_WORD(work, 0x197C) = 2;
            }
        }
    } else if (MODEL_VARIANT_WORD(work, 0x197C) < 3 && MODEL_VARIANT_HALF(work, 0x1964) > 0) {
        if ((u32)MODEL_VARIANT_WORD(work, 0x193C) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x194C), 0x28)) {
            MODEL_VARIANT_HALF(work, 0x1964) = 0x1000 - (u32)(((u32)MODEL_VARIANT_WORD(work, 0x193C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x194C), 0x28)) << 12) /
                                          (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x194C), 0x2C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x194C), 0x28));
            if (MODEL_VARIANT_HALF(work, 0x1964) <= 0) {
                MODEL_VARIANT_HALF(work, 0x1964) = 0;
                MODEL_VARIANT_WORD(work, 0x197C) = 3;
            }
        }
    }
}
