#include "../../types.h"
#include "variant473_ribs.h"

#define VARIANT473_UWORD(p, o) (*(u32 *)((u8 *)(p) + (o)))

/* Five nine-point ribs. While a rib's size is positive, its spine grows from
 * the origin toward the anchor at work + 0x3B70 (state below 2) or runs from
 * fade/1024 of the anchor to the anchor (state 2 and above), each point bent
 * by a sine of its length; a copy moved along the view by the spread is
 * projected with it and each segment draws two POLY_GT4 halves offset by the
 * projected width. Size grows by 32 per step; reaching 0x400 sets state 2 and
 * moves phase 2 to 3. From state 2 the rib snaps to fade 0x400 and state 3.
 * The ripples at work + 0x3C4C/0x3C50 advance by 850 and 128 per step, and
 * once the clock passes the fade start work + 0x3C00 falls linearly from
 * 0x400, moving phase 4 to 5 when it reaches zero.
 *
 * The loop increment's dead angle and the empty loop in the projection keep
 * the retail hoisting of the /5 constant and the retail projection schedule;
 * the per-point locals are scoped to the build loop for the retail slots. */
void func_8013CFB0(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    PSXLONG flags[5][9];
    u8 *work;
    GsOT *ot;
    s16 j;
    s16 i;
    s32 angle0;
    s32 turn;
    s16 spread;
    u8 *pos;
    u8 *timing;
    Variant473Rib *rec;
    POLY_GT4 *poly;
    s32 ddx;
    s32 ddy;
    s32 k;
    s32 n;

    work = ctx;
    ot = func_80058F10();
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x3BCC), MODEL_VARIANT_WORD(work, 0x3BC4)) + 0xC00;
    ratan2(MODEL_VARIANT_WORD(work, 0x3BC8), MODEL_VARIANT_WORD(work, 0x3BC4));
    poly = (POLY_GT4 *)(work + 0x39D0);
    if (!(MODEL_VARIANT_WORD(work, 0x3BDC) & 1)) {
        spread = MODEL_VARIANT_WORD(work, 0x3C00) / 16;
    } else {
        spread = MODEL_VARIANT_WORD(work, 0x3C00) * 72 / 1024;
    }
    rec = (Variant473Rib *)(work + 0x18D8);
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
            pos = work + j * 16;
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
                    dx = MODEL_VARIANT_WORD(pos, 0x3B70) * rec->size / 1024;
                    dy = MODEL_VARIANT_WORD(pos, 0x3B74) * rec->size / 1024;
                    dz = MODEL_VARIANT_WORD(pos, 0x3B78) * rec->size / 1024;
                    len = rec->size * (i << 1) / 8;
                } else {
                    ox = MODEL_VARIANT_WORD(pos, 0x3B70) * rec->fade / 1024;
                    oy = MODEL_VARIANT_WORD(pos, 0x3B74) * rec->fade / 1024;
                    oz = MODEL_VARIANT_WORD(pos, 0x3B78) * rec->fade / 1024;
                    dx = MODEL_VARIANT_WORD(pos, 0x3B70) - ox;
                    dz = MODEL_VARIANT_WORD(pos, 0x3B78) - oz;
                    dy = MODEL_VARIANT_WORD(pos, 0x3B74) - oy;
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
    m.t[0] = MODEL_VARIANT_WORD(work, 0x3AFC);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x3B00);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x3B04);
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
    rec = (Variant473Rib *)(work + 0x18D8);
    for (j = 0; j < 5; j++, rec++) {
        if (rec->size > 0) {
            for (i = 0; i < 9; i++) {
                if (i == 8) {
                    rec->otz[i] = RotTransPers4(&rec->a[i - 1], &rec->a[i], &rec->a[i - 1], &rec->a[i],
                                                &rec->sa[i - 1], &rec->sa[i], &rec->sa[i - 1], &rec->sa[i],
                                                &p, &flags[j][i]);
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
    rec = (Variant473Rib *)(work + 0x18D8);
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
            rec->size += MODEL_VARIANT_WORD(work, 0x3BE8) * 32;
            if (rec->size >= 0x400) {
                rec->size = 0x400;
                rec->state = 2;
                if (MODEL_VARIANT_WORD(work, 0x3C5C) == 2) {
                    MODEL_VARIANT_WORD(work, 0x3C5C) = 3;
                }
            }
        } else if (rec->fade < 0x400 && rec->state == 2) {
            rec->fade += MODEL_VARIANT_WORD(work, 0x3BE8) * 32;
            rec->size = 0x400;
            rec->fade = 0x400;
            rec->state = 3;
        }
    }
    MODEL_VARIANT_WORD(work, 0x3C4C) += MODEL_VARIANT_WORD(work, 0x3BE8) * 850;
    MODEL_VARIANT_WORD(work, 0x3C50) += MODEL_VARIANT_WORD(work, 0x3BE8) * 128;
    timing = *(u8 **)(work + 0x3BF0);
    if (VARIANT473_UWORD(timing, 0x24) <= VARIANT473_UWORD(work, 0x3BE0) && MODEL_VARIANT_WORD(work, 0x3C00) > 0) {
        MODEL_VARIANT_WORD(work, 0x3C00) = 0x400 - ((VARIANT473_UWORD(work, 0x3BE0) - VARIANT473_UWORD(timing, 0x24)) << 10) /
                                      (VARIANT473_UWORD(timing, 0x28) - VARIANT473_UWORD(timing, 0x24));
        if (MODEL_VARIANT_WORD(work, 0x3C00) <= 0) {
            MODEL_VARIANT_WORD(work, 0x3C00) = 0;
            if (MODEL_VARIANT_WORD(work, 0x3C5C) == 4) {
                MODEL_VARIANT_WORD(work, 0x3C5C) = 5;
            }
        }
    }
}
