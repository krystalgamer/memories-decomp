#include "../../types.h"
#include "variant473_ribs.h"
#include "variant473_entry.h"

/* Five nine-point ribs. While a rib's size is positive, its spine grows from
 * the origin toward the anchor in the entry ribbon directions (state below 2) or runs from
 * fade/1024 of the anchor to the anchor (state 2 and above), each point bent
 * by a sine of its length; a copy moved along the view by the spread is
 * projected with it and each segment draws two POLY_GT4 halves offset by the
 * projected width. Size grows by 32 per step; reaching 0x400 sets state 2 and
 * moves phase 2 to 3. From state 2 the rib snaps to fade 0x400 and state 3.
 * The ripples (ribbon_sway/field_3C50) advance by 850 and 128 per step, and
 * once the clock passes the fade start width falls linearly from
 * 0x400, moving phase 4 to 5 when it reaches zero.
 *
 * The loop increment's dead angle and the empty loop in the projection keep
 * the retail hoisting of the /5 constant and the retail projection schedule;
 * the per-point locals are scoped to the build loop for the retail slots. */
void FrenchModelVariant473_DrawRibs(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    long flags[5][9];
    EntryState473 *work;
    GsOT *ot;
    s16 j;
    s16 i;
    s32 angle0;
    s32 turn;
    s16 spread;
    EntryConfig473 *timing;
    Variant473Rib *rec;
    s16 prev;
    s16 cur;
    POLY_GT4 *poly;
    s32 ddx;
    s32 ddy;
    s32 k;
    s32 n;

    work = (EntryState473 *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    ratan2(work->view_delta.vy, work->view_delta.vx);
    poly = work->packets.textured;
    if (!(work->clock.frame_count & 1)) {
        spread = work->width / 16;
    } else {
        spread = work->width * 72 / 1024;
    }
    rec = (Variant473Rib *)work->ribbons;
    for (j = 0; j < 5; j++, rec++, angle0 = (j << 11) / 5 + 0x400) {
        if (rec->size > 0) {
            if (rec->state == 0) {
                rec->state = 1;
            }
            if (j == 0) {
                angle0 = 0x400;
            } else if (j % 2 == 1) {
                angle0 = (j << 11) / 5 + 0x400;
            } else {
                angle0 = (j << 11) / 5 + 0xC00;
            }
            for (i = 0; i < 9; i++) {
                s32 dz;
                s32 oy;
                s32 oz;
                s32 ox;
                s32 dx;
                s32 dy;
                s32 len;

                if (rec->state < 2) {
                    ox = 0;
                    oy = 0;
                    oz = 0;
                    dx = work->ribbon_directions[j].vx * rec->size / 1024;
                    dy = work->ribbon_directions[j].vy * rec->size / 1024;
                    dz = work->ribbon_directions[j].vz * rec->size / 1024;
                    len = rec->size * (i << 1) / 8;
                } else {
                    ox = work->ribbon_directions[j].vx * rec->fade / 1024;
                    oy = work->ribbon_directions[j].vy * rec->fade / 1024;
                    oz = work->ribbon_directions[j].vz * rec->fade / 1024;
                    dx = work->ribbon_directions[j].vx - ox;
                    dz = work->ribbon_directions[j].vz - oz;
                    dy = work->ribbon_directions[j].vy - oy;
                    len = (0x800 - rec->fade * 2) * i / 8 + rec->fade * 2;
                }
                n = i;
                /* Per-component stores keep the index copy sequenced. */
                (&rec->a[k = n])->vx = ox + dx * i / 8 - (rcos(angle0) * (rsin(len) * 256 >> 12) >> 12);
                (&rec->a[k])->vy = oy + dy * i / 8 - (rsin(angle0) * (rsin(len) * 256 >> 12) >> 12);
                (&rec->a[k])->vz = oz + dz * i / 8;
                setVector(&rec->b[i], rec->a[i].vx + (rcos(turn) * spread >> 12), rec->a[i].vy,
                          rec->a[i].vz + (rsin(turn) * spread >> 12));
            }
        }
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = work->matrix.t[0];
    m.t[1] = work->matrix.t[1];
    m.t[2] = work->matrix.t[2];
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
    rec = (Variant473Rib *)work->ribbons;
    for (j = 0; j < 5; j++, rec++) {
        if (rec->size > 0) {
            for (i = 0; i < 9; i++) {
                if (i == 8) {
                    rec->otz[i] = RotTransPers4(&rec->a[i - 1], &rec->a[i], &rec->a[i - 1], &rec->a[i],
                                                &rec->sa[i - 1], &rec->sa[i], &rec->sa[i - 1], &rec->sa[i],
                                                &p, &flags[j][i]);
                    RotTransPers(&rec->b[i], &rec->sb[i], &p, &flag);
                    ddx = (s16)rec->sa[i] - (s16)rec->sa[i - 1];
                    /* Both halves pass through s16 locals: gcc 2.7.2 then keeps the flags
                     * row address inside the loop, as the North American build does; gcc
                     * 2.8.1 compiles the PAL function the same either way. */
                    cur = rec->sa[i] >> 16;
                    prev = rec->sa[i - 1] >> 16;
                    ddy = cur - prev;
                    rec->angle[i] = ratan2(ddy, ddx) - 0x400;
                    rec->width[i] = (s16)rec->sb[i] - (s16)rec->sa[i];
                    rec->ox[i] = rcos(rec->angle[i]) * rec->width[i] >> 12;
                    rec->oy[i] = rsin(rec->angle[i]) * rec->width[i] >> 12;
                } else {
                    rec->otz[i] = RotTransPers4(&rec->a[i], &rec->a[i + 1], &rec->a[i], &rec->a[i + 1],
                                                &rec->sa[i], &rec->sa[i + 1], &rec->sa[i], &rec->sa[i + 1],
                                                &p, &flags[j][i]);
                    RotTransPers(&rec->b[i], &rec->sb[i], &p, &flag);
                    do {
                    } while (0);
                    ddx = (s16)rec->sa[i + 1] - (s16)rec->sa[i];
                    ddy = (rec->sa[i + 1] >> 16) - (rec->sa[i] >> 16);
                    rec->angle[i] = ratan2(ddy, ddx) - 0x400;
                    rec->width[i] = (s16)rec->sb[i] - (s16)rec->sa[i];
                    rec->ox[i] = rcos(rec->angle[i]) * rec->width[i] >> 12;
                    rec->oy[i] = rsin(rec->angle[i]) * rec->width[i] >> 12;
                }
            }
        }
    }
    rec = (Variant473Rib *)work->ribbons;
    for (j = 0; j < 5; j++, rec++) {
        if (rec->size > 0) {
            for (i = 0; i < 8; i++) {
                poly->r0 = rec->cb[i][0];
                poly->g0 = rec->cb[i][1];
                poly->b0 = rec->cb[i][2];
                poly->r1 = rec->cb[i + 1][0];
                poly->g1 = rec->cb[i + 1][1];
                poly->b1 = rec->cb[i + 1][2];
                poly->r2 = rec->ca[i][0];
                poly->g2 = rec->ca[i][1];
                poly->b2 = rec->ca[i][2];
                poly->r3 = rec->ca[i + 1][0];
                poly->g3 = rec->ca[i + 1][1];
                poly->b3 = rec->ca[i + 1][2];
                poly->x0 = rec->sa[i] + rec->ox[i];
                poly->y0 = (rec->sa[i] >> 16) + rec->oy[i];
                poly->x1 = rec->sa[i + 1] + rec->ox[i + 1];
                poly->y1 = (rec->sa[i + 1] >> 16) + rec->oy[i + 1];
                poly->x2 = rec->sa[i];
                poly->y2 = rec->sa[i] >> 16;
                poly->x3 = rec->sa[i + 1];
                poly->y3 = rec->sa[i + 1] >> 16;
                if (rec->otz[i] > 0) {
                    GsSortPoly(poly, ot, rec->otz[i]);
                }
                poly->x0 = rec->sa[i] - rec->ox[i];
                poly->y0 = (rec->sa[i] >> 16) - rec->oy[i];
                poly->x1 = rec->sa[i + 1] - rec->ox[i + 1];
                poly->y1 = (rec->sa[i + 1] >> 16) - rec->oy[i + 1];
                poly->x2 = rec->sa[i];
                poly->y2 = rec->sa[i] >> 16;
                poly->x3 = rec->sa[i + 1];
                poly->y3 = rec->sa[i + 1] >> 16;
                if (rec->otz[i] >= 0 && flags[j][i] >= 0) {
                    GsSortPoly(poly, ot, rec->otz[i]);
                }
            }
        }
        if (rec->size < 0x400 && rec->state < 2) {
            rec->size += work->clock.step * 32;
            if (rec->size >= 0x400) {
                rec->size = 0x400;
                rec->state = 2;
                if (work->phase == 2) {
                    work->phase = 3;
                }
            }
        } else if (rec->fade < 0x400 && rec->state == 2) {
            rec->fade += work->clock.step * 32;
            rec->size = 0x400;
            rec->fade = 0x400;
            rec->state = 3;
        }
    }
    work->ribbon_sway += work->clock.step * 850;
    work->field_3C50 += work->clock.step * 128;
    timing = work->config;
    if (timing->fade_begin <= work->clock.frame && work->width > 0) {
        work->width = 0x400 - ((work->clock.frame - timing->fade_begin) << 10) /
                                      (timing->fade_end - timing->fade_begin);
        if (work->width <= 0) {
            work->width = 0;
            if (work->phase == 4) {
                work->phase = 5;
            }
        }
    }
}
