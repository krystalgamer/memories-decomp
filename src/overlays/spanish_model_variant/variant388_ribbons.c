#include "../../types.h"

#include "variant388_ribbons.h"

/* Builds eight two-point ribbons fanned 0x200 apart from the angle at
 * work + 0x1894, projects each spine and its sideways copy, and draws the
 * first point of each as a POLY_G3 as wide as the projected offset when
 * both its depth and its projection flag are non-negative. The fan turns
 * by step * 32 on the last frame of the timing record. */
void func_8013C344(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    u8 *timing;
    GsOT *ot;
    s16 i;
    s32 turn;
    s32 tilt;
    Ribbon388 *ribbon;
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
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x1848), MODEL_VARIANT_WORD(work, 0x1840)) + 0xC00;
    ratan2(MODEL_VARIANT_WORD(work, 0x1844), MODEL_VARIANT_WORD(work, 0x1840));
    ribbon = (Ribbon388 *)(work + 0x228);
    tilt = ratan2(MODEL_VARIANT_WORD(work, 0x1834), MODEL_VARIANT_WORD(work, 0x1830)) + 0x400;
    ratan2(MODEL_VARIANT_WORD(work, 0x1834), MODEL_VARIANT_WORD(work, 0x182C));
    timing = work + 0xF8;
    prim = (POLY_G3 *)(work + 0x1680);
    if (MODEL_VARIANT_HALF(work, 0x18AC) == 1) {
        turn = -turn;
    }
    length = 16;
    flags = MODEL_VARIANT_WORD(work, 0x1858);
    for (i = 0, angle = MODEL_VARIANT_WORD(work, 0x1894); i < 8;
         i++, angle = MODEL_VARIANT_WORD(work, 0x1894) + i * 0x200, ribbon++) {
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
    m.t[0] = MODEL_VARIANT_WORD(work, 0x1818) + MODEL_VARIANT_WORD(work, 0x182C) * MODEL_VARIANT_HALF(work, 0x1890) / 1024;
    m.t[1] = MODEL_VARIANT_WORD(work, 0x181C) + MODEL_VARIANT_WORD(work, 0x1830) * MODEL_VARIANT_HALF(work, 0x1890) / 1024;
    m.t[2] = MODEL_VARIANT_WORD(work, 0x1820) + MODEL_VARIANT_WORD(work, 0x1834) * MODEL_VARIANT_HALF(work, 0x1890) / 1024;
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
    ribbon = (Ribbon388 *)(work + 0x228);
    for (i = 0; i < 8; i++, ribbon++) {
        for (k = 0; k < 2; k++) {
            if (k == 0) {
                ribbon->otz[0] = RotTransPers4(&ribbon->a[0], &ribbon->a[1], &ribbon->a[0], &ribbon->a[1],
                                               &ribbon->sa[0], &ribbon->sa[1], &ribbon->sa[0], &ribbon->sa[1], &p,
                                               &ribbon->flag[0]);
                dx = (s16)ribbon->sa[k + 1] - (s16)ribbon->sa[k];
                dy = (ribbon->sa[k + 1] >> 16) - (ribbon->sa[k] >> 16);
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k], &ribbon->a[k - 1], &ribbon->a[k],
                                               &ribbon->sa[k - 1], &ribbon->sa[k], &ribbon->sa[k - 1], &ribbon->sa[k], &p,
                                               &ribbon->flag[k]);
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
    ribbon = (Ribbon388 *)(work + 0x228);
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
            prim->r1 = 0xFF;
            prim->g1 = 0;
            prim->b1 = 0xFF;
            prim->r2 = 0x80;
            prim->g2 = 0x80;
            prim->b2 = 0x80;
            if (ribbon->otz[k] >= 0 && ribbon->flag[k] >= 0) {
                func_8005B260((u32 *)prim, ot, (u16)ribbon->otz[k], 1);
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x1880) + 1 == *(u16 *)(MODEL_VARIANT_WORD(work, 0x186C) + 0xC)) {
        MODEL_VARIANT_WORD(work, 0x1894) += MODEL_VARIANT_WORD(work, 0x1864) << 5;
    }
}
