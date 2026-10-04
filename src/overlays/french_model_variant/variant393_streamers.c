#include "../../types.h"

#include "../model_variant/variant376_streamers.h"

/* Builds two seventeen-point streamers that coil around the variant's axis,
 * projects each spine and a copy of it moved along the view, and draws each
 * segment as a POLY_G4 as wide as the projected offset. Once the fourth phase
 * starts, the streamers shrink by step * 12 until they vanish. */
void func_8013CB8C(u8 *ctx)
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
    Variant376Streamer *streamer;
    s16 k;
    s32 twist;
    s32 bulge;
    s32 r;
    s32 length;
    s32 wobble;
    s32 w;
    s32 phase;
    s16 reach;
    s32 dx;
    s32 dy;
    PSXLONG *previous;

    work = ctx;
    ot = func_80058F10();
    poly = (POLY_G4 *)(work + 0xCBC);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0xD90), MODEL_VARIANT_WORD(work, 0xD88)) + 0xC00;
    reach = 0x180;
    step = 0x400;
    if (!(MODEL_VARIANT_WORD(work, 0xDA0) & 1)) {
        size = 0x1000;
        rx = 0x200;
        ry = 0;
    } else {
        size = 0x1000;
        rx = 0x600;
        ry = step;
    }
    if ((u32)(MODEL_VARIANT_WORD(work, 0xDA0) & 3) < 2) {
        phase = 0;
    } else {
        phase = 0x5DC;
    }
    streamer = (Variant376Streamer *)(work + 0x4D4);
    i = 0;
    base = MODEL_VARIANT_WORD(work, 0xDE0);
    spin = phase;
    radius = reach;
    for (; i < 2; i++, streamer++, base += step) {
        for (k = 0, wave = 0, twist = base * 2; k < 17;
             k++, twist = base * 2 + k * 1500 / 16, spin += 0x5DC, wave += 0x200) {
            bulge = rcos(wave) * 16 >> 12;
            wobble = bulge + (rcos(spin) * 8 >> 12);
            length = MODEL_VARIANT_WORD(work, 0xDD8) / 64;
            setVector(&streamer->a[k], bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                      bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                      rsin(twist) * (radius + (s16)wobble) >> 12);
            setVector(&streamer->b[k], streamer->a[k].vx + (rcos(turn) * (s16)length >> 12), streamer->a[k].vy,
                      streamer->a[k].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    rot.vx = MODEL_VARIANT_HALF(work, 0xDDC) + rx;
    rot.vy = MODEL_VARIANT_HALF(work, 0xDE0) + ry;
    rot.vz = MODEL_VARIANT_HALF(work, 0xDE4);
    if ((u32)MODEL_VARIANT_WORD(work, 0xDA0) % 10 == 0) {
        MODEL_VARIANT_WORD(work, 0xDE0) += 0x514;
    }
    MODEL_VARIANT_WORD(work, 0xDE0) += 0x20;
    m.t[0] = MODEL_VARIANT_HALF(work, 0xD3C);
    m.t[1] = MODEL_VARIANT_HALF(work, 0xD3E);
    m.t[2] = MODEL_VARIANT_HALF(work, 0xD40);
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
    streamer = (Variant376Streamer *)(work + 0x4D4);
    for (i = 0, previous = &streamer->sa[15]; i < 2; i++,
         previous = (PSXLONG *)((u8 *)previous + sizeof(Variant376Streamer)), streamer++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                streamer->otz[k] = RotTransPers4(&streamer->a[15], &streamer->a[16], &streamer->a[15], &streamer->a[16],
                                                  previous, &streamer->sa[16],
                                                  previous, &streamer->sa[16], &p, &streamer->flag[16]);
                RotTransPers(&streamer->b[16], &streamer->sb[16], &p, &flag);
                dx = (s16)streamer->sa[k] - (s16)*previous;
                dy = (streamer->sa[k] >> 16) - (*previous >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                if (MODEL_VARIANT_WORD(work, 0xDD8) > 0x200) {
                    streamer->width[k] = 3;
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
                if (MODEL_VARIANT_WORD(work, 0xDD8) > 0x200) {
                    streamer->width[k] = 3;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            }
        }
    }
    streamer = (Variant376Streamer *)(work + 0x4D4);
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
            if (streamer->otz[k] >= 0) {
                if (streamer->flag[k] >= 0) {
                    GsSortPoly(poly, ot, streamer->otz[k]);
                }
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0xDF8) == 4 && MODEL_VARIANT_WORD(work, 0xDD8) > 0) {
        MODEL_VARIANT_WORD(work, 0xDD8) -= MODEL_VARIANT_WORD(work, 0xDAC) * 12;
        if (MODEL_VARIANT_WORD(work, 0xDD8) <= 0) {
            MODEL_VARIANT_WORD(work, 0xDD8) = 0;
            MODEL_VARIANT_WORD(work, 0xDF8) = 5;
        }
    }
}
