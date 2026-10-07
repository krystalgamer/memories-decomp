#include "../../types.h"

#include "variant456_ribbons.h"

/* Two three-point ribbons. Each spine is placed at i / 2 of the offset at
 * +0x22F0 and copied 48 units along the view; both are projected, and every
 * point is pushed sideways along its screen normal by a sway built from the
 * phase at +0x235C and the spine wave. Each segment is drawn as a POLY_FT4 as
 * wide as the scaled projected offset. */
void func_8013C834(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    GsOT *ot;
    s16 i;
    s16 j;
    s32 turn;
    s32 wave;
    s32 base; /* never assigned: the retail code reads its stack slot */
    s32 size;
    s32 length;
    s32 phase;
    s32 width;
    s32 amp;
    s32 dx;
    s32 dy;
    Ribbon456 *ribbon;
    POLY_FT4 *poly;

    work = ctx;
    ribbon = (Ribbon456 *)(work + 0x280);
    ot = func_80058F10();
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x230C), MODEL_VARIANT_WORD(work, 0x2304)) + 0xC00;
    ratan2(MODEL_VARIANT_WORD(work, 0x2308), MODEL_VARIANT_WORD(work, 0x2304));
    length = 48;
    for (i = 0; i < 2; i++, ribbon++) {
        for (j = 0; j < 3; j++, wave = j * 0x800) {
            setVector(&ribbon->a[j], MODEL_VARIANT_WORD(work, 0x22F0) * i / 2, MODEL_VARIANT_WORD(work, 0x22F4) * i / 2,
                      MODEL_VARIANT_WORD(work, 0x22F8) * i / 2);
            setVector(&ribbon->b[j], ribbon->a[j].vx + (rcos(turn) * length >> 12), ribbon->a[j].vy,
                      ribbon->a[j].vz + (rsin(turn) * length >> 12));
        }
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x22DC);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x22E0);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x22E4);
    scale.vx = 0x1000;
    scale.vy = 0x1000;
    scale.vz = 0x1000;
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    ribbon = (Ribbon456 *)(work + 0x280);
    for (i = 0; i < 2; i++, ribbon++) {
        for (j = 0; j < 3; j++) {
            if (j == 2) {
                ribbon->otz[j] = RotTransPers4(&ribbon->a[1], &ribbon->a[2], &ribbon->a[1], &ribbon->a[2],
                                             &ribbon->sa[1], &ribbon->sa[2], &ribbon->sa[1], &ribbon->sa[2], &p, &flag);
                RotTransPers(&ribbon->b[2], &ribbon->sb[2], &p, &flag);
                dx = (s16)ribbon->sa[j] - (s16)ribbon->sa[j - 1];
                dy = (ribbon->sa[j] >> 16) - (ribbon->sa[j - 1] >> 16);
            } else {
                ribbon->otz[j] = RotTransPers4(&ribbon->a[j], &ribbon->a[j + 1], &ribbon->a[j], &ribbon->a[j + 1],
                                             &ribbon->sa[j], &ribbon->sa[j + 1], &ribbon->sa[j], &ribbon->sa[j + 1], &p, &flag);
                RotTransPers(&ribbon->b[j], &ribbon->sb[j], &p, &flag);
                dx = (s16)ribbon->sa[j + 1] - (s16)ribbon->sa[j];
                dy = (ribbon->sa[j + 1] >> 16) - (ribbon->sa[j] >> 16);
            }
            ribbon->angle[j] = ratan2(dy, dx) - 0x400;
            ribbon->width[j] = (s16)ribbon->sb[j] - (s16)ribbon->sa[j];
            size = ribbon->scale * ribbon->width[j] / 1024;
            ribbon->ox[j] = rcos(ribbon->angle[j]) * size >> 12;
            ribbon->oy[j] = rsin(ribbon->angle[j]) * size >> 12;
            if (j == 0 || j == 2) {
                phase = 0;
            } else {
                phase = base - j * 0xC00 + MODEL_VARIANT_WORD(work, 0x235C);
            }
            width = ribbon->width[j];
            amp = (width << 7) / 48;
            ribbon->sa[j] += rcos(ribbon->angle[j]) * ((rsin(phase) * amp >> 12) + (rsin(wave) * width >> 12)) >> 12;
            ribbon->sa[j] += (rsin(ribbon->angle[j]) * ((rsin(phase) * amp >> 12) + (rsin(wave) * width >> 12)) >> 12) << 16;
        }
    }
    ribbon = (Ribbon456 *)(work + 0x280);
    for (i = 0; i < 2; i++, ribbon++) {
        poly = (POLY_FT4 *)(work + 0x2204);
        for (j = 0; j < 2; j++) {
            poly->x0 = ribbon->sa[j] + ribbon->ox[j];
            poly->y0 = (ribbon->sa[j] >> 16) + ribbon->oy[j];
            poly->x1 = ribbon->sa[j + 1] + ribbon->ox[j + 1];
            poly->y1 = (ribbon->sa[j + 1] >> 16) + ribbon->oy[j + 1];
            poly->x2 = ribbon->sa[j] - ribbon->ox[j];
            poly->y2 = (ribbon->sa[j] >> 16) - ribbon->oy[j];
            poly->x3 = ribbon->sa[j + 1] - ribbon->ox[j + 1];
            poly->y3 = (ribbon->sa[j + 1] >> 16) - ribbon->oy[j + 1];
            poly->r0 = ribbon->color[0];
            poly->g0 = ribbon->color[1];
            poly->b0 = ribbon->color[2];
            if (ribbon->otz[j] > 0) {
                GsSortPoly(poly, ot, ribbon->otz[j]);
            }
            if (!(j & 1)) {
                poly++;
            } else {
                poly--;
            }
        }
    }
}
