#include "../../types.h"
#include "../model_variant/variant448_fan.h"

/* French header-465 fans: the GCC 2.8.1 build of header 448's fan body
 * (see `variant448_fan.c`) over the same three 0x208-byte units at
 * `work + 0xBD8`. The six colour channels are separate 16-byte stack tables
 * indexed by `j + i * 2 + t * 6`. The flag test at the start keeps an arm
 * whose copy of `work` only the combiner reduces to a no-op; routing it
 * through `ot` rather than `poly` keeps `poly` below the first arm loop's
 * `t` in global-allocation priority, as the retail registers require. */

void func_8013C66C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG status[3][2][3];
    u8 c0[16];
    u8 c1[16];
    u8 c2[16];
    u8 c3[16];
    u8 c4[16];
    u8 c5[16];
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    GsOT *ot;
    POLY_GT4 *poly;
    Variant448FanUnit *unit;
    s16 i;
    s32 angle;
    s16 t;
    s16 k;
    s16 j;
    s32 i2;
    s32 otz;
    s32 dx, dy;
    s32 v;
    s32 tangle;
    s32 iangle;
    s32 count;
    s16 length;
    s32 amt;
    s32 f;
    s32 turn;
    s32 st;

    work = ctx;
    ot = func_80058F10();
    count = 0;
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x1AEC), MODEL_VARIANT_WORD(work, 0x1AE4)) + 0xC00;
    ratan2(MODEL_VARIANT_HALF(work, 0x1AE2), MODEL_VARIANT_HALF(work, 0x1AE0));
    poly = (POLY_GT4 *)(work + 0x1988);
    if ((MODEL_VARIANT_WORD(work, 0x1AFC) & 1) == 0) {
        i = 0;
    } else {
        s32 s = work - (u8 *)ot;
        work = (u8 *)ot + s;
    }
    unit = (Variant448FanUnit *)(work + 0xBD8);
    i = 0;
    iangle = 0;
    length = 0x20;
    do {
        for (t = 0, tangle = 0; t < 2; t++, tangle += 0x800) {
            for (k = 0, angle = tangle + iangle; k < 3; k++, angle = tangle + iangle + (k << 12) / 3) {
                for (j = 0; j < 2; j++) {
                    amt = 0;
                    if (unit->amount[t][j] >= 0) {
                        amt = unit->amount[t][j];
                        if (amt > 0x400) {
                            amt = 0x400;
                        }
                    }
                    setVector(&unit->a[t][k][j], (rcos(angle) * 3 << 6) * (t + 1) >> 12, -amt, (rsin(angle) * 3 << 6) * (t + 1) >> 12);
                    setVector(&unit->b[t][k][j], unit->a[t][k][j].vx + (rcos(turn) * length >> 12), unit->a[t][k][j].vy,
                              unit->a[t][k][j].vz + (rsin(turn) * length >> 12));
                }
            }
        }
        i2 = i * 2;
        for (t = 0; t < 2; t++) {
            for (j = 0; j < 2; j++) {
                v = unit->amount[t][j];
                if (v < 0x200) {
                    c0[j + i2 + t * 6] = unit->cb[j][0];
                    c1[j + i2 + t * 6] = unit->cb[j][1];
                    c2[j + i2 + t * 6] = unit->cb[j][2];
                    c3[j + i2 + t * 6] = unit->ca[j][0];
                    c4[j + i2 + t * 6] = unit->ca[j][1];
                    c5[j + i2 + t * 6] = unit->ca[j][2];
                } else {
                    f = 0;
                    if (v < 0x400) {
                        f = 0x400 - v;
                    }
                    c0[j + i2 + t * 6] = unit->cb[j][0] * f / 512;
                    c1[j + i2 + t * 6] = unit->cb[j][1] * f / 512;
                    c2[j + i2 + t * 6] = unit->cb[j][2] * f / 512;
                    c3[j + i2 + t * 6] = unit->ca[j][0] * f / 512;
                    c4[j + i2 + t * 6] = unit->ca[j][1] * f / 512;
                    c5[j + i2 + t * 6] = unit->ca[j][2] * f / 512;
                }
                if (MODEL_VARIANT_WORD(work, 0x1B3C) < 4) {
                    if (unit->amount[t][1] < 0x400) {
                        unit->amount[t][j] += MODEL_VARIANT_WORD(work, 0x1B08) << 5;
                        if (unit->amount[t][1] >= 0x400) {
                            if (*(u32 *)(MODEL_VARIANT_WORD(work, 0x1B10) + 0x24) <= (u32)MODEL_VARIANT_WORD(work, 0x1B00)) {
                                for (j = 0; j < 2; j++) {
                                    unit->amount[t][j] = 0x400;
                                }
                                st = 1;
                                unit->stage[t] = st;
                            } else {
                                for (j = 0; j < 2; j++) {
                                    unit->amount[t][j] -= 0x600;
                                }
                            }
                        }
                    }
                } else {
                    if (unit->amount[t][1] > 0) {
                        unit->amount[t][j] -= MODEL_VARIANT_WORD(work, 0x1B08) << 6;
                        if (unit->amount[0][0] <= 0 && MODEL_VARIANT_WORD(work, 0x1B3C) == 4) {
                            MODEL_VARIANT_WORD(work, 0x1B3C) = 5;
                        }
                        if (unit->amount[t][0] <= 0) {
                            st = 1;
                            unit->stage[t] = st;
                        }
                        if (unit->amount[t][1] <= 0) {
                            for (j = 0; j < 2; j++) {
                                unit->amount[t][j] = 0;
                            }
                            unit->stage[t] = 2;
                        }
                    }
                }
            }
            count += unit->stage[t];
            if (count >= 6 && MODEL_VARIANT_WORD(work, 0x1B3C) == 1) {
                MODEL_VARIANT_WORD(work, 0x1B3C) = 2;
            }
            if (count >= 12 && MODEL_VARIANT_WORD(work, 0x1B3C) == 5) {
                MODEL_VARIANT_WORD(work, 0x1B3C) = 6;
            }
        }
        i++;
        unit++;
        iangle = (i << 10) / 3;
    } while (i < 3);
    i = 0;
    setVector(&rot, 0, 0, 0);
    unit = (Variant448FanUnit *)(work + 0xBD8);
    m.t[0] = MODEL_VARIANT_WORD(work, 0x1AB8);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x1ABC);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x1AC0);
    setVector(&scale, 0x1000, 0x1000, 0x1000);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    do {
        for (t = 0; t < 2; t++) {
            for (k = 0; k < 3; k++) {
                for (j = 0; j < 2; j++) {
                    if (j == 1) {
                        otz = RotTransPers4(&unit->a[t][k][0], &unit->a[t][k][1], &unit->a[t][k][0], &unit->a[t][k][1],
                                            &unit->sa[t][k][0], &unit->sa[t][k][1], &unit->sa[t][k][0], &unit->sa[t][k][1], &p,
                                            &status[i][t][k]);
                        unit->otz[t][k][j] = otz;
                        RotTransPers(&unit->b[t][k][1], &unit->sb[t][k][1], &p, &flag);
                        dx = (s16)unit->sa[t][k][j] - (s16)unit->sa[t][k][j - 1];
                        dy = (unit->sa[t][k][j] >> 16) - (unit->sa[t][k][j - 1] >> 16);
                        unit->angle[t][k][j] = ratan2(dy, dx) - 0x400;
                        unit->width[t][k][j] = (s16)unit->sb[t][k][j] - (s16)unit->sa[t][k][j];
                        unit->ox[t][k][j] = rcos(unit->angle[t][k][j]) * unit->width[t][k][j] >> 12;
                        unit->oy[t][k][j] = rsin(unit->angle[t][k][j]) * unit->width[t][k][j] >> 12;
                    } else {
                        unit->otz[t][k][j] = RotTransPers4(&unit->a[t][k][j], &unit->a[t][k][j + 1], &unit->a[t][k][j], &unit->a[t][k][j + 1],
                                                           &unit->sa[t][k][j], &unit->sa[t][k][j + 1], &unit->sa[t][k][j], &unit->sa[t][k][j + 1], &p,
                                                           &status[i][t][k]);
                        RotTransPers(&unit->b[t][k][j], &unit->sb[t][k][j], &p, &flag);
                        dx = (s16)unit->sa[t][k][j + 1] - (s16)unit->sa[t][k][j];
                        dy = (unit->sa[t][k][j + 1] >> 16) - (unit->sa[t][k][j] >> 16);
                        unit->angle[t][k][j] = ratan2(dy, dx) - 0x400;
                        unit->width[t][k][j] = (s16)unit->sb[t][k][j] - (s16)unit->sa[t][k][j];
                        unit->ox[t][k][j] = rcos(unit->angle[t][k][j]) * unit->width[t][k][j] >> 12;
                        unit->oy[t][k][j] = rsin(unit->angle[t][k][j]) * unit->width[t][k][j] >> 12;
                    }
                }
            }
        }
        for (t = 0; t < 2; t++) {
            for (k = 0; k < 3; k++) {
                for (j = 0; j < 1; j++) {
                    poly->x0 = unit->sa[t][k][j] + unit->ox[t][k][j];
                    poly->y0 = unit->oy[t][k][j] + (unit->sa[t][k][j] >> 16);
                    poly->x1 = unit->sa[t][k][j] - unit->ox[t][k][j];
                    poly->y1 = (unit->sa[t][k][j] >> 16) - unit->oy[t][k][j];
                    poly->x2 = unit->sa[t][k][j + 1] + unit->ox[t][k][j + 1];
                    poly->y2 = unit->oy[t][k][j + 1] + (unit->sa[t][k][j + 1] >> 16);
                    poly->x3 = unit->sa[t][k][j + 1] - unit->ox[t][k][j + 1];
                    poly->y3 = (unit->sa[t][k][j + 1] >> 16) - unit->oy[t][k][j + 1];
                    poly->r0 = c0[j + i * 2 + t * 6];
                    poly->g0 = c1[j + i * 2 + t * 6];
                    poly->b0 = c2[j + i * 2 + t * 6];
                    poly->r1 = c0[j + i * 2 + t * 6];
                    poly->g1 = c1[j + i * 2 + t * 6];
                    poly->b1 = c2[j + i * 2 + t * 6];
                    poly->r2 = c0[j + 1 + i * 2 + t * 6];
                    poly->g2 = c1[j + 1 + i * 2 + t * 6];
                    poly->b2 = c2[j + 1 + i * 2 + t * 6];
                    poly->r3 = c0[j + 1 + i * 2 + t * 6];
                    poly->g3 = c1[j + 1 + i * 2 + t * 6];
                    poly->b3 = c2[j + 1 + i * 2 + t * 6];
                    if (unit->otz[t][k][j] >= 0 && status[i][t][k] >= 0) {
                        func_8005B260((u32 *)poly, ot, (u16)unit->otz[t][k][j], 1);
                    }
                }
            }
        }
        i++;
        unit++;
    } while (i < 3);
    unit = (Variant448FanUnit *)(work + 0xBD8);
    if (MODEL_VARIANT_WORD(work, 0x1B3C) == 3) {
        i = 0;
        do {
            for (t = 0; t < 2; t++) {
                for (j = 0; j < 2; j++) {
                    unit->amount[t][j] = (i << 9) + 0x400 + (t << 8) + (j << 9);
                }
                unit->stage[t] = 0;
            }
            i++;
            unit++;
        } while (i < 3);
        MODEL_VARIANT_WORD(work, 0x1B3C) = 4;
    }
}
