#include "../../types.h"

#include "../model_variant/variant418_spiral.h"

/* Phase-guarded three-pass twelve-arm spiral: while the phase is not negative,
 * each pass builds the arms on a spiral of the size at work + 0x3794 turned by
 * a further 1300, moves them along the view by the spread, and draws them as
 * two POLY_GT4 halves at the pass position where the depth and the arm's
 * projection flag are not negative. Reaching size 0x400 moves phase 1 to 2;
 * from phase 2 on the sweep advances by step * 24. */
void func_8013C57C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    u8 *timing;
    u8 *pos;
    GsOT *ot;
    s16 i;
    s16 n;
    s32 phi;
    s32 offset;
    s32 eighth;
    s32 r;
    Variant418SpiralArm *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 spread;
    s32 turn;
    s32 theta;
    s16 length;
    s16 size;
    s32 dx;
    s32 dy;
    u8 c0;
    u8 c1;
    u8 c3;

    work = ctx;
    timing = work + 0x1FF8;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x3560);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x372C), MODEL_VARIANT_WORD(work, 0x3724)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x37C4) >= 0) {
    for (n = 0, offset = 0; n < 3; n++, offset += 1300) {
        r = MODEL_VARIANT_HALF(work, 0x3794);
        eighth = r / 8;
        if (r * MODEL_VARIANT_HALF(work, 0x3792) < 0) {
            length = 0;
        }
        if (eighth * MODEL_VARIANT_HALF(work, 0x3792) < 0) {
            length = 0;
        }
        if (MODEL_VARIANT_WORD(work, 0x37C4) >= 3) {
            timing += 0x98;
            spread = MODEL_VARIANT_WORD(timing, 0x88) / 8;
        } else {
            spread = 0x400 - MODEL_VARIANT_HALF(work, 0x3796);
        }
        arm = (Variant418SpiralArm *)(work + 0x19C8);
        for (i = 0; i < 12; i++, arm++) {
            phi = offset + (i << 12) / 12 + MODEL_VARIANT_HALF(work, 0x3790) * 2;
            theta = offset + (i << 13) / 12 + MODEL_VARIANT_HALF(work, 0x3790);
            for (k = 0; k < 2; k++) {
                length = (k * 48) * spread / 1024;
                if (k == 0) {
                    setVector(&arm->a[k], 0, 0, 0);
                } else {
                    setVector(&arm->a[k], rcos(phi) * (rsin(theta) * (r * k) >> 12) >> 12,
                              rcos(theta) * (r * k) >> 12,
                              rsin(phi) * (rsin(theta) * (r * k) >> 12) >> 12);
                }
                setVector(&arm->b[k], arm->a[k].vx + (rcos(turn) * length >> 12), arm->a[k].vy,
                          arm->a[k].vz + (rsin(turn) * length >> 12));
            }
        }
        if (!(MODEL_VARIANT_WORD(work, 0x373C) & 1)) {
            size = 0x1000;
        } else {
            size = 0x1200;
        }
        arm = (Variant418SpiralArm *)(work + 0x19C8);
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        pos = work + n * 32;
        m.t[0] = MODEL_VARIANT_WORD(pos, 0x363C);
        m.t[1] = MODEL_VARIANT_WORD(pos, 0x3640);
        m.t[2] = MODEL_VARIANT_WORD(pos, 0x3644);
        scale.vx = size;
        scale.vy = size;
        scale.vz = size;
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        for (i = 0; i < 12; i++, arm++) {
            for (k = 0; k < 2; k++) {
                if (k == 1) {
                    arm->otz[k] = RotTransPers4(&arm->a[0], &arm->a[k], &arm->a[0], &arm->a[k],
                                                &arm->sa[0], &arm->sa[k], &arm->sa[0], &arm->sa[k],
                                                &p, &arm->flag[k]);
                    RotTransPers(&arm->b[k], &arm->sb[k], &p, &flag);
                    dx = (s16)arm->sa[k] - (s16)arm->sa[0];
                    dy = (arm->sa[k] >> 16) - (arm->sa[0] >> 16);
                    arm->angle[k] = ratan2(dy, dx) + 0xC00;
                    arm->width[k] = (s16)arm->sb[k] - (s16)arm->sa[k];
                    arm->ox[k] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                    arm->oy[k] = rsin(arm->angle[k]) * arm->width[k] >> 12;
                } else {
                    arm->otz[k] = RotTransPers4(&arm->a[k], &arm->a[k + 1], &arm->a[k], &arm->a[k + 1],
                                                &arm->sa[k], &arm->sa[k + 1], &arm->sa[k], &arm->sa[k + 1],
                                                &p, &arm->flag[k]);
                    RotTransPers(&arm->b[k], &arm->sb[k], &p, &flag);
                    dx = (s16)arm->sa[k + 1] - (s16)arm->sa[k];
                    dy = (arm->sa[k + 1] >> 16) - (arm->sa[k] >> 16);
                    arm->angle[k] = ratan2(dy, dx) + 0xC00;
                    arm->width[k] = (s16)arm->sb[k] - (s16)arm->sa[k];
                    arm->ox[k] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                    arm->oy[k] = rsin(arm->angle[k]) * arm->width[k] >> 12;
                }
            }
        }
        c0 = 0;
        c3 = 0xC0;
        c1 = 0x80;
        arm = (Variant418SpiralArm *)(work + 0x19C8);
        for (i = 0; i < 12; i++, arm++) {
            for (k = 0; k < 1; k++) {
                poly->x0 = arm->sa[k] + arm->ox[k];
                poly->y0 = (arm->sa[k] >> 16) + arm->oy[k];
                poly->x1 = arm->sa[k + 1] + arm->ox[k + 1];
                poly->y1 = (arm->sa[k + 1] >> 16) + arm->oy[k + 1];
                poly->x2 = arm->sa[k];
                poly->y2 = arm->sa[k] >> 16;
                poly->x3 = arm->sa[k + 1];
                poly->y3 = arm->sa[k + 1] >> 16;
                if (k + 1 == 1) {
                    poly->r0 = c0;
                    poly->g0 = c3;
                    poly->b0 = c0;
                    poly->r1 = 0;
                    poly->g1 = 0;
                    poly->b1 = 0;
                    poly->r2 = c1;
                    poly->g2 = c3;
                    poly->b2 = c1;
                    poly->r3 = 0;
                    poly->g3 = 0;
                    poly->b3 = 0;
                } else {
                    poly->r0 = c0;
                    poly->g0 = c3;
                    poly->b0 = c0;
                    poly->r1 = c0;
                    poly->g1 = c3;
                    poly->b1 = c0;
                    poly->r2 = c1;
                    poly->g2 = c3;
                    poly->b2 = c1;
                    poly->r3 = c1;
                    poly->g3 = c3;
                    poly->b3 = c1;
                }
                if (arm->otz[k] >= 0 && arm->flag[k] >= 0) {
                    GsSortPoly(poly, ot, arm->otz[k]);
                }
                poly->x0 = arm->sa[k] - arm->ox[k];
                poly->y0 = (arm->sa[k] >> 16) - arm->oy[k];
                poly->x1 = arm->sa[k + 1] - arm->ox[k + 1];
                poly->y1 = (arm->sa[k + 1] >> 16) - arm->oy[k + 1];
                poly->x2 = arm->sa[k];
                poly->y2 = arm->sa[k] >> 16;
                poly->x3 = arm->sa[k + 1];
                poly->y3 = arm->sa[k + 1] >> 16;
                if (k + 1 == 1) {
                    poly->r0 = c0;
                    poly->g0 = c3;
                    poly->b0 = c0;
                    poly->r1 = 0;
                    poly->g1 = 0;
                    poly->b1 = 0;
                    poly->r2 = c1;
                    poly->g2 = c3;
                    poly->b2 = c1;
                    poly->r3 = 0;
                    poly->g3 = 0;
                    poly->b3 = 0;
                } else {
                    poly->r0 = c0;
                    poly->g0 = c3;
                    poly->b0 = c0;
                    poly->r1 = c0;
                    poly->g1 = c3;
                    poly->b1 = c0;
                    poly->r2 = c1;
                    poly->g2 = c3;
                    poly->b2 = c1;
                    poly->r3 = c1;
                    poly->g3 = c3;
                    poly->b3 = c1;
                }
                if (arm->otz[k] >= 0 && arm->flag[k] >= 0) {
                    GsSortPoly(poly, ot, arm->otz[k]);
                }
            }
        }
    }
    }
    if (MODEL_VARIANT_HALF(work, 0x3794) < 0x400) {
        MODEL_VARIANT_HALF(work, 0x3794) += MODEL_VARIANT_WORD(work, 0x3748) * 64;
        if (MODEL_VARIANT_HALF(work, 0x3794) >= 0x400) {
            MODEL_VARIANT_HALF(work, 0x3794) = 0x400;
            if (MODEL_VARIANT_WORD(work, 0x37C4) == 1) {
                MODEL_VARIANT_WORD(work, 0x37C4) = 2;
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x37C4) >= 2) {
        MODEL_VARIANT_HALF(work, 0x3790) += MODEL_VARIANT_WORD(work, 0x3748) * 24;
    }
}
