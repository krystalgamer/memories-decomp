#include "../../types.h"

#include "model_variant.h"

/* Header 404's form of the header-425 ribbons: the RotTransPers4 flag of each
 * point is kept, and a point is drawn only when its depth and flag are not
 * negative. */
void func_8013C1D4(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG status[8][2];
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    u8 *timing;
    GsOT *ot;
    s16 i;
    s32 turn;
    s32 tilt;
    ModelVariantRibbonShort *ribbon;
    POLY_G3 *prim;
    s16 k;
    s16 angle;
    s16 length;
    s32 flags;
    s16 radius;
    s32 dx;
    s32 dy;

    work = ctx;
    ot = func_80058F10();
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x190C), MODEL_VARIANT_WORD(work, 0x1904)) + 0xC00;
    ratan2(MODEL_VARIANT_WORD(work, 0x1908), MODEL_VARIANT_WORD(work, 0x1904));
    ribbon = (ModelVariantRibbonShort *)(work + 0xAB0);
    tilt = ratan2(MODEL_VARIANT_WORD(work, 0x18F8), MODEL_VARIANT_WORD(work, 0x18F4)) + 0x400;
    ratan2(MODEL_VARIANT_WORD(work, 0x18F8), MODEL_VARIANT_WORD(work, 0x18F0));
    timing = work + 0xFD8;
    prim = (POLY_G3 *)(work + 0x1738);
    if (MODEL_VARIANT_HALF(work, 0x1974) == 1) {
        turn = -turn;
    }
    length = 16;
    flags = MODEL_VARIANT_WORD(work, 0x191C);
    for (i = 0, angle = MODEL_VARIANT_WORD(work, 0x195C); i < 8;
         i++, angle = MODEL_VARIANT_WORD(work, 0x195C) + i * 0x200, ribbon++) {
        for (k = 0; k < 2; k++) {
            radius = 0x96;
            if (k == 0) {
                radius = 0x28;
            }
            setVector(&ribbon->a[k], rcos(angle) * radius >> 12, rsin(angle) * radius >> 12, k * ((flags & 1) * 32 + 0xA0));
            setVector(&ribbon->b[k], ribbon->a[k].vx + (rcos(turn) * length >> 12), ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(turn) * length >> 12));
        }
    }
    rot.vx = tilt;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x189C) + MODEL_VARIANT_WORD(work, 0x18F0) * MODEL_VARIANT_WORD(work, 0x1954) / 1024;
    m.t[1] = MODEL_VARIANT_WORD(work, 0x18A0) + MODEL_VARIANT_WORD(work, 0x18F4) * MODEL_VARIANT_WORD(work, 0x1954) / 1024;
    m.t[2] = MODEL_VARIANT_WORD(work, 0x18A4) + MODEL_VARIANT_WORD(work, 0x18F8) * MODEL_VARIANT_WORD(work, 0x1954) / 1024;
    scale.vx = MODEL_VARIANT_WORD(timing, 0x88);
    scale.vy = MODEL_VARIANT_WORD(timing, 0x88);
    scale.vz = MODEL_VARIANT_WORD(timing, 0x88);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    ribbon = (ModelVariantRibbonShort *)(work + 0xAB0);
    for (i = 0; i < 8; i++, ribbon++) {
        for (k = 0; k < 2; k++) {
            if (k == 0) {
                ribbon->otz[0] = RotTransPers4(&ribbon->a[0], &ribbon->a[1], &ribbon->a[0], &ribbon->a[1],
                                               &ribbon->sa[0], &ribbon->sa[1], &ribbon->sa[0], &ribbon->sa[1], &p, &status[i][0]);
                dx = (s16)ribbon->sa[1] - (s16)ribbon->sa[0];
                dy = (ribbon->sa[1] >> 16) - (ribbon->sa[0] >> 16);
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k], &ribbon->a[k - 1], &ribbon->a[k],
                                               &ribbon->sa[k - 1], &ribbon->sa[k], &ribbon->sa[k - 1], &ribbon->sa[k], &p,
                                               &status[i][k]);
                dx = (s16)ribbon->sa[k] - (s16)ribbon->sa[k - 1];
                dy = (ribbon->sa[k] >> 16) - (ribbon->sa[k - 1] >> 16);
            }
            RotTransPers(&ribbon->b[k], &ribbon->sb[k], &p, &flag);
            ribbon->angle[k] = ratan2(dy, dx) + 0xC00;
            ribbon->width[k] = (s16)ribbon->sb[k] - (s16)ribbon->sa[k];
            ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
            ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
        }
    }
    ribbon = (ModelVariantRibbonShort *)(work + 0xAB0);
    for (i = 0; i < 8; i++, ribbon++) {
        for (k = 0; k < 1; k++) {
            prim->x0 = ribbon->sa[k] + ribbon->ox[k];
            prim->y0 = (ribbon->sa[k] >> 16) + ribbon->oy[k];
            prim->x1 = ribbon->sa[k + 1];
            prim->y1 = ribbon->sa[k + 1] >> 16;
            prim->x2 = ribbon->sa[k] - ribbon->ox[k];
            prim->y2 = (ribbon->sa[k] >> 16) - ribbon->oy[k];
            prim->r0 = 0x80;
            prim->g0 = 0x80;
            prim->b0 = 0x80;
            prim->r1 = 0x40;
            prim->g1 = 0x60;
            prim->b1 = 0xFF;
            prim->r2 = 0x80;
            prim->g2 = 0x80;
            prim->b2 = 0x80;
            if (ribbon->otz[k] >= 0 && status[i][k] >= 0) {
                func_8005B260((u32 *)prim, ot, (u16)ribbon->otz[k], 1);
            }
        }
    }
    if (MODEL_VARIANT_HALF(work, 0x194C) + 1 == *(u16 *G32)(MODEL_VARIANT_WORD(work, 0x1938) + 0x18)) {
        MODEL_VARIANT_WORD(work, 0x195C) += MODEL_VARIANT_WORD(work, 0x1928) << 5;
    }
}
