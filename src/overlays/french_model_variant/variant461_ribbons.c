#include "../../types.h"

#include "variant469_ribbons.h"

/* The header-469 five-ribbon body on ribbons at the start of the work area,
 * with each RotTransPers4 flag kept in a local per-ribbon table that also gates
 * sorting. The last drawn segment ends on the bare projected spine point
 * instead of its width offsets. The unused `turn` and first-loop `bend` are
 * kept from the template, as in the header-469 helper. */
void func_8013C508(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag[5][11];
    PSXLONG flag2;
    u8 *work;
    GsOT *ot;
    s16 i;
    s32 wave;
    s32 phase;
    s32 sum;
    s32 yaw;
    s16 len;
    Variant469Ribbon *ribbon;
    POLY_FT4 *poly;
    s16 k;
    s32 bend;
    s32 n;
    s32 w;
    s32 amp1;
    s32 amp2;
    s32 dx;
    s32 dy;
    s32 turn;

    work = ctx;
    ot = func_80058F10();
    ribbon = (Variant469Ribbon *)work;
    sum = 0;
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x3348), MODEL_VARIANT_WORD(work, 0x3340)) + 0xC00;
    ratan2(MODEL_VARIANT_WORD(work, 0x3344), MODEL_VARIANT_WORD(work, 0x3340));
    for (i = 0, phase = 0; i < 5; i++, ribbon++, phase = i * 6144 / 5) {
        len = 0x30;
        if (i == 0) {
            turn = 0x400;
        } else if (i % 2 == 1) {
            turn = i * 1800 / 5 + 0x400;
        } else {
            turn = 0x400 - i * 1800 / 5;
        }
        for (k = 0; k < 11; k++) {
            if (k == 0 || k == 10) {
                bend = 0;
            } else {
                bend = phase - k * 512;
            }
            setVector(&ribbon->a[k],
                      MODEL_VARIANT_WORD(work + (i << 4), 0x2FDC) * k / 10,
                      MODEL_VARIANT_WORD(work + (i << 4), 0x2FE0) * k / 10,
                      MODEL_VARIANT_WORD(work + (i << 4), 0x2FE4) * k / 10);
            setVector(&ribbon->b[k], ribbon->a[k].vx + (rcos(yaw) * len >> 12), ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(yaw) * len >> 12));
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F3C);
        m.t[1] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F40);
        m.t[2] = MODEL_VARIANT_WORD(work + (i << 4), 0x2F44);
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
        for (k = 0, wave = -MODEL_VARIANT_WORD(work, 0x3390); k < 11; k++, wave += 0x514) {
            if (k == 10) {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k], &ribbon->a[k - 1], &ribbon->a[k],
                                                &ribbon->sa[k - 1], &ribbon->sa[k], &ribbon->sa[k - 1], &ribbon->sa[k],
                                                &p, &flag[i][k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k], &p, &flag2);
                dx = (s16)ribbon->sa[k] - (s16)ribbon->sa[k - 1];
                dy = (ribbon->sa[k] >> 16) - (ribbon->sa[k - 1] >> 16);
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                               &ribbon->sa[k], &ribbon->sa[k + 1], &ribbon->sa[k], &ribbon->sa[k + 1],
                                               &p, &flag[i][k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k], &p, &flag2);
                dx = (s16)ribbon->sa[k + 1] - (s16)ribbon->sa[k];
                dy = (ribbon->sa[k + 1] >> 16) - (ribbon->sa[k] >> 16);
            }
            ribbon->angle[k] = ratan2(dy, dx) - 0x400;
            ribbon->width[k] = (s16)ribbon->sb[k] - (s16)ribbon->sa[k];
            w = ribbon->len * ribbon->width[k] / 1024;
            ribbon->ox[k] = rcos(ribbon->angle[k]) * w >> 12;
            ribbon->oy[k] = rsin(ribbon->angle[k]) * w >> 12;
            if (k == 0 || k == 10) {
                bend = 0;
            } else {
                bend = phase - k * 6144 / 10;
            }
            amp1 = ribbon->width[k] * 64 / 48;
            amp2 = ribbon->width[k];
            ribbon->sa[k] += rcos(ribbon->angle[k]) * ((rsin(bend) * amp1 >> 12) + (rsin(wave) * amp2 >> 12)) >> 12;
            ribbon->sa[k] += (rsin(ribbon->angle[k]) * ((rsin(bend) * amp1 >> 12) + (rsin(wave) * amp2 >> 12)) >> 12) << 16;
        }
        if (ribbon->count < 0) {
            n = 0;
        } else {
            n = ribbon->count;
        }
        poly = (POLY_FT4 *)(work + 0x2E68);
        for (k = 0; k < n; k++) {
            if ((u32)(MODEL_VARIANT_WORD(work, 0x3358) & 1) == 1) {
                if (!(k & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
            if (k + 1 == n) {
                poly->x0 = ribbon->sa[k] + ribbon->ox[k];
                poly->y0 = (ribbon->sa[k] >> 16) + ribbon->oy[k];
                poly->x1 = ribbon->sa[n];
                poly->y1 = ribbon->sa[n] >> 16;
                poly->x2 = ribbon->sa[k] - ribbon->ox[k];
                poly->y2 = (ribbon->sa[k] >> 16) - ribbon->oy[k];
                poly->x3 = ribbon->sa[n];
                poly->y3 = ribbon->sa[n] >> 16;
            } else {
                poly->x0 = ribbon->sa[k] + ribbon->ox[k];
                poly->y0 = (ribbon->sa[k] >> 16) + ribbon->oy[k];
                poly->x1 = ribbon->sa[k + 1] + ribbon->ox[k + 1];
                poly->y1 = (ribbon->sa[k + 1] >> 16) + ribbon->oy[k + 1];
                poly->x2 = ribbon->sa[k] - ribbon->ox[k];
                poly->y2 = (ribbon->sa[k] >> 16) - ribbon->oy[k];
                poly->x3 = ribbon->sa[k + 1] - ribbon->ox[k + 1];
                poly->y3 = (ribbon->sa[k + 1] >> 16) - ribbon->oy[k + 1];
            }
            poly->r0 = ribbon->color[0];
            poly->g0 = ribbon->color[1];
            poly->b0 = ribbon->color[2];
            if (ribbon->otz[k] >= 0 && flag[i][k] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[k]);
            }
            if (!(MODEL_VARIANT_WORD(work, 0x3358) & 1)) {
                if (!(k & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
        }
        if (ribbon->count < 10 && ribbon->state == 0) {
            ribbon->count += MODEL_VARIANT_WORD(work, 0x3364) * 2;
            if (ribbon->count >= 10) {
                ribbon->count = 10;
                ribbon->state = 1;
                if (MODEL_VARIANT_WORD(work, 0x33AC) == 2) {
                    MODEL_VARIANT_WORD(work, 0x33AC) = 3;
                }
            }
        } else if (ribbon->state == 1 && ribbon->len > 0) {
            ribbon->len -= MODEL_VARIANT_WORD(work, 0x3364) << 6;
            if (ribbon->len <= 0) {
                if (MODEL_VARIANT_WORD(work, 0x33AC) >= 5) {
                    ribbon->len = 0;
                    ribbon->count = 0;
                    ribbon->state = 2;
                } else {
                    ribbon->len = 0x400;
                    ribbon->count = -50;
                    ribbon->state = 0;
                }
            }
        }
        sum += ribbon->state;
        if (sum == 10 && MODEL_VARIANT_WORD(work, 0x33AC) == 5) {
            MODEL_VARIANT_WORD(work, 0x33AC) = 6;
        }
    }
    MODEL_VARIANT_WORD(work, 0x3390) += MODEL_VARIANT_WORD(work, 0x3364) * 384;
    MODEL_VARIANT_WORD(work, 0x3394) += MODEL_VARIANT_WORD(work, 0x3364) << 7;
}
