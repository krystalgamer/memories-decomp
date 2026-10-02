#include "../../types.h"
#include "../model_variant/variant418_spiral.h"

void func_8013C70C(u8 *ctx)
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
    s32 turn;
    Variant418SpiralArm *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 theta;
    s16 length;
    s16 size;
    s32 dx;
    s32 dy;
    s16 inner_rg, inner_blue;
    s16 outer_rg, outer_blue;

    work = ctx;
    arm = (Variant418SpiralArm *)(work + 0x720);
    ot = func_80058F10();
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x279C), MODEL_VARIANT_WORD(work, 0x2794)) + 0xC00;
    poly = (POLY_GT4 *)(work + 0x22B4);
    for (i = 0; i < 16; i++, arm++) {
        if (!(i & 1)) {
            phi = (i << 8) + MODEL_VARIANT_WORD(work, 0x27F0);
            theta = (i << 9) + MODEL_VARIANT_WORD(work, 0x27F0);
        } else {
            phi = (i << 8) - MODEL_VARIANT_WORD(work, 0x27F0);
            theta = (i << 9) + MODEL_VARIANT_WORD(work, 0x27F0);
        }
        for (k = 0; k < 2; k++) {
            if (k == 0) {
                length = MODEL_VARIANT_WORD(work, 0x27EC) / 64;
            } else {
                length = MODEL_VARIANT_WORD(work, 0x27EC) / 16;
            }
            if (k == 0) {
                setVector(&arm->a[0], 0, 0, 0);
            } else {
                setVector(&arm->a[k], (rcos(phi) * ((rsin(theta) << 10) * k >> 12) >> 12),
                          ((rcos(theta) << 10) * k >> 12),
                          (rsin(phi) * ((rsin(theta) << 10) * k >> 12) >> 12));
            }
            setVector(&arm->b[k], arm->a[k].vx + (rcos(turn) * length >> 12), arm->a[k].vy,
                      arm->a[k].vz + (rsin(turn) * length >> 12));
        }
    }
    if (!(MODEL_VARIANT_WORD(work, 0x27AC) & 1)) {
        size = MODEL_VARIANT_WORD(work, 0x27E4);
    } else {
        size = MODEL_VARIANT_WORD(work, 0x27E4) + MODEL_VARIANT_WORD(work, 0x27E4) / 8;
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x274C);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x2750);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x2754);
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
    arm = (Variant418SpiralArm *)(work + 0x720);
    for (i = 0; i < 16; i++, arm++) {
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
    arm = (Variant418SpiralArm *)(work + 0x720);
    i = 0;
    inner_rg = 64;
    inner_blue = 0;
    outer_rg = 160;
    outer_blue = 128;
    for (; i < 16; i++, arm++) {
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
                setRGB0(poly, inner_rg, inner_rg, inner_blue);
                setRGB1(poly, 0, 0, 0);
                setRGB2(poly, outer_rg, outer_rg, outer_blue);
                setRGB3(poly, 0, 0, 0);
            } else {
                setRGB0(poly, inner_rg, inner_rg, inner_blue);
                setRGB1(poly, inner_rg, inner_rg, inner_blue);
                setRGB2(poly, outer_rg, outer_rg, outer_blue);
                setRGB3(poly, outer_rg, outer_rg, outer_blue);
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
                setRGB0(poly, inner_rg, inner_rg, inner_blue);
                setRGB1(poly, 0, 0, 0);
                setRGB2(poly, outer_rg, outer_rg, outer_blue);
                setRGB3(poly, 0, 0, 0);
            } else {
                setRGB0(poly, inner_rg, inner_rg, inner_blue);
                setRGB1(poly, inner_rg, inner_rg, inner_blue);
                setRGB2(poly, outer_rg, outer_rg, outer_blue);
                setRGB3(poly, outer_rg, outer_rg, outer_blue);
            }
            if (arm->otz[k] >= 0 && arm->flag[k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k]);
            }
        }
    }
    if ((u32)MODEL_VARIANT_WORD(work, 0x27B0) >=
        (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x27C0), 0x20) &&
        MODEL_VARIANT_WORD(work, 0x27E4) < 4096) {
        MODEL_VARIANT_WORD(work, 0x27E4) =
            (u32)((MODEL_VARIANT_WORD(work, 0x27B0) -
                   MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x27C0), 0x20)) << 12) /
            (u32)(MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x27C0), 0x24) -
                  MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x27C0), 0x20));
        if (MODEL_VARIANT_WORD(work, 0x27E4) >= 4096) {
            MODEL_VARIANT_WORD(work, 0x27E4) = 4096;
        }
    }
    if ((u32)MODEL_VARIANT_WORD(work, 0x27B0) >=
        (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x27C0), 0x24) &&
        MODEL_VARIANT_WORD(work, 0x27EC) > 0) {
        MODEL_VARIANT_WORD(work, 0x27EC) = 1024 -
            (u32)((MODEL_VARIANT_WORD(work, 0x27B0) -
                   MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x27C0), 0x24)) << 10) /
            (u32)(MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x27C0), 0x28) -
                  MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x27C0), 0x24));
        if (MODEL_VARIANT_WORD(work, 0x27EC) <= 0) {
            MODEL_VARIANT_WORD(work, 0x27EC) = 0;
        }
    }
    MODEL_VARIANT_WORD(work, 0x27F0) += 48;
}
