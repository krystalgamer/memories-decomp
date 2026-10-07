#include "../../types.h"
#include "variant388_petals.h"

/* Builds and draws twelve two-point petal arms around the origin while the
 * state word at +0x1898 is non-negative, then advances the spin, grows the
 * radius after the first timing mark and widens the spread over the timing
 * window. */
void func_8013D410(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    GsOT *ot;
    s16 i;
    s32 phi;
    s32 radius;
    s32 eighth;
    s32 distance;
    s32 spread;
    s32 turn;
    Petal388Arm *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 theta;
    s16 length;
    s16 size;
    s32 dx;
    s32 dy;

    work = ctx;
    arm = (Petal388Arm *)(work + 0x1050);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x16C0);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x1848), MODEL_VARIANT_WORD(work, 0x1840)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x1898) >= 0) {
        radius = MODEL_VARIANT_HALF(work, 0x1888) / 2;
        eighth = MODEL_VARIANT_HALF(work, 0x1888) / 8;
        /* As in the MODEL418 template, both products are tested for nothing. */
        if (radius * MODEL_VARIANT_HALF(work, 0x1886) < 0) {
            length = 0;
        }
        if (eighth * MODEL_VARIANT_HALF(work, 0x1886) < 0) {
            length = 0;
        }
        spread = 0x400 - MODEL_VARIANT_HALF(work, 0x188A);
        for (i = 0; i < 12; i++, arm++) {
            phi = (i << 12) / 12 + MODEL_VARIANT_HALF(work, 0x1884) * 2;
            theta = (i << 13) / 12 + MODEL_VARIANT_HALF(work, 0x1884);
            for (k = 0; k < 2; k++) {
                if (k == 0) {
                    length = spread * 8 / 1024;
                } else {
                    length = spread * 24 / 1024;
                }
                if (k == 0) {
                    setVector(&arm->a[0], 0, 0, 0);
                } else {
                    distance = radius * k;
                    setVector(&arm->a[k], rcos(phi) * (rsin(theta) * distance >> 12) >> 12,
                              rcos(theta) * distance >> 12,
                              rsin(phi) * (rsin(theta) * distance >> 12) >> 12);
                }
                setVector(&arm->b[k], arm->a[k].vx + (rcos(turn) * length >> 12), arm->a[k].vy,
                          arm->a[k].vz + (rsin(turn) * length >> 12));
            }
        }
        if (!(MODEL_VARIANT_WORD(work, 0x1858) & 1)) {
            size = 0x1000;
        } else {
            size = 0x1200;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work, 0x1818);
        m.t[1] = MODEL_VARIANT_WORD(work, 0x181C);
        m.t[2] = MODEL_VARIANT_WORD(work, 0x1820);
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
        arm = (Petal388Arm *)(work + 0x1050);
        for (i = 0; i < 12; i++, arm++) {
            for (k = 0; k < 2; k++) {
                if (k == 1) {
                    arm->otz[k] = RotTransPers4(&arm->a[0], &arm->a[k], &arm->a[0], &arm->a[k],
                                                &arm->sa[0], &arm->sa[k], &arm->sa[0], &arm->sa[k], &p, &arm->flag[k]);
                    RotTransPers(&arm->b[k], &arm->sb[k], &p, &flag);
                    dx = (s16)arm->sa[k] - (s16)arm->sa[0];
                    dy = (arm->sa[k] >> 16) - (arm->sa[0] >> 16);
                    arm->angle[k] = ratan2(dy, dx) + 0xC00;
                    arm->width[k] = (s16)arm->sb[k] - (s16)arm->sa[k];
                    arm->ox[k] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                    arm->oy[k] = rsin(arm->angle[k]) * arm->width[k] >> 12;
                } else {
                    arm->otz[k] = RotTransPers4(&arm->a[k], &arm->a[k + 1], &arm->a[k], &arm->a[k + 1],
                                                &arm->sa[k], &arm->sa[k + 1], &arm->sa[k], &arm->sa[k + 1], &p, &arm->flag[k]);
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
        arm = (Petal388Arm *)(work + 0x1050);
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
                if (arm->otz[k] >= 0 && arm->flag[k] >= 0) {
                    GsSortPoly(poly, ot, arm->otz[k] & 0xFFFF);
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
                if (arm->otz[k] >= 0 && arm->flag[k] >= 0) {
                    GsSortPoly(poly, ot, arm->otz[k] & 0xFFFF);
                }
            }
        }
    }
    MODEL_VARIANT_HALF(work, 0x1884) += 16;
    if (*(u32 *)(MODEL_VARIANT_WORD(work, 0x186C) + 0x10) <= MODEL_VARIANT_WORD(work, 0x185C)
        && MODEL_VARIANT_WORD(work, 0x189C) == 0 && MODEL_VARIANT_HALF(work, 0x1888) < 0x400) {
        MODEL_VARIANT_HALF(work, 0x1888) += MODEL_VARIANT_WORD(work, 0x1864) * 64;
        if (MODEL_VARIANT_HALF(work, 0x1888) >= 0x400) {
            MODEL_VARIANT_HALF(work, 0x1888) = 0x400;
            MODEL_VARIANT_WORD(work, 0x189C) = 2;
        }
    }
    if ((u32)MODEL_VARIANT_WORD(work, 0x185C) >= *(u32 *)(MODEL_VARIANT_WORD(work, 0x186C) + 0x1C)
        && MODEL_VARIANT_HALF(work, 0x188A) < 0x400) {
        MODEL_VARIANT_HALF(work, 0x188A) = (((u32)MODEL_VARIANT_WORD(work, 0x185C)
            - *(u32 *)(MODEL_VARIANT_WORD(work, 0x186C) + 0x1C)) << 10)
            / (*(u32 *)(MODEL_VARIANT_WORD(work, 0x186C) + 0x20) - *(u32 *)(MODEL_VARIANT_WORD(work, 0x186C) + 0x1C));
        if (MODEL_VARIANT_HALF(work, 0x188A) >= 0x400) {
            MODEL_VARIANT_HALF(work, 0x188A) = 0x400;
            MODEL_VARIANT_WORD(work, 0x189C) = 4;
        }
    }
}
