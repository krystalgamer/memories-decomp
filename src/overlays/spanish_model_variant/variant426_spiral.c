#include "../../types.h"

#include "../model_variant/variant425_spiral.h"

/* Phase-guarded twelve-arm spiral: while the phase is not negative, the arms
 * are built on a spiral of radius 0x300 (or three quarters of the size before
 * phase 1), moved along the view by a flicker-selected share of the size, and
 * drawn as two POLY_GT4 halves with negative depths clamped to zero. The size
 * grows to 0x400 before phase 1 and shrinks to zero after it; the sweep advances
 * by 40 a frame. */
void func_8013C3B0(u8 *ctx)
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
    u8 c0;
    u8 c1;
    u8 c3;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x1F90);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x20EC), MODEL_VARIANT_WORD(work, 0x20E4)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x2180) >= 0) {
    if (MODEL_VARIANT_WORD(work, 0x2180) <= 0) {
        r = MODEL_VARIANT_HALF(work, 0x2150) * 0x300 / 0x400;
    } else {
        r = 0x300;
    }
    eighth = MODEL_VARIANT_HALF(work, 0x2150) / 8;
    if (r * MODEL_VARIANT_HALF(work, 0x214E) < 0) {
        length = 0;
    }
    if (eighth * MODEL_VARIANT_HALF(work, 0x214E) < 0) {
        length = 0;
    }
    spread = MODEL_VARIANT_HALF(work, 0x2150);
    arm = (Variant425SpiralArm *)(work + 0x84C);
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + MODEL_VARIANT_HALF(work, 0x214C) * 2;
        theta = (i << 13) / 12 + MODEL_VARIANT_HALF(work, 0x214C);
        for (k = 0; k < 2; k++) {
            if (!(MODEL_VARIANT_WORD(work, 0x20FC) & 1)) {
                length = (k * 32 + 16) * spread / 1024;
            } else {
                length = (k * 16 + 8) * spread / 1024;
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
    if (!(MODEL_VARIANT_WORD(work, 0x20FC) & 1)) {
        size = 0x1000;
    } else {
        size = 0x1200;
    }
    arm = (Variant425SpiralArm *)(work + 0x84C);
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x20BC);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x20C0);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x20C4);
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
    c0 = 0;
    c1 = 0x80;
    c3 = 0xC0;
    arm = (Variant425SpiralArm *)(work + 0x84C);
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
                poly->g0 = c0;
                poly->b0 = c1;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = c1;
                poly->g2 = c1;
                poly->b2 = c3;
                poly->r3 = 0;
                poly->g3 = 0;
                poly->b3 = 0;
            } else {
                poly->r0 = c0;
                poly->g0 = c0;
                poly->b0 = c1;
                poly->r1 = c0;
                poly->g1 = c0;
                poly->b1 = c1;
                poly->r2 = c1;
                poly->g2 = c1;
                poly->b2 = c3;
                poly->r3 = c1;
                poly->g3 = c1;
                poly->b3 = c3;
            }
            if (arm->otz[k] < 0) {
                arm->otz[k] = 0;
            }
            status[i][k] = 0;
            if (arm->otz[k] >= 0) {
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
                poly->g0 = c0;
                poly->b0 = c1;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = c1;
                poly->g2 = c1;
                poly->b2 = c3;
                poly->r3 = 0;
                poly->g3 = 0;
                poly->b3 = 0;
            } else {
                poly->r0 = c0;
                poly->g0 = c0;
                poly->b0 = c1;
                poly->r1 = c0;
                poly->g1 = c0;
                poly->b1 = c1;
                poly->r2 = c1;
                poly->g2 = c1;
                poly->b2 = c3;
                poly->r3 = c1;
                poly->g3 = c1;
                poly->b3 = c3;
            }
            if (arm->otz[k] < 0) {
                arm->otz[k] = 0;
            }
            status[i][k] = 0;
            if (arm->otz[k] > 0) {
                GsSortPoly(poly, ot, arm->otz[k]);
            }
        }
    }
    }
    if (MODEL_VARIANT_HALF(work, 0x2150) < 0x400 && MODEL_VARIANT_WORD(work, 0x2180) <= 0) {
        MODEL_VARIANT_HALF(work, 0x2150) += MODEL_VARIANT_WORD(work, 0x2108) * 64;
        if (MODEL_VARIANT_HALF(work, 0x2150) >= 0x400) {
            MODEL_VARIANT_HALF(work, 0x2150) = 0x400;
        }
    } else if (MODEL_VARIANT_HALF(work, 0x2150) > 0) {
        MODEL_VARIANT_HALF(work, 0x2150) -= MODEL_VARIANT_WORD(work, 0x2108) * 16;
        if (MODEL_VARIANT_HALF(work, 0x2150) <= 0) {
            MODEL_VARIANT_HALF(work, 0x2150) = 0;
        }
    }
    MODEL_VARIANT_HALF(work, 0x214C) += 40;
}
