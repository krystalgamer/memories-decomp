#include "../../types.h"

#include "variant458_ribbons.h"

/* Builds one thirteen-point ribbon per entry of the timing record's count,
 * each turned by its own angle (0x400, then 0x400 +- i * 1800 / count by
 * parity) and a copy moved along the view; projects both, bends the projected
 * spine by two travelling waves scaled by the projected width, and draws the
 * first `count` segments as flat POLY_FT4 quads. The drawn length grows by two
 * frame steps up to 12, then the view offset shrinks by step * 128 and the
 * ribbon restarts or retires. The first loop's `bend` is template code left
 * unused, as in the header-443 ribbons. */
void func_8013BE94(u8 *ctx)
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
    s32 wave;
    s32 phase;
    s32 sum;
    s32 turn;
    s32 yaw;
    s16 len;
    Variant458Ribbon *ribbon;
    POLY_FT4 *poly;
    s16 k;
    s32 bend;
    s32 coil;
    s32 n;
    s32 w;
    s32 amp1;
    s32 amp2;
    s32 dx;
    s32 dy;

    work = ctx;
    ribbon = (Variant458Ribbon *)(work + 0x1E4);
    ot = func_80058F10();
    sum = 0;
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x2FA8), MODEL_VARIANT_WORD(work, 0x2FA0)) + 0xC00;
    ratan2(MODEL_VARIANT_WORD(work, 0x2FA4), MODEL_VARIANT_WORD(work, 0x2FA0));
    for (i = 0, phase = 0; i < *(s32 *)(*(u8 *G32 *)(work + 0x2FCC) + 0x14); i++, ribbon++, phase = i * 0x300) {
        len = 0x30;
        if (i == 0) {
            turn = 0x400;
        } else if (i % 2 == 1) {
            turn = i * 1800 / *(s32 *)(*(u8 *G32 *)(work + 0x2FCC) + 0x14) + 0x400;
        } else {
            turn = 0x400 - i * 1800 / *(s32 *)(*(u8 *G32 *)(work + 0x2FCC) + 0x14);
        }
        for (k = 0; k < 13; k++) {
            if (k == 0 || k == 12) {
                bend = 0;
            } else {
                bend = phase - k * 512 + MODEL_VARIANT_WORD(work, 0x301C);
            }
            coil = k * 2048 / 12;
            setVector(&ribbon->a[k],
                      MODEL_VARIANT_WORD(work + (i << 4), 0x2F0C) * k / 12 - (rcos(turn) * (rsin(coil) * 384 >> 12) >> 12),
                      MODEL_VARIANT_WORD(work + (i << 4), 0x2F10) * k / 12 - (rsin(turn) * (rsin(coil) * 384 >> 12) >> 12),
                      MODEL_VARIANT_WORD(work + (i << 4), 0x2F14) * k / 12);
            setVector(&ribbon->b[k], ribbon->a[k].vx + (rcos(yaw) * len >> 12), ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(yaw) * len >> 12));
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work + (i << 4), 0x2E0C);
        m.t[1] = MODEL_VARIANT_WORD(work + (i << 4), 0x2E10);
        m.t[2] = MODEL_VARIANT_WORD(work + (i << 4), 0x2E14);
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
        for (k = 0, wave = -MODEL_VARIANT_WORD(work, 0x3018); k < 13; k++, wave += 0x514) {
            if (k == 12) {
                ribbon->otz[12] = RotTransPers4(&ribbon->a[11], &ribbon->a[12], &ribbon->a[11], &ribbon->a[12],
                                                &ribbon->sa[11], &ribbon->sa[12], &ribbon->sa[11], &ribbon->sa[12],
                                                &p, &ribbon->flag[12]);
                RotTransPers(&ribbon->b[12], &ribbon->sb[12], &p, &flag);
                dx = (s16)ribbon->sa[12] - (s16)ribbon->sa[11];
                dy = (ribbon->sa[12] >> 16) - (ribbon->sa[11] >> 16);
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                               &ribbon->sa[k], &ribbon->sa[k + 1], &ribbon->sa[k], &ribbon->sa[k + 1],
                                               &p, &ribbon->flag[k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k], &p, &flag);
                dx = (s16)ribbon->sa[k + 1] - (s16)ribbon->sa[k];
                dy = (ribbon->sa[k + 1] >> 16) - (ribbon->sa[k] >> 16);
            }
            ribbon->angle[k] = ratan2(dy, dx) - 0x400;
            ribbon->width[k] = (s16)ribbon->sb[k] - (s16)ribbon->sa[k];
            w = ribbon->len * ribbon->width[k] / 1024;
            ribbon->ox[k] = rcos(ribbon->angle[k]) * w >> 12;
            ribbon->oy[k] = rsin(ribbon->angle[k]) * w >> 12;
            if (k == 0 || k == 12) {
                bend = 0;
            } else {
                bend = phase - k * 512 + MODEL_VARIANT_WORD(work, 0x301C);
            }
            amp1 = ribbon->width[k] * 64 / 48;
            amp2 = ribbon->width[k] * 32 / 48;
            ribbon->sa[k] += rcos(ribbon->angle[k]) * ((rsin(bend) * amp1 >> 12) + (rsin(wave) * amp2 >> 12)) >> 12;
            ribbon->sa[k] += (rsin(ribbon->angle[k]) * ((rsin(bend) * amp1 >> 12) + (rsin(wave) * amp2 >> 12)) >> 12) << 16;
        }
        if (ribbon->count < 0) {
            n = 0;
        } else {
            n = ribbon->count;
        }
        poly = (POLY_FT4 *)(work + 0x2D20);
        for (k = 0; k < n; k++) {
            if ((u32)(MODEL_VARIANT_WORD(work, 0x2FB8) & 1) == 1) {
                if (!(k & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
            poly->x0 = ribbon->sa[k] + ribbon->ox[k];
            poly->y0 = (ribbon->sa[k] >> 16) + ribbon->oy[k];
            poly->x1 = ribbon->sa[k + 1] + ribbon->ox[k + 1];
            poly->y1 = (ribbon->sa[k + 1] >> 16) + ribbon->oy[k + 1];
            poly->x2 = ribbon->sa[k] - ribbon->ox[k];
            poly->y2 = (ribbon->sa[k] >> 16) - ribbon->oy[k];
            poly->x3 = ribbon->sa[k + 1] - ribbon->ox[k + 1];
            poly->y3 = (ribbon->sa[k + 1] >> 16) - ribbon->oy[k + 1];
            poly->r0 = ribbon->color[0];
            poly->g0 = ribbon->color[1];
            poly->b0 = ribbon->color[2];
            if (ribbon->otz[k] >= 0 && ribbon->flag[k] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[k]);
            }
            if (!(MODEL_VARIANT_WORD(work, 0x2FB8) & 1)) {
                if (!(k & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
        }
        if (ribbon->count < 12 && ribbon->state == 0) {
            ribbon->count += MODEL_VARIANT_WORD(work, 0x2FC4) * 2;
            if (ribbon->count >= 12) {
                ribbon->count = 12;
                ribbon->state = 1;
                if (MODEL_VARIANT_WORD(work, 0x3020) == 2) {
                    MODEL_VARIANT_WORD(work, 0x3020) = 3;
                }
            }
        } else if (ribbon->state == 1 && ribbon->len > 0) {
            ribbon->len -= MODEL_VARIANT_WORD(work, 0x2FC4) << 7;
            if (ribbon->len <= 0) {
                if (MODEL_VARIANT_WORD(work, 0x3020) >= 4) {
                    ribbon->len = 0;
                    ribbon->count = 0;
                    ribbon->state = 2;
                } else {
                    ribbon->len = 0x400;
                    ribbon->count = -(*(s32 *)(*(u8 *G32 *)(work + 0x2FCC) + 0x14) * 8);
                    ribbon->state = 0;
                }
            }
        }
        sum += ribbon->state;
        if (sum == *(s32 *)(*(u8 *G32 *)(work + 0x2FCC) + 0x14) * 2 && MODEL_VARIANT_WORD(work, 0x3020) == 5) {
            MODEL_VARIANT_WORD(work, 0x3020) = 6;
        }
    }
    MODEL_VARIANT_WORD(work, 0x3018) += MODEL_VARIANT_WORD(work, 0x2FC4) * 650;
    MODEL_VARIANT_WORD(work, 0x301C) += MODEL_VARIANT_WORD(work, 0x2FC4) << 7;
}
