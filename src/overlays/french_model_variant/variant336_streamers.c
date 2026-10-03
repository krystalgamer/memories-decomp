#include "../../types.h"
#include "../model_variant/variant321_streamers.h"

void func_8013CB6C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    POLY_G4 *poly;
    GsOT *ot;
    s16 i;
    s16 size;
    s16 rx;
    s16 ry;
    s32 step;
    s32 base;
    s32 spin;
    s32 wave;
    s32 turn;
    s32 radius;
    Variant321Streamer *streamer;
    s16 k;
    s32 twist;
    s32 bulge;
    s32 length;
    s32 wobble;
    s32 phase;
    s16 reach;
    s32 dx;
    s32 dy;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_G4 *)(work + 0x1EC0);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x1FB4), MODEL_VARIANT_WORD(work, 0x1FAC)) + 0xC00;
    reach = 0x80;
    step = 0x400;
    if (!(MODEL_VARIANT_WORD(work, 0x1FC4) & 1)) {
        size = 0x1000;
        rx = 0x200;
        ry = 0;
    } else {
        size = 0x1000;
        rx = 0x600;
        ry = step;
    }
    if ((u32)(MODEL_VARIANT_WORD(work, 0x1FC4) & 3) < 2) {
        phase = 0;
    } else {
        phase = 0x5DC;
    }
    streamer = (Variant321Streamer *)(work + 0x13C0);
    i = 0;
    base = MODEL_VARIANT_WORD(work, 0x2018);
    spin = phase;
    radius = reach;
    for (; i < 2; i++, streamer++, base += step) {
        for (k = 0, wave = 0, twist = base * 2; k < 17;
             k++, twist = base * 2 + k * 1500 / 16, spin += 0x5DC, wave += 0x200) {
            bulge = rcos(wave) * 16 >> 12;
            wobble = bulge + (rcos(spin) * 8 >> 12);
            length = MODEL_VARIANT_WORD(work, 0x1FFC) / 64;
            setVector(&streamer->a[k], bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                      bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                      rsin(twist) * (radius + (s16)wobble) >> 12);
            setVector(&streamer->b[k], streamer->a[k].vx + (rcos(turn) * (s16)length >> 12), streamer->a[k].vy,
                      streamer->a[k].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    rot.vx = MODEL_VARIANT_HALF(work, 0x2014) + rx;
    rot.vy = MODEL_VARIANT_HALF(work, 0x2018) + ry;
    rot.vz = MODEL_VARIANT_HALF(work, 0x201C);
    if ((u32)MODEL_VARIANT_WORD(work, 0x1FC4) % 10 == 0) {
        MODEL_VARIANT_WORD(work, 0x2018) += 0x514;
    }
    MODEL_VARIANT_WORD(work, 0x2018) += 0x20;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x1F34);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x1F38);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x1F3C);
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
    streamer = (Variant321Streamer *)(work + 0x13C0);
    for (i = 0; i < 2; i++, streamer++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                streamer->otz[k] = RotTransPers4(&streamer->a[15], &streamer->a[k], &streamer->a[15], &streamer->a[k],
                                               &streamer->sa[15], &streamer->sa[k],
                                               &streamer->sa[15], &streamer->sa[k], &p, &streamer->flag[k]);
                RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                dx = (s16)streamer->sa[k] - (s16)streamer->sa[15];
                dy = (streamer->sa[k] >> 16) - (streamer->sa[15] >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                if (MODEL_VARIANT_WORD(work, 0x1FFC) > 0x200) {
                    streamer->width[k] = 2;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            } else {
                streamer->otz[k] = RotTransPers4(&streamer->a[k], &streamer->a[k + 1], &streamer->a[k], &streamer->a[k + 1],
                                               &streamer->sa[k], &streamer->sa[k + 1],
                                               &streamer->sa[k], &streamer->sa[k + 1], &p, &streamer->flag[k]);
                RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                dx = (s16)streamer->sa[k + 1] - (s16)streamer->sa[k];
                dy = (streamer->sa[k + 1] >> 16) - (streamer->sa[k] >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                if (MODEL_VARIANT_WORD(work, 0x1FFC) > 0x200) {
                    streamer->width[k] = 2;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            }
        }
    }
    streamer = (Variant321Streamer *)(work + 0x13C0);
    for (i = 0; i < 2; i++, streamer++) {
        for (k = 0; k < 16; k++) {
            poly->x0 = streamer->sa[k] + streamer->ox[k];
            poly->y0 = (streamer->sa[k] >> 16) + streamer->oy[k];
            poly->x1 = streamer->sa[k + 1] + streamer->ox[k + 1];
            poly->y1 = (streamer->sa[k + 1] >> 16) + streamer->oy[k + 1];
            poly->x2 = streamer->sa[k] - streamer->ox[k];
            poly->y2 = (streamer->sa[k] >> 16) - streamer->oy[k];
            poly->x3 = streamer->sa[k + 1] - streamer->ox[k + 1];
            poly->y3 = (streamer->sa[k + 1] >> 16) - streamer->oy[k + 1];
            poly->r0 = streamer->color[k][0];
            poly->g0 = streamer->color[k][1];
            poly->b0 = streamer->color[k][2];
            if (streamer->otz[k] >= 0 && streamer->flag[k] >= 0) {
                GsSortPoly(poly, ot, streamer->otz[k]);
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x2000) == 0 && MODEL_VARIANT_WORD(work, 0x1FFC) > 0) {
        MODEL_VARIANT_WORD(work, 0x1FFC) = 1024
            - ((((u32)MODEL_VARIANT_WORD(work, 0x1FC8)
                - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x1FD8), 0x10)) << 10)
               / ((u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x1FD8), 0x14)
                - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x1FD8), 0x10)));
        if (MODEL_VARIANT_WORD(work, 0x1FFC) <= 0) {
            MODEL_VARIANT_WORD(work, 0x1FFC) = 0;
            MODEL_VARIANT_WORD(work, 0x2000) = 1;
        }
    }
}
