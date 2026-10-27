#include "../../types.h"

#include "variant479_spiral.h"

/* Phase-guarded three-pass twelve-arm spiral: while the phase is not negative,
 * each pass builds three-point arms on a spiral of 800/1024 of the size at
 * work + 0x41BC turned by a further 768, moves them along the view by the
 * spread, projects them, and draws two POLY_GT4 halves per segment at the pass
 * position where the depth and the projection flag are not negative. Reaching
 * size 0x400 moves phase 1 to 2; from phase 2 on the sweep advances by 48. */
void func_8013C4F8(u8 *ctx)
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
    s32 offset;
    s32 eighth;
    s32 r;
    Variant479SpiralArm *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 spread;
    s32 turn;
    s32 phi;
    s32 theta;
    s16 length;
    s16 size;
    s32 dx;
    s32 dy;
    u8 c0;
    u8 c1;
    u8 c3;
    u8 c2;

    work = ctx;
    timing = work + 0x2D98;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x3F48);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x4154), MODEL_VARIANT_WORD(work, 0x414C)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x41EC) >= 0) {
        for (n = 0, offset = 0; n < 3; n++, offset += 768) {
            r = MODEL_VARIANT_HALF(work, 0x41BC) * 800 / 1024;
            eighth = MODEL_VARIANT_HALF(work, 0x41BC) / 8;
            if (r * MODEL_VARIANT_HALF(work, 0x41BA) < 0) {
                length = 0;
            }
            if (eighth * MODEL_VARIANT_HALF(work, 0x41BA) < 0) {
                length = 0;
            }
            if (MODEL_VARIANT_WORD(work, 0x41EC) >= 3) {
                timing += 0x98;
                spread = MODEL_VARIANT_WORD(timing, 0x88) / 8;
            } else {
                spread = 0x400 - MODEL_VARIANT_HALF(work, 0x41BE);
            }
            arm = (Variant479SpiralArm *)(work + 0x2468);
            for (i = 0; i < 12; i++, arm++) {
                phi = (i << 12) / 12 + MODEL_VARIANT_HALF(work, 0x41B8) * 2;
                theta = (i << 13) / 12 + MODEL_VARIANT_HALF(work, 0x41B8);
                for (k = 0; k < 3; k++) {
                    length = (k * 16 + 16) * spread / 1024;
                    if (k == 0) {
                        setVector(&arm->a[k], 0, 0, 0);
                    } else {
                        setVector(&arm->a[k], rcos(phi + offset) * (rsin(theta + offset) * (r * k / 2) >> 12) >> 12,
                                  rcos(theta + offset) * (r * k / 2) >> 12,
                                  rsin(phi + offset) * (rsin(theta + offset) * (r * k / 2) >> 12) >> 12);
                    }
                    setVector(&arm->b[k], arm->a[k].vx + (rcos(turn) * length >> 12), arm->a[k].vy,
                              arm->a[k].vz + (rsin(turn) * length >> 12));
                }
            }
            if (!(MODEL_VARIANT_WORD(work, 0x4164) & 1)) {
                size = 0x1000;
            } else {
                size = 0x1200;
            }
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            pos = work + n * 32;
            m.t[0] = MODEL_VARIANT_WORD(pos, 0x4024);
            m.t[1] = MODEL_VARIANT_WORD(pos, 0x4028);
            m.t[2] = MODEL_VARIANT_WORD(pos, 0x402C);
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
            arm = (Variant479SpiralArm *)(work + 0x2468);
            for (i = 0; i < 12; i++, arm++) {
                for (k = 0; k < 3; k++) {
                    if (k == 2) {
                        arm->otz[k] = RotTransPers4(&arm->a[k - 1], &arm->a[k], &arm->a[k - 1], &arm->a[k],
                                                    &arm->sa[k - 1], &arm->sa[k], &arm->sa[k - 1], &arm->sa[k],
                                                    &p, &arm->flag[k]);
                        RotTransPers(&arm->b[k], &arm->sb[k], &p, &flag);
                        dx = (s16)arm->sa[k] - (s16)arm->sa[k - 1];
                        dy = (arm->sa[k] >> 16) - (arm->sa[k - 1] >> 16);
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
            c2 = 0xA0;
            arm = (Variant479SpiralArm *)(work + 0x2468);
            for (i = 0; i < 12; i++, arm++) {
                for (k = 0; k < 2; k++) {
                    poly->x0 = arm->sa[k] + arm->ox[k];
                    poly->y0 = (arm->sa[k] >> 16) + arm->oy[k];
                    poly->x1 = arm->sa[k + 1] + arm->ox[k + 1];
                    poly->y1 = (arm->sa[k + 1] >> 16) + arm->oy[k + 1];
                    poly->x2 = arm->sa[k];
                    poly->y2 = arm->sa[k] >> 16;
                    poly->x3 = arm->sa[k + 1];
                    poly->y3 = arm->sa[k + 1] >> 16;
                    if (k + 1 == 2) {
                        poly->r0 = c0;
                        poly->g0 = c3;
                        poly->b0 = c0;
                        poly->r1 = 0;
                        poly->g1 = 0;
                        poly->b1 = 0;
                        poly->r2 = c1;
                        poly->g2 = c2;
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
                        poly->g2 = c2;
                        poly->b2 = c1;
                        poly->r3 = c1;
                        poly->g3 = c2;
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
                    if (k + 1 == 2) {
                        poly->r0 = c0;
                        poly->g0 = c3;
                        poly->b0 = c0;
                        poly->r1 = 0;
                        poly->g1 = 0;
                        poly->b1 = 0;
                        poly->r2 = c1;
                        poly->g2 = c2;
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
                        poly->g2 = c2;
                        poly->b2 = c1;
                        poly->r3 = c1;
                        poly->g3 = c2;
                        poly->b3 = c1;
                    }
                    if (arm->otz[k] >= 0 && arm->flag[k] >= 0) {
                        GsSortPoly(poly, ot, arm->otz[k]);
                    }
                }
            }
        }
    }
    if (MODEL_VARIANT_HALF(work, 0x41BC) < 0x400) {
        MODEL_VARIANT_HALF(work, 0x41BC) += MODEL_VARIANT_WORD(work, 0x4170) * 64;
        if (MODEL_VARIANT_HALF(work, 0x41BC) >= 0x400) {
            MODEL_VARIANT_HALF(work, 0x41BC) = 0x400;
            if (MODEL_VARIANT_WORD(work, 0x41EC) == 1) {
                MODEL_VARIANT_WORD(work, 0x41EC) = 2;
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x41EC) >= 2) {
        MODEL_VARIANT_HALF(work, 0x41B8) += 48;
    }
}
