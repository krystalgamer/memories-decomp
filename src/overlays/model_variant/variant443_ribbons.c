#include "../../types.h"

#include "variant443_ribbons.h"

/* Builds one seventeen-point ribbon per entry of the timing record's count,
 * each turned by its own angle (0x400, then 0x400 +- i * 1800 / count by
 * parity) and a copy moved along the view; projects both, bends the projected
 * spine by two travelling waves scaled by the projected width, and draws the
 * first `count` segments as flat POLY_FT4 quads. The drawn length grows by two
 * frame steps up to 16, then the view offset shrinks by step * 64 and the
 * ribbon restarts or retires. The first loop's `bend` is template code left
 * unused; it still shapes the retail code. */
void func_8013BD00(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[8][17];
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
    Variant443Ribbon *ribbon;
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
    ot = func_80058F10();
    sum = 0;
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x2E58), MODEL_VARIANT_WORD(work, 0x2E50)) + 0xC00;
    ratan2(MODEL_VARIANT_WORD(work, 0x2E54), MODEL_VARIANT_WORD(work, 0x2E50));
    ribbon = (Variant443Ribbon *)work;
    for (i = 0, phase = 0; i < *(s32 *)(*(u8 *G32 *)(work + 0x2E7C) + 0x20); i++, ribbon++, phase = i * 0x300) {
        len = 0x30;
        if (i == 0) {
            turn = 0x400;
        } else if (i % 2 == 1) {
            turn = i * 1800 / *(s32 *)(*(u8 *G32 *)(work + 0x2E7C) + 0x20) + 0x400;
        } else {
            turn = 0x400 - i * 1800 / *(s32 *)(*(u8 *G32 *)(work + 0x2E7C) + 0x20);
        }
        for (k = 0; k < 17; k++) {
            if (k == 0 || k == 16) {
                bend = 0;
            } else {
                bend = phase - k * 384 + MODEL_VARIANT_WORD(work, 0x2EC0);
            }
            coil = k * 128;
            setVector(&ribbon->a[k],
                      MODEL_VARIANT_WORD(work + (i << 4), 0x2DBC) * k / 16 - (rcos(turn) * (rsin(coil) * 384 >> 12) >> 12),
                      MODEL_VARIANT_WORD(work + (i << 4), 0x2DC0) * k / 16 - (rsin(turn) * (rsin(coil) * 384 >> 12) >> 12),
                      MODEL_VARIANT_WORD(work + (i << 4), 0x2DC4) * k / 16);
            setVector(&ribbon->b[k], ribbon->a[k].vx + (rcos(yaw) * len >> 12), ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(yaw) * len >> 12));
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work + (i << 5), 0x2C50);
        m.t[1] = MODEL_VARIANT_WORD(work + (i << 5), 0x2C54);
        m.t[2] = MODEL_VARIANT_WORD(work + (i << 5), 0x2C58);
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
        for (k = 0, wave = -MODEL_VARIANT_WORD(work, 0x2EBC); k < 17; k++, wave += 0x514) {
            if (k == 16) {
                ribbon->otz[16] = RotTransPers4(&ribbon->a[15], &ribbon->a[16], &ribbon->a[15], &ribbon->a[16],
                                                &ribbon->sa[15], &ribbon->sa[16], &ribbon->sa[15], &ribbon->sa[16],
                                                &p, &flags[i][16]);
                RotTransPers(&ribbon->b[16], &ribbon->sb[16], &p, &flag);
                dx = (s16)ribbon->sa[16] - (s16)ribbon->sa[15];
                dy = (ribbon->sa[16] >> 16) - (ribbon->sa[15] >> 16);
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                               &ribbon->sa[k], &ribbon->sa[k + 1], &ribbon->sa[k], &ribbon->sa[k + 1],
                                               &p, &flags[i][k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k], &p, &flag);
                dx = (s16)ribbon->sa[k + 1] - (s16)ribbon->sa[k];
                dy = (ribbon->sa[k + 1] >> 16) - (ribbon->sa[k] >> 16);
            }
            ribbon->angle[k] = ratan2(dy, dx) - 0x400;
            ribbon->width[k] = (s16)ribbon->sb[k] - (s16)ribbon->sa[k];
            w = ribbon->len * ribbon->width[k] / 1024;
            ribbon->ox[k] = rcos(ribbon->angle[k]) * w >> 12;
            ribbon->oy[k] = rsin(ribbon->angle[k]) * w >> 12;
            if (k == 0 || k == 16) {
                bend = 0;
            } else {
                bend = phase - k * 384 + MODEL_VARIANT_WORD(work, 0x2EC0);
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
        poly = (POLY_FT4 *)(work + 0x2BB4);
        for (k = 0; k < n; k++) {
            if ((u32)(MODEL_VARIANT_WORD(work, 0x2E68) & 1) == 1) {
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
            if (ribbon->otz[k] >= 0 && flags[i][k] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[k]);
            }
            if (!(MODEL_VARIANT_WORD(work, 0x2E68) & 1)) {
                if (!(k & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
        }
        if (ribbon->count < 16 && ribbon->state == 0) {
            ribbon->count += MODEL_VARIANT_WORD(work, 0x2E74) * 2;
            if (ribbon->count >= 16) {
                ribbon->count = 16;
                ribbon->state = 1;
                if (MODEL_VARIANT_WORD(work, 0x2EC4) == 1) {
                    MODEL_VARIANT_WORD(work, 0x2EC4) = 2;
                }
            }
        } else if (ribbon->state == 1 && ribbon->len > 0) {
            ribbon->len -= MODEL_VARIANT_WORD(work, 0x2E74) << 6;
            if (ribbon->len <= 0) {
                if (MODEL_VARIANT_WORD(work, 0x2EC4) >= 3) {
                    ribbon->len = 0;
                    ribbon->count = 0;
                    ribbon->state = 2;
                } else {
                    ribbon->len = 0x400;
                    ribbon->count = -(*(s32 *)(*(u8 *G32 *)(work + 0x2E7C) + 0x20) * 8);
                    ribbon->state = 0;
                }
            }
        }
        sum += ribbon->state;
        if (sum == *(s32 *)(*(u8 *G32 *)(work + 0x2E7C) + 0x20) * 2 && MODEL_VARIANT_WORD(work, 0x2EC4) == 4) {
            MODEL_VARIANT_WORD(work, 0x2EC4) = 5;
        }
    }
    MODEL_VARIANT_WORD(work, 0x2EBC) += MODEL_VARIANT_WORD(work, 0x2E74) * 650;
    MODEL_VARIANT_WORD(work, 0x2EC0) += MODEL_VARIANT_WORD(work, 0x2E74) << 7;
}
