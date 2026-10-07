#include "../../types.h"

#include "../model_variant/variant472_spiral.h"

/* Twelve-arm spiral of four-point spines: the arms are built on a spiral of
 * radius and spread chosen by phase, leaned along the direction, moved along
 * the view, and drawn as two POLY_GT4 halves with per-point inner and outer
 * colours where the depth and the arm's projection flag are not negative. The
 * sweep advances by 16; the size grows over the timing record and fades out
 * after phase 0. */
void func_8013CD30(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    PSXLONG *previous;
    u8 *work;
    GsOT *ot;
    s16 i;
    s16 length;
    s32 phi;
    s32 theta;
    s32 r;
    s32 ux;
    s32 uy;
    s32 uz;
    s32 spread;
    s32 turn;
    s32 eighth;
    Variant472SpiralArm *arm;
    POLY_GT4 *poly;
    s16 k;
    s16 size;
    s32 dx;
    s32 dy;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_GT4 *)(work + 0x2C14);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x2ED8), MODEL_VARIANT_WORD(work, 0x2ED0)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x2F40) < 3) {
        spread = 0x400;
        r = MODEL_VARIANT_WORD(work, 0x2F1C) / 2;
    } else {
        r = 0x200;
        spread = MODEL_VARIANT_WORD(work, 0x2F1C);
    }
    ux = MODEL_VARIANT_WORD(work, 0x2EBC) * MODEL_VARIANT_WORD(work, 0x2F1C) / 1024;
    uy = MODEL_VARIANT_WORD(work, 0x2EC0) * MODEL_VARIANT_WORD(work, 0x2F1C) / 1024;
    uz = MODEL_VARIANT_WORD(work, 0x2EC4) * MODEL_VARIANT_WORD(work, 0x2F1C) / 1024;
    eighth = MODEL_VARIANT_WORD(work, 0x2F1C) / 8;
    if (r * MODEL_VARIANT_WORD(work, 0x2F28) < 0) {
        length = 0;
    }
    if (eighth * MODEL_VARIANT_WORD(work, 0x2F28) < 0) {
        length = 0;
    }
    arm = (Variant472SpiralArm *)(work + 0x1EF4);
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + MODEL_VARIANT_WORD(work, 0x2F20) * 2;
        theta = (i << 13) / 12 + MODEL_VARIANT_WORD(work, 0x2F20);
        for (k = 0; k < 4; k++) {
            length = (k * 32 / 3 + 8) * spread / 1024;
            if (k == 0) {
                setVector(&arm->a[0], 0, 0, 0);
            } else {
                setVector(&arm->a[k], (rcos(phi) * (rsin(theta) * (r * k / 3) >> 12) >> 12) + ux * k / 3,
                          (rcos(theta) * (r * k / 3) >> 12) + uy * k / 3,
                          (rsin(phi) * (rsin(theta) * (r * k / 3) >> 12) >> 12) + uz * k / 3);
            }
            setVector(&arm->b[k], arm->a[k].vx + (rcos(turn) * length >> 12), arm->a[k].vy,
                      arm->a[k].vz + (rsin(turn) * length >> 12));
        }
    }
    if (!(MODEL_VARIANT_WORD(work, 0x2EE8) & 1)) {
        size = 0x1000;
    } else {
        size = 0x1200;
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x2E98);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x2E9C);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x2EA0);
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
    arm = (Variant472SpiralArm *)(work + 0x1EF4);
    for (i = 0, previous = &arm->sa[2]; i < 12; i++,
         previous = (PSXLONG *)((u8 *)previous + sizeof(Variant472SpiralArm)), arm++) {
        for (k = 0; k < 4; k++) {
            if (k == 3) {
                arm->otz[k] = RotTransPers4(&arm->a[2], &arm->a[3], &arm->a[2], &arm->a[3],
                                            previous, &arm->sa[3], previous, &arm->sa[3], &p, &arm->flag[3]);
                RotTransPers(&arm->b[3], &arm->sb[3], &p, &flag);
                dx = (s16)arm->sa[k] - (s16)*previous;
                dy = (arm->sa[k] >> 16) - (*previous >> 16);
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
    arm = (Variant472SpiralArm *)(work + 0x1EF4);
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 3; k++) {
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
            if (arm->otz[k] >= 0 && arm->flag[k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k]);
            }
        }
    }
    MODEL_VARIANT_WORD(work, 0x2F20) += 16;
    if (MODEL_VARIANT_WORD(work, 0x2F40) == 0) {
        if (MODEL_VARIANT_WORD(work, 0x2F1C) < 0x400) {
            MODEL_VARIANT_WORD(work, 0x2F1C) = (u32)((MODEL_VARIANT_WORD(work, 0x2EEC) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2EFC), 0x1C)) << 10) /
                                               (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2EFC), 0x20) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2EFC), 0x1C));
            if (MODEL_VARIANT_WORD(work, 0x2F1C) >= 0x400) {
                MODEL_VARIANT_WORD(work, 0x2F1C) = 0x400;
            }
        }
    } else if ((u32)MODEL_VARIANT_WORD(work, 0x2EEC) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2EFC), 0x28) &&
               MODEL_VARIANT_WORD(work, 0x2F1C) > 0) {
        MODEL_VARIANT_WORD(work, 0x2F1C) = 0x400 - (u32)((MODEL_VARIANT_WORD(work, 0x2EEC) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2EFC), 0x28)) << 10) /
                                           (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2EFC), 0x2C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x2EFC), 0x28));
        if (MODEL_VARIANT_WORD(work, 0x2F1C) <= 0) {
            MODEL_VARIANT_WORD(work, 0x2F1C) = 0;
        }
    }
}
