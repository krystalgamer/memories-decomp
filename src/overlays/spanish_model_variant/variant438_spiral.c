#include "../../types.h"

#include "../model_variant/variant425_spiral.h"

/* Phase-guarded twelve-arm spiral with an eight-entry palette: while the phase
 * is not negative, the arms are built on a spiral of half the size, moved along
 * the view by the spread, and drawn as two POLY_GT4 halves whose inner and outer
 * colours follow (flags + arm) & 7. The sweep advances by 16 a frame and the
 * size grows to 0x400 once the timing record's bound is reached. */
void func_8013DA34(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG status[12][2];
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    u8 *timing;
    GsOT *ot;
    s16 i;
    s32 phi;
    s32 eighth;
    s32 r;
    Variant425SpiralArm *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 spread;
    s32 turn;
    s32 theta;
    s16 length;
    s16 size;
    s32 dx;
    s32 dy;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x20D8);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x2288), MODEL_VARIANT_WORD(work, 0x2280)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x22E8) >= 0) {
    r = MODEL_VARIANT_HALF(work, 0x22B0) / 2;
    eighth = MODEL_VARIANT_HALF(work, 0x22B0) / 8;
    if (r * MODEL_VARIANT_HALF(work, 0x22AE) < 0) {
        length = 0;
    }
    if (eighth * MODEL_VARIANT_HALF(work, 0x22AE) < 0) {
        length = 0;
    }
    if (MODEL_VARIANT_WORD(work, 0x22E8) >= 3) {
        timing = work + 0x19D0;
        spread = MODEL_VARIANT_WORD(timing, 0x88) / 8;
    } else {
        spread = 0x400 - MODEL_VARIANT_HALF(work, 0x22B2);
    }
    arm = (Variant425SpiralArm *)(work + 0xE40);
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + MODEL_VARIANT_HALF(work, 0x22AC) * 2;
        theta = (i << 13) / 12 + MODEL_VARIANT_HALF(work, 0x22AC);
        for (k = 0; k < 2; k++) {
            if (k == 0) {
                length = spread / 128;
            } else {
                length = spread / 32;
            }
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
    if (!(MODEL_VARIANT_WORD(work, 0x2298) & 1)) {
        size = 0x1000;
    } else {
        size = 0x1200;
    }
    arm = (Variant425SpiralArm *)(work + 0xE40);
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_HALF(work, 0x2264);
    m.t[1] = MODEL_VARIANT_HALF(work, 0x2266);
    m.t[2] = MODEL_VARIANT_HALF(work, 0x2268);
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
                arm->otz[1] = RotTransPers4(&arm->a[0], &arm->a[1], &arm->a[0], &arm->a[1],
                                            &arm->sa[0], &arm->sa[1], &arm->sa[0], &arm->sa[1],
                                            &p, &status[i][1]);
                RotTransPers(&arm->b[1], &arm->sb[1], &p, &flag);
                dx = (s16)arm->sa[1] - (s16)arm->sa[0];
                dy = (arm->sa[1] >> 16) - (arm->sa[0] >> 16);
                arm->angle[1] = ratan2(dy, dx) + 0xC00;
                arm->width[1] = (s16)arm->sb[1] - (s16)arm->sa[1];
                arm->ox[1] = rcos(arm->angle[1]) * arm->width[1] >> 12;
                arm->oy[1] = rsin(arm->angle[1]) * arm->width[1] >> 12;
            } else {
                arm->otz[k] = RotTransPers4(&arm->a[k], &arm->a[k + 1], &arm->a[k], &arm->a[k + 1],
                                            &arm->sa[k], &arm->sa[k + 1], &arm->sa[k], &arm->sa[k + 1],
                                            &p, &status[i][k]);
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
    arm = (Variant425SpiralArm *)(work + 0xE40);
    for (i = 0; i < 12; i++, arm++) {
        u8 r0;
        u8 g0;
        u8 b0;
        u8 r2;
        u8 g2;
        u8 b2;

        if (((MODEL_VARIANT_WORD(work, 0x2298) + i) & 7) == 0) {
            r0 = 128;
            g0 = 0;
            b0 = 0;
            r2 = 192;
            g2 = 128;
            b2 = 128;
        } else if (((MODEL_VARIANT_WORD(work, 0x2298) + i) & 7) == 1) {
            r0 = 128;
            g0 = 64;
            b0 = 0;
            r2 = 192;
            g2 = 160;
            b2 = 128;
        } else if (((MODEL_VARIANT_WORD(work, 0x2298) + i) & 7) == 2) {
            r0 = 128;
            g0 = 128;
            b0 = 0;
            r2 = 192;
            g2 = 192;
            b2 = 128;
        } else if (((MODEL_VARIANT_WORD(work, 0x2298) + i) & 7) == 3) {
            r0 = 0;
            g0 = 128;
            b0 = 0;
            r2 = 128;
            g2 = 192;
            b2 = 128;
        } else if (((MODEL_VARIANT_WORD(work, 0x2298) + i) & 7) == 4) {
            r0 = 0;
            g0 = 128;
            b0 = 128;
            r2 = 128;
            g2 = 192;
            b2 = 192;
        } else if (((MODEL_VARIANT_WORD(work, 0x2298) + i) & 7) == 5) {
            r0 = 0;
            g0 = 0;
            b0 = 128;
            r2 = 128;
            g2 = 128;
            b2 = 192;
        } else if (((MODEL_VARIANT_WORD(work, 0x2298) + i) & 7) == 6) {
            r0 = 128;
            g0 = 0;
            b0 = 128;
            r2 = 192;
            g2 = 128;
            b2 = 192;
        } else if (((MODEL_VARIANT_WORD(work, 0x2298) + i) & 7) == 7) {
            r0 = 128;
            g0 = 0;
            b0 = 64;
            r2 = 192;
            g2 = 128;
            b2 = 160;
        }
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
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = 0;
                poly->g3 = 0;
                poly->b3 = 0;
            } else {
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = r0;
                poly->g1 = g0;
                poly->b1 = b0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = r2;
                poly->g3 = g2;
                poly->b3 = b2;
            }
            if (arm->otz[k] > 0) {
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
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = 0;
                poly->g3 = 0;
                poly->b3 = 0;
            } else {
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = r0;
                poly->g1 = g0;
                poly->b1 = b0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = r2;
                poly->g3 = g2;
                poly->b3 = b2;
            }
            if (arm->otz[k] >= 0 && status[i][k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k]);
            }
        }
    }
    }
    MODEL_VARIANT_HALF(work, 0x22AC) += 16;
    if ((u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x22B4), 0x20) <= (u32)MODEL_VARIANT_WORD(work, 0x229C) &&
        MODEL_VARIANT_WORD(work, 0x22EC) == 0 && MODEL_VARIANT_HALF(work, 0x22B0) < 0x400) {
        MODEL_VARIANT_HALF(work, 0x22B0) += MODEL_VARIANT_WORD(work, 0x22A4) * 64;
        if (MODEL_VARIANT_HALF(work, 0x22B0) >= 0x400) {
            MODEL_VARIANT_HALF(work, 0x22B0) = 0x400;
            MODEL_VARIANT_WORD(work, 0x22EC) = 2;
        }
    }
}
