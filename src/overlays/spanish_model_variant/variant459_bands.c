#include "../../types.h"

#include "../model_variant/model_variant.h"

/* Draws every band of the timing record's band count as nine-point strips of
 * paired POLY_GT4 quads along the variant's direction. The radius follows the
 * record's size and the shrinking half-word at work + 0x30CC; the bands grow
 * along their own directions through phase 1 and shrink from the record's
 * fade window until phase 3. */
void func_8013E224(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag[6][9];
    u8 *work;
    GsOT *ot;
    POLY_GT4 *poly;
    ModelVariantBand *band;
    ModelVariantBand *col;
    s16 i;
    s16 j;
    s16 r;
    s32 angle;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x2D60);
    angle = ratan2(MODEL_VARIANT_HALF(work, 0x3066), MODEL_VARIANT_HALF(work, 0x3064)) + 0x800;
    if (!(MODEL_VARIANT_WORD(work, 0x3080) & 1)) {
        r = MODEL_VARIANT_HALF(work, 0x30CC) * *(u16 *)(MODEL_VARIANT_WORD(work, 0x309C) + 0x2A) / 4096;
    } else {
        r = MODEL_VARIANT_HALF(work, 0x30CC) *
            (*(u16 *)(MODEL_VARIANT_WORD(work, 0x309C) + 0x2A) +
             (rsin(0x100) * *(u16 *)(MODEL_VARIANT_WORD(work, 0x309C) + 0x2A) >> 12)) / 4096;
    }
    band = (ModelVariantBand *)(work + 0x1AB8);
    for (i = 0; i < *(u16 *)(MODEL_VARIANT_WORD(work, 0x309C) + 0x24); i++, band++) {
        for (j = 0; j < 9; j++) {
            setVector(&band->a[j], rcos(0x400) * r >> 12, rsin(0x400) * r >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(0xC00) * r >> 12, rsin(0xC00) * r >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            if (j == 0) {
                m.t[0] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F8C);
                m.t[1] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F90);
                m.t[2] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F94);
            } else {
                m.t[0] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F8C) + MODEL_VARIANT_WORD(work + (i << 4), 0x2FEC) * MODEL_VARIANT_WORD(work, 0x30D0) / 1024 * j / 8;
                m.t[1] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F90) + MODEL_VARIANT_WORD(work + (i << 4), 0x2FF0) * MODEL_VARIANT_WORD(work, 0x30D0) / 1024 * j / 8;
                m.t[2] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F94) + MODEL_VARIANT_WORD(work + (i << 4), 0x2FF4) * MODEL_VARIANT_WORD(work, 0x30D0) / 1024 * j / 8;
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
    band = (ModelVariantBand *)(work + 0x1AB8);
    for (i = 0; i < *(u16 *)(MODEL_VARIANT_WORD(work, 0x309C) + 0x24); i++, band++) {
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
            poly->x1 = band->sc[j + 1];
            poly->y1 = band->sc[j + 1] >> 16;
            poly->x2 = col->sb[0];
            poly->y2 = col->sb[0] >> 16;
            poly->x3 = band->sb[j + 1];
            poly->y3 = band->sb[j + 1] >> 16;
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
    if (MODEL_VARIANT_WORD(work, 0x30DC) == 1) {
        if (MODEL_VARIANT_WORD(work, 0x30D0) < 0x401) {
            MODEL_VARIANT_WORD(work, 0x30D0) = (u32)((MODEL_VARIANT_WORD(work, 0x3084) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x309C), 0x38)) << 10) /
                             (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x309C), 0x3C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x309C), 0x38));
            if (MODEL_VARIANT_WORD(work, 0x30D0) >= 0x400) {
                MODEL_VARIANT_WORD(work, 0x30D0) = 0x400;
                MODEL_VARIANT_WORD(work, 0x30DC) = 2;
            }
        }
    } else if (MODEL_VARIANT_WORD(work, 0x30DC) < 3 && MODEL_VARIANT_HALF(work, 0x30CC) > 0) {
        if ((u32)MODEL_VARIANT_WORD(work, 0x3084) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x309C), 0x40)) {
            MODEL_VARIANT_HALF(work, 0x30CC) = 0x1000 - (u32)(((u32)MODEL_VARIANT_WORD(work, 0x3084) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x309C), 0x40)) << 12) /
                                          (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x309C), 0x44) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x309C), 0x40));
            if (MODEL_VARIANT_HALF(work, 0x30CC) <= 0) {
                MODEL_VARIANT_HALF(work, 0x30CC) = 0;
                MODEL_VARIANT_WORD(work, 0x30DC) = 3;
            }
        }
    }
}
