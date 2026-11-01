#include "../../types.h"

#include "variant445us_entry.h"

/* Five thirteen-point ribbons, each spine running from its path origin along
 * its path and a copy moved along the view; both are projected, the projected
 * spine is bent by two travelling waves scaled by the projected width, and the
 * first `count` segments are drawn as flat POLY_FT4 quads, the last one ending
 * on the bare spine point. Each RotTransPers4 flag is kept in a per-ribbon
 * table that also gates sorting. The unused `turn` and first-loop `bend` are
 * template code left dead; they still shape the retail code (the kept sign
 * extensions of `i` and `k`). The two packets in `flat` alternate, so `poly`
 * only moves between flat[0] and flat[1]; `count` never exceeds 12, so
 * `k + 1` stays within the thirteen points. The French header-462 images
 * build it with GCC 2.8.1, 12 bytes longer. */
void ModelVariant445_DrawRibbons(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flag[5][13];
    PSXLONG p;
    PSXLONG flag2;
    EntryState445Us *work;
    GsOT *ot;
    s16 i;
    s32 wave;
    s32 phase;
    s32 sum;
    s32 yaw;
    s16 len;
    EntryRibbon445Us *ribbon;
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

    work = (EntryState445Us *)ctx;
    ot = func_80058F10();
    ribbon = work->ribbons;
    sum = 0;
    yaw = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    ratan2(work->view_delta.vy, work->view_delta.vx);
    for (i = 0, phase = 0; i < 5; i++, ribbon++, phase = i * 6144 / 5) {
        len = 0x30;
        if (i == 0) {
            turn = 0x400;
        } else if (i % 2 == 1) {
            turn = i * 1800 / 5 + 0x400;
        } else {
            turn = 0x400 - i * 1800 / 5;
        }
        for (k = 0; k < 13; k++) {
            if (k == 0 || k == 12) {
                bend = 0;
            } else {
                bend = phase - k * 512;
            }
            setVector(&ribbon->a[k], work->paths[i].vx * k / 12, work->paths[i].vy * k / 12,
                      work->paths[i].vz * k / 12);
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
        for (k = 0, wave = -work->ribbon_wave; k < 13; k++, wave += 0x514) {
            if (k == 12) {
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
            w = ribbon->length * ribbon->width[k] / 1024;
            ribbon->ox[k] = rcos(ribbon->angle[k]) * w >> 12;
            ribbon->oy[k] = rsin(ribbon->angle[k]) * w >> 12;
            if (k == 0 || k == 12) {
                bend = 0;
            } else {
                bend = phase - k * 512 + work->ribbon_phase;
            }
            amp1 = ribbon->width[k] * 128 / 48;
            amp2 = ribbon->width[k];
            ribbon->sa[k] += rcos(ribbon->angle[k]) * ((rsin(bend) * amp1 >> 12) + (rsin(wave) * amp2 >> 12)) >> 12;
            ribbon->sa[k] += (s32)((u32)(rsin(ribbon->angle[k]) * ((rsin(bend) * amp1 >> 12) + (rsin(wave) * amp2 >> 12)) >> 12) << 16);
        }
        if (ribbon->count < 0) {
            n = 0;
        } else {
            n = ribbon->count;
        }
        poly = work->flat;
        for (k = 0; k < n; k++) {
            /* The unsigned compare keeps the retail explicit test against 1. */
            if ((u32)(work->frame_count & 1) == 1) {
                if (!(k & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
            if (k + 1 == n) {
                poly->x0 = ribbon->sa[k] + ribbon->ox[k];
                poly->y0 = (ribbon->sa[k] >> 16) + ribbon->oy[k];
                poly->x1 = ribbon->sa[k + 1];
                poly->y1 = ribbon->sa[k + 1] >> 16;
                poly->x2 = ribbon->sa[k] - ribbon->ox[k];
                poly->y2 = (ribbon->sa[k] >> 16) - ribbon->oy[k];
                poly->x3 = ribbon->sa[k + 1];
                poly->y3 = ribbon->sa[k + 1] >> 16;
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
            poly->r0 = ribbon->inner.r;
            poly->g0 = ribbon->inner.g;
            poly->b0 = ribbon->inner.b;
            if (ribbon->otz[k] >= 0 && flag[i][k] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[k]);
            }
            if (!(work->frame_count & 1)) {
                if (!(k & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
        }
        if (ribbon->count < 12 && ribbon->state == 0) {
            ribbon->count += work->step * 2;
            if (ribbon->count >= 12) {
                ribbon->count = 12;
                ribbon->state = 1;
                if (work->phase == 1) {
                    work->phase = 2;
                }
            }
        } else if (ribbon->state == 1 && ribbon->length > 0) {
            ribbon->length -= work->step << 6;
            if (ribbon->length <= 0) {
                if (work->phase >= 3) {
                    ribbon->length = 0;
                    ribbon->count = 0;
                    ribbon->state = 2;
                } else {
                    ribbon->length = 0x400;
                    ribbon->count = -50;
                    ribbon->state = 0;
                }
            }
        }
        sum += ribbon->state;
        if (sum == 10 && work->phase == 3) {
            work->phase = 4;
        }
    }
    work->ribbon_wave += work->step << 7;
    work->ribbon_phase += work->step << 6;
}
