#include "../../types.h"

#include "variant479_ribs.h"
#include "variant479_entry.h"

/* Five nine-point ribs. While a rib's size is positive, its spine grows from
 * the origin toward the anchor in the entry deltas (state below 2) or runs from
 * fade/1024 of the anchor to the anchor (state 2 and above), each point bent
 * by a sine of its length; a copy moved along the view by the spread is
 * projected with it and each segment draws two POLY_GT4 halves offset by the
 * projected width. Size grows by 32 per step; reaching 0x400 sets state 2 and
 * moves phase 2 to 3. From state 2 the rib snaps to fade 0x400 and state 3.
 * The ripples (field_41DC/field_41E0) advance by 850 and 128 per step, and
 * once the clock passes the fade start rib_fade falls linearly from
 * 0x400, moving phase 4 to 5 when it reaches zero. */
void SpanishModelVariant479_DrawRibs(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant479EntryState *work;
    GsOT *ot;
    s16 j;
    s16 i;
    s32 angle0;
    s32 turn;
/* The North American header-462 build declares dy first, which gives the
 * per-point coordinates the retail spill-slot order under CDK GCC. */
#ifdef MODEL_VARIANT462_RIBS_DY_FIRST
    s32 dy;
#endif
    s32 dz;
    s32 oy;
    s32 oz;
    s16 spread;
    s32 ox;
    s32 dx;
#ifndef MODEL_VARIANT462_RIBS_DY_FIRST
    s32 dy;
#endif
    s32 len;
    Variant479EntryConfig *timing;
    Variant479Rib *rec;
    POLY_GT4 *poly;
    s32 ddx;
    s32 ddy;
    s32 k;
    s32 n;

    work = (Variant479EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    ratan2(work->view_delta.vy, work->view_delta.vx);
    poly = work->packets.textured;
    if (!(work->clock.frame_count & 1)) {
        spread = work->rib_fade / 16;
    } else {
        spread = work->rib_fade * 72 / 1024;
    }
    rec = work->ribs;
    for (j = 0; j < 5; j++, rec++) {
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
                if (rec->state < 2) {
                    ox = 0;
                    oy = 0;
                    oz = 0;
                    dx = work->deltas[j].vx * rec->size / 1024;
                    dy = work->deltas[j].vy * rec->size / 1024;
                    dz = work->deltas[j].vz * rec->size / 1024;
                    len = rec->size * (i << 1) / 8;
                } else {
                    ox = work->deltas[j].vx * rec->fade / 1024;
                    oy = work->deltas[j].vy * rec->fade / 1024;
                    oz = work->deltas[j].vz * rec->fade / 1024;
                    dx = work->deltas[j].vx - ox;
                    dz = work->deltas[j].vz - oz;
                    dy = work->deltas[j].vy - oy;
                    len = (0x800 - rec->fade * 2) * i / 8 + rec->fade * 2;
                }
                n = i;
                setVector(&rec->a[k = n], ox + dx * i / 8 - (rcos(angle0) * (rsin(len) * 256 >> 12) >> 12),
                          oy + dy * i / 8 - (rsin(angle0) * (rsin(len) * 256 >> 12) >> 12),
                          oz + dz * k / 8);
                setVector(&rec->b[i], rec->a[i].vx + (rcos(turn) * spread >> 12), rec->a[i].vy,
                          rec->a[i].vz + (rsin(turn) * spread >> 12));
            }
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->matrices[j % 3].t[0];
        m.t[1] = work->matrices[j % 3].t[1];
        m.t[2] = work->matrices[j % 3].t[2];
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
        if (rec->size > 0) {
            for (i = 0; i < 9; i++) {
                if (i == 8) {
                    rec->otz[i] = RotTransPers4(&rec->a[i - 1], &rec->a[i], &rec->a[i - 1], &rec->a[i],
                                                &rec->sa[i - 1], &rec->sa[i], &rec->sa[i - 1], &rec->sa[i],
                                                &p, &rec->flag[i]);
                    RotTransPers(&rec->b[i], &rec->sb[i], &p, &flag);
                    ddx = (s16)rec->sa[i] - (s16)rec->sa[i - 1];
                    ddy = (rec->sa[i] >> 16) - (rec->sa[i - 1] >> 16);
                    rec->angle[i] = ratan2(ddy, ddx) - 0x400;
                    rec->width[i] = (s16)rec->sb[i] - (s16)rec->sa[i];
                    rec->ox[i] = rcos(rec->angle[i]) * rec->width[i] >> 12;
                    rec->oy[i] = rsin(rec->angle[i]) * rec->width[i] >> 12;
                } else {
                    rec->otz[i] = RotTransPers4(&rec->a[i], &rec->a[i + 1], &rec->a[i], &rec->a[i + 1],
                                                &rec->sa[i], &rec->sa[i + 1], &rec->sa[i], &rec->sa[i + 1],
                                                &p, &rec->flag[i]);
                    RotTransPers(&rec->b[i], &rec->sb[i], &p, &flag);
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
    rec = work->ribs;
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
                if (rec->otz[i] >= 0 && rec->flag[i] >= 0) {
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
                if (rec->otz[i] >= 0 && rec->flag[i] >= 0) {
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
    work->field_41DC += work->clock.step * 850;
    work->field_41E0 += work->clock.step * 128;
    timing = work->config;
    if (timing->fade_start <= work->clock.frame && work->rib_fade > 0) {
        work->rib_fade = 0x400 - ((work->clock.frame - timing->fade_start) << 10) /
                                      (timing->fade_end - timing->fade_start);
        if (work->rib_fade <= 0) {
            work->rib_fade = 0;
            if (work->phase == 4) {
                work->phase = 5;
            }
        }
    }
}
