#include "../../types.h"

#include "model_variant.h"

/* Draws one band of nine segments as pairs of POLY_GT4 quads along the variant's
 * path, fading its radius in and out with the two phases of the timing record at
 * work + 0x2714. */
void func_8013E2F4(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    u8 *work;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *poly;
    ModelVariantBand *band;
    s16 i;
    s16 j;
    s16 r;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x2570);
    angle = ratan2(MODEL_VARIANT_HALF(work, 0x26DA), MODEL_VARIANT_HALF(work, 0x26D8)) + 0x800;
    if (!(MODEL_VARIANT_WORD(work, 0x26F4) & 1)) {
        r = MODEL_VARIANT_HALF(work, 0x272C) / 64;
    } else {
        r = MODEL_VARIANT_HALF(work, 0x272C) * 70 / 4096;
    }
    band = (ModelVariantBand *)(work + 0x1CA0);
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 9; j++) {
            setVector(&band->a[j], rcos(0x400) * r >> 12, rsin(0x400) * r >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(0xC00) * r >> 12, rsin(0xC00) * r >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            if (j == 0) {
                m.t[0] = MODEL_VARIANT_WORD(work, 0x2694);
                m.t[1] = MODEL_VARIANT_WORD(work, 0x2698);
                m.t[2] = MODEL_VARIANT_WORD(work, 0x269C);
            } else {
                m.t[0] = MODEL_VARIANT_WORD(work, 0x2694) + MODEL_VARIANT_WORD(work, 0x26B8) * MODEL_VARIANT_WORD(work, 0x2730) / 1024 * j / 8;
                m.t[1] = MODEL_VARIANT_WORD(work, 0x2698) + MODEL_VARIANT_WORD(work, 0x26BC) * MODEL_VARIANT_WORD(work, 0x2730) / 1024 * j / 8;
                m.t[2] = MODEL_VARIANT_WORD(work, 0x269C) + MODEL_VARIANT_WORD(work, 0x26C0) * MODEL_VARIANT_WORD(work, 0x2730) / 1024 * j / 8;
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
                                         &band->sc[j], &p, &flag);
        }
    }
    band = (ModelVariantBand *)(work + 0x1CA0);
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
            if (band->otz[j] > 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
            poly->x0 = band->sc[j];
            poly->y0 = band->sc[j] >> 16;
            poly->x1 = band->sc[j + 1];
            poly->y1 = band->sc[j + 1] >> 16;
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
            if (band->otz[j] > 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x2748) == 1) {
        if (MODEL_VARIANT_WORD(work, 0x2730) < 0x401) {
            MODEL_VARIANT_WORD(work, 0x2730) = (u32)((MODEL_VARIANT_WORD(work, 0x26F8) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x20)) << 10) /
                             (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x24) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x20));
            if (MODEL_VARIANT_WORD(work, 0x2730) >= 0x400) {
                MODEL_VARIANT_WORD(work, 0x2730) = 0x400;
                MODEL_VARIANT_WORD(work, 0x2748) = 2;
            }
        }
    } else if (MODEL_VARIANT_WORD(work, 0x2748) < 3 && MODEL_VARIANT_HALF(work, 0x272C) > 0) {
        if ((u32)MODEL_VARIANT_WORD(work, 0x26F8) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x28)) {
            MODEL_VARIANT_HALF(work, 0x272C) = 0x1000 - (u32)(((u32)MODEL_VARIANT_WORD(work, 0x26F8) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x28)) << 12) /
                                          (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x2C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2714), 0x28));
            if (MODEL_VARIANT_HALF(work, 0x272C) <= 0) {
                MODEL_VARIANT_HALF(work, 0x272C) = 0;
                MODEL_VARIANT_WORD(work, 0x2748) = 3;
            }
        }
    }
}
