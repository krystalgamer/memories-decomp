#include "../../types.h"
#include "variant448_fan.h"

/* Header 448's fans: three 0x208-byte fan units at `work + 0xBD8`, each two
 * arms of three fans. Per unit it builds the fans' spines and their copies
 * moved along the view, fades the arm colours into a stack table by the
 * arms' growth amounts, steps the amounts and stages, projects the spines and
 * draws one POLY_GT4 per fan. Several statements are written for the retail
 * register allocation and scheduling rather than for reading: the flag test
 * at the start keeps an arm whose copy of `work` only the combiner reduces to
 * a no-op, some stores sit in `do { } while (0)` blocks whose loop notes keep
 * them in place, and the colour addresses use one name per channel. */

void ModelVariant448_DrawFans(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG status[3][2][3];
    u8 col[6][16];
    PSXLONG p;
    PSXLONG flag;
    u8 *q;
    u8 *q1;
    u8 *work;
    GsOT *ot;
    POLY_GT4 *poly;
    Variant448FanUnit *unit;
    s16 i;
    s32 angle;
    s16 t;
    s16 k;
    s16 j;
    u8 *q2;
    s32 u, i2;
    s32 base, sarow, sp0, anrow, wrow, cu, w1, w2, w3;
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
        s32 s = work - (u8 *)poly;
        work = (u8 *)poly + s;
    }
    unit = (Variant448FanUnit *)(work + 0xBD8);
    i = 0;
    iangle = 0;
    length = 0x20;
    do {
        for (t = 0, tangle = 0; t < 2; t++) {
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
            tangle += 0x800;
        }
        i2 = i * 2;
        for (t = 0; t < 2; t++) {
            for (j = 0; j < 2; j++) {
                v = unit->amount[t][j];
                if (v < 0x200) {
                    s32 ip0, ip5;
                    ip0 = t * 6 + (s32)&rot + 240;
                    ip5 = t * 6 + (s32)&rot + 320;
                    {
                        s32 x;
                        u8 c;
                        x = i2 + ip0 + j;
                        c = unit->cb[j][0];
                        do {
                            *(u8 *)x = c;
                        } while (0);
                    }
                    {
                        s32 w; u = t * 6 + (s32)&rot + 256; w = i2 + u; u = w + j; *(u8 *)u = unit->cb[j][1];
                    }
                    {
                        s32 w; u = t * 6 + (s32)&rot + 272; w = i2 + u; u = w + j; *(u8 *)u = unit->cb[j][2];
                    }
                    {
                        s32 w; u = t * 6 + (s32)&rot + 288; w = i2 + u; u = w + j; *(u8 *)u = unit->ca[j][0];
                    }
                    {
                        s32 w; u = t * 6 + (s32)&rot + 304; w = i2 + u; u = w + j; *(u8 *)u = unit->ca[j][1];
                    }
                    {
                        s32 x;
                        x = i2 + ip5 + j;
                        *(u8 *)x = unit->ca[j][2];
                    }
                } else {
                    s32 ip0, ip5, jj;
                    f = 0;
                    if (v < 0x400) {
                        f = 0x400 - v;
                    }
                    jj = j;
                    ip0 = t * 6 + (s32)&rot + 240;
                    {
                        s32 a; a = i2 + ip0 + jj; *(u8 *)a = unit->cb[j][0] * f / 512;
                    }
                    {
                        s32 a, b, d; a = t * 6 + (s32)&rot + 256; b = i2 + a; d = b + jj; *(u8 *)d = unit->cb[j][1] * f / 512;
                    }
                    {
                        s32 a, b, d; a = t * 6 + (s32)&rot + 272; b = i2 + a; d = b + jj; *(u8 *)d = unit->cb[j][2] * f / 512;
                    }
                    {
                        s32 a, b, d; a = t * 6 + (s32)&rot + 288; b = i2 + a; d = b + jj; *(u8 *)d = unit->ca[j][0] * f / 512;
                    }
                    {
                        s32 a, b, d; a = t * 6 + (s32)&rot + 304; b = i2 + a; d = b + jj; *(u8 *)d = unit->ca[j][1] * f / 512;
                    }
                    ip5 = t * 6 + (s32)&rot + 320;
                    {
                        s32 a; a = i2 + ip5 + j; *(u8 *)a = unit->ca[j][2] * f / 512;
                    }
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
                        base = t * 24 + (s32)unit;
                        unit->otz[t][k][j] = otz;
                        RotTransPers(&unit->b[t][k][1], &unit->sb[t][k][1], &p, &flag);
                        sarow = base + 96;
                        sp0 = k * 8 + sarow;
                        anrow = base + 144;
                        dx = *(s16 *)(sp0 + 4) - *(s16 *)(sp0 + 0);
                        dy = *(s16 *)(sp0 + 6) - *(s16 *)(sp0 + 2);
                        *(s32 *)(k * 8 + anrow + 4) = ratan2(dy, dx) - 0x400;
                        wrow = base + 336;
                        *(s32 *)(k * 8 + wrow + 4) = (s16)unit->sb[t][k][j] - *(s16 *)(sp0 + 4);
                        unit->ox[t][k][j] = rcos(*(s32 *)(k * 8 + anrow + 4)) * *(s32 *)(k * 8 + wrow + 4) >> 12;
                        unit->oy[t][k][j] = rsin(*(s32 *)(k * 8 + anrow + 4)) * *(s32 *)(k * 8 + wrow + 4) >> 12;
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
                    cu = t * 6 + (s32)&rot;
                    w1 = cu + 240;
                    q = (u8 *)(i * 2 + w1 + j);
                    w2 = cu + 256;
                    q1 = (u8 *)(i * 2 + w2 + j);
                    w3 = cu + 272;
                    q2 = (u8 *)(i * 2 + w3 + j);
                    poly->r0 = q[0];
                    poly->g0 = q1[0];
                    poly->b0 = q2[0];
                    poly->r1 = q[0];
                    poly->g1 = q1[0];
                    poly->b1 = q2[0];
                    poly->r2 = q[1];
                    poly->g2 = q1[1];
                    poly->b2 = q2[1];
                    poly->r3 = q[1];
                    poly->g3 = q1[1];
                    poly->b3 = q2[1];
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
