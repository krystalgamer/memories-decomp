#include "../../types.h"

#include "variant423_bands.h"

/* Draws one band of nine segments as pairs of POLY_GT4 quads along the variant's
 * path, fading its radius in and out with the two phases of the timing record at
 * work + 0xF00. */
void func_8013BF18(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag[1][5];
    u8 *work;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *poly;
    Variant423Band *band;
    Variant423Band *col;
    s16 i;
    s16 j;
    s16 r;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0xD88);
    angle = ratan2(MODEL_VARIANT_HALF(work, 0xED2), MODEL_VARIANT_HALF(work, 0xED0)) + 0x800;
    if (!(MODEL_VARIANT_WORD(work, 0xEEC) & 1)) {
        r = MODEL_VARIANT_HALF(work, 0xF18) * 48 / 4096 +
            (rsin(MODEL_VARIANT_HALF(work, 0xF1E)) * (MODEL_VARIANT_HALF(work, 0xF18) / 256) >> 12);
    } else {
        r = MODEL_VARIANT_HALF(work, 0xF18) * 54 / 4096 +
            (rsin(MODEL_VARIANT_HALF(work, 0xF1E)) * (MODEL_VARIANT_HALF(work, 0xF18) / 256) >> 12);
    }
    MODEL_VARIANT_HALF(work, 0xF1E) += MODEL_VARIANT_WORD(work, 0xEF8) * 256;
    band = (Variant423Band *)(work + 0x4E0);
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 5; j++) {
            setVector(&band->a[j], rcos(0x400) * r >> 12, rsin(0x400) * r >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(0xC00) * r >> 12, rsin(0xC00) * r >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            if (j == 0) {
                m.t[0] = MODEL_VARIANT_WORD(work, 0xEAC);
                m.t[1] = MODEL_VARIANT_WORD(work, 0xEB0);
                m.t[2] = MODEL_VARIANT_WORD(work, 0xEB4);
            } else {
                m.t[0] = MODEL_VARIANT_WORD(work, 0xEAC) + MODEL_VARIANT_WORD(work, 0xEC0) * MODEL_VARIANT_HALF(work, 0xF1C) / 1024 * j / 4;
                m.t[1] = MODEL_VARIANT_WORD(work, 0xEB0) + MODEL_VARIANT_WORD(work, 0xEC4) * MODEL_VARIANT_HALF(work, 0xF1C) / 1024 * j / 4;
                m.t[2] = MODEL_VARIANT_WORD(work, 0xEB4) + MODEL_VARIANT_WORD(work, 0xEC8) * MODEL_VARIANT_HALF(work, 0xF1C) / 1024 * j / 4;
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
    band = (Variant423Band *)(work + 0x4E0);
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
            flag[i][j] = 0;
            if (band->otz[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j + 1]);
            }
            /* Every field is a four-byte array, so moving the record pointer
             * by j words views column j as index 0. */
            col = (Variant423Band *)((s32 *)band + j);
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
            col = (Variant423Band *)((s32 *)band + j + 1);
            flag[i][j] = 0;
            if (band->otz[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j + 1]);
            }
        }
    }
    col = NULL;
    if (MODEL_VARIANT_WORD(work, 0xF24) == 1) {
        if (MODEL_VARIANT_HALF(work, 0xF1C) < 0x401) {
            MODEL_VARIANT_HALF(work, 0xF1C) = (u32)((MODEL_VARIANT_WORD(work, 0xEF0) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF00), 0x20)) << 10) /
                             (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF00), 0x24) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF00), 0x20));
            if (MODEL_VARIANT_HALF(work, 0xF1C) >= 0x400) {
                MODEL_VARIANT_HALF(work, 0xF1C) = 0x400;
                MODEL_VARIANT_WORD(work, 0xF24) = 2;
            }
        }
    } else if (MODEL_VARIANT_WORD(work, 0xF24) < 3 && MODEL_VARIANT_HALF(work, 0xF18) > 0) {
        if ((u32)MODEL_VARIANT_WORD(work, 0xEF0) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF00), 0x28)) {
            MODEL_VARIANT_HALF(work, 0xF18) = 0x1000 - (u32)(((u32)MODEL_VARIANT_WORD(work, 0xEF0) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF00), 0x28)) << 12) /
                                          (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF00), 0x2C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0xF00), 0x28));
            if (MODEL_VARIANT_HALF(work, 0xF18) <= 0) {
                MODEL_VARIANT_HALF(work, 0xF18) = 0;
                MODEL_VARIANT_WORD(work, 0xF24) = 3;
            }
        }
    }
}
