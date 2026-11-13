#include "../../types.h"

#include "variant469_ribbons.h"
#include "variant469_entry.h"

/* Builds five eleven-point ribbons from the per-ribbon end points and a copy
 * moved along the view; projects both, bends the projected spine by two
 * travelling waves scaled by the projected width, and draws the first `count`
 * segments as flat POLY_FT4 quads. The drawn length grows by two frame steps
 * up to 10, then the view offset shrinks by step * 64 and the ribbon restarts
 * or retires. The `turn` and first-loop `bend` values are template code left
 * unused; they keep the retail sign extensions of `i` and `k`.
 *
 * The North American header-452 copy, built with the CDK compiler, sets the
 * constant ribbon length once before the ribbon loop rather than per ribbon
 * (MODEL_VARIANT469_RIBBONS_EARLY_LEN); that keeps its sign extension inside
 * the ribbon loop and the hoisted projection addresses in retail order. */
void FrenchModelVariant469_DrawRibbons(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    EntryState469 *work;
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

    work = (EntryState469 *)ctx;
    ribbon = (Variant469Ribbon *)work->ribbons;
    ot = Graphics_GetDefaultOrderingTable();
    sum = 0;
#ifdef MODEL_VARIANT469_RIBBONS_EARLY_LEN
    len = 0x30;
#endif
    yaw = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    ratan2(work->view_delta.vy, work->view_delta.vx);
    for (i = 0, phase = 0; i < 5; i++, ribbon++, phase = i * 6144 / 5) {
#ifndef MODEL_VARIANT469_RIBBONS_EARLY_LEN
        len = 0x30;
#endif
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
                      work->paths[i].vx * k / 10,
                      work->paths[i].vy * k / 10,
                      work->paths[i].vz * k / 10);
            setVector(&ribbon->b[k], ribbon->a[k].vx + (rcos(yaw) * len >> 12), ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(yaw) * len >> 12));
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->path_origins[i].vx;
        m.t[1] = work->path_origins[i].vy;
        m.t[2] = work->path_origins[i].vz;
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
        for (k = 0, wave = -work->field_39BC; k < 11; k++, wave += 0x514) {
            if (k == 10) {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k], &ribbon->a[k - 1], &ribbon->a[k],
                                                &ribbon->sa[k - 1], &ribbon->sa[k], &ribbon->sa[k - 1], &ribbon->sa[k],
                                                &p, &flag);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k], &p, &flag);
                dx = (s16)ribbon->sa[k] - (s16)ribbon->sa[k - 1];
                dy = (ribbon->sa[k] >> 16) - (ribbon->sa[k - 1] >> 16);
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                               &ribbon->sa[k], &ribbon->sa[k + 1], &ribbon->sa[k], &ribbon->sa[k + 1],
                                               &p, &flag);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k], &p, &flag);
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
            ribbon->sa[k] = (u32)ribbon->sa[k] + ((u32)(rsin(ribbon->angle[k]) * ((rsin(bend) * amp1 >> 12) + (rsin(wave) * amp2 >> 12)) >> 12) << 16);
        }
        if (ribbon->count < 0) {
            n = 0;
        } else {
            n = ribbon->count;
        }
        poly = work->flat;
        for (k = 0; k < n; k++) {
            if ((u32)(work->clock.frame_count & 1) == 1) {
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
            if (ribbon->otz[k] > 0) {
                GsSortPoly(poly, ot, ribbon->otz[k]);
            }
            if (!(work->clock.frame_count & 1)) {
                if (!(k & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
        }
        if (ribbon->count < 10 && ribbon->state == 0) {
            ribbon->count += work->clock.step * 2;
            if (ribbon->count >= 10) {
                ribbon->count = 10;
                ribbon->state = 1;
                if (work->phase == 2) {
                    work->phase = 3;
                }
            }
        } else if (ribbon->state == 1 && ribbon->len > 0) {
            ribbon->len -= work->clock.step << 6;
            if (ribbon->len <= 0) {
                if (work->phase >= 5) {
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
        if (sum == 10 && work->phase == 5) {
            work->phase = 6;
        }
    }
    work->field_39BC += work->clock.step * 384;
    work->field_39C0 += work->clock.step << 7;
}
