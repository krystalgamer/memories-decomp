#include "../../types.h"

#include "../model_variant/variant448_spiral.h"

/* Header 425's phase-guarded spiral: while the phase at work + 0x180C is not
 * negative, twelve two-point arms (records from work + 0x84C) on a spiral of
 * three quarters of the size at work + 0x17DC are moved along the view by the
 * spread and drawn as two POLY_GT4 halves in each arm's own colours where the
 * depth and projection flag are not negative. Reaching size 0x400 moves phase
 * 1 to 2; from phase 2 on the sweep at work + 0x17D8 advances by 48. */
void func_8013C370(u8 *ctx)
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
    Variant448SpiralArm *arm;
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
    poly = (POLY_GT4 *)(work + 0x166C);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x1778), MODEL_VARIANT_WORD(work, 0x1770)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x180C) >= 0) {
    r = MODEL_VARIANT_HALF(work, 0x17DC) * 768 / 1024;
    eighth = MODEL_VARIANT_HALF(work, 0x17DC) / 8;
    if (r * MODEL_VARIANT_HALF(work, 0x17DA) < 0) {
        length = 0;
    }
    if (eighth * MODEL_VARIANT_HALF(work, 0x17DA) < 0) {
        length = 0;
    }
    if (MODEL_VARIANT_WORD(work, 0x180C) >= 3) {
        timing = work + 0xEB4;
        spread = MODEL_VARIANT_WORD(timing, 0x88) / 8;
    } else {
        spread = 0x400 - MODEL_VARIANT_HALF(work, 0x17DE);
    }
    arm = (Variant448SpiralArm *)(work + 0x84C);
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + MODEL_VARIANT_HALF(work, 0x17D8) * 2;
        theta = (i << 13) / 12 + MODEL_VARIANT_HALF(work, 0x17D8);
        for (k = 0; k < 2; k++) {
            length = (k * 16 + 4) * spread / 1024;
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
    if (!(MODEL_VARIANT_WORD(work, 0x1788) & 1)) {
        size = 0x1000;
    } else {
        size = 0x1200;
    }
    arm = (Variant448SpiralArm *)(work + 0x84C);
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x1748);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x174C);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x1750);
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
    arm = (Variant448SpiralArm *)(work + 0x84C);
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
            poly->r0 = arm->ca[k][0];
            poly->g0 = arm->ca[k][1];
            poly->b0 = arm->ca[k][2];
            poly->r1 = arm->ca[k + 1][0];
            poly->g1 = arm->ca[k + 1][1];
            poly->b1 = arm->ca[k + 1][2];
            poly->r2 = arm->cb[k][0];
            poly->g2 = arm->cb[k][1];
            poly->b2 = arm->cb[k][2];
            poly->r3 = arm->cb[k + 1][0];
            poly->g3 = arm->cb[k + 1][1];
            poly->b3 = arm->cb[k + 1][2];
            if (arm->otz[k] >= 0 && status[i][k] >= 0) {
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
            poly->r0 = arm->ca[k][0];
            poly->g0 = arm->ca[k][1];
            poly->b0 = arm->ca[k][2];
            poly->r1 = arm->ca[k + 1][0];
            poly->g1 = arm->ca[k + 1][1];
            poly->b1 = arm->ca[k + 1][2];
            poly->r2 = arm->cb[k][0];
            poly->g2 = arm->cb[k][1];
            poly->b2 = arm->cb[k][2];
            poly->r3 = arm->cb[k + 1][0];
            poly->g3 = arm->cb[k + 1][1];
            poly->b3 = arm->cb[k + 1][2];
            if (arm->otz[k] >= 0 && status[i][k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k]);
            }
        }
    }
    }
    if (MODEL_VARIANT_HALF(work, 0x17DC) < 0x400) {
        MODEL_VARIANT_HALF(work, 0x17DC) += MODEL_VARIANT_WORD(work, 0x1794) * 64;
        if (MODEL_VARIANT_HALF(work, 0x17DC) >= 0x400) {
            MODEL_VARIANT_HALF(work, 0x17DC) = 0x400;
            if (MODEL_VARIANT_WORD(work, 0x180C) == 1) {
                MODEL_VARIANT_WORD(work, 0x180C) = 2;
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x180C) >= 2) {
        MODEL_VARIANT_HALF(work, 0x17D8) += 48;
    }
}
