#include "../../types.h"

#include "../model_variant/variant458_streamers.h"

/* Builds four seventeen-point streamers that coil around the variant's axis,
 * projects each spine and a copy of it moved along the view, and draws each
 * segment as a POLY_G4 as wide as the projected offset (3 once the length
 * passes 0x200). RotTransPers4 writes each point's flag into a local table;
 * a segment is drawn only when its depth and flag are non-negative and the
 * word at work + 0x2128 has reached 0x1000. From the fifth phase on, the
 * streamer length is reset from the word at work + 0x2138. */
void func_8013DBC0(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG depth[4][17];
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
    Variant458Streamer *streamer;
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
    poly = (POLY_G4 *)(work + 0x2048);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x20EC), MODEL_VARIANT_WORD(work, 0x20E4)) + 0xC00;
    reach = 0x240;
    step = 0x400;
    if (!(MODEL_VARIANT_WORD(work, 0x20FC) & 1)) {
        size = 0x1000;
        rx = 0x200;
        ry = 0;
    } else {
        size = 0x1000;
        rx = 0x600;
        ry = step;
    }
    if ((u32)(MODEL_VARIANT_WORD(work, 0x20FC) & 3) < 2) {
        phase = 0;
    } else {
        phase = 0x5DC;
    }
    streamer = (Variant458Streamer *)(work + 0xF4C);
    i = 0;
    base = MODEL_VARIANT_WORD(work, 0x215C);
    spin = phase;
    radius = reach;
    for (; i < 4; i++, streamer++, base += step) {
        wave = 0;
        twist = base * 2;
        for (k = 0; k < 17;
             k++, twist = base * 2 + k * 1500 / 16, spin += 0x5DC, wave += 0x200) {
            bulge = rcos(wave) * 24 >> 12;
            wobble = bulge + (rcos(spin) * 16 >> 12);
            length = MODEL_VARIANT_WORD(work, 0x2154) / 64;
            setVector(&streamer->a[k], bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                      bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                      rsin(twist) * (radius + (s16)wobble) >> 12);
            setVector(&streamer->b[k], streamer->a[k].vx + (rcos(turn) * (s16)length >> 12), streamer->a[k].vy,
                      streamer->a[k].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    rot.vx = MODEL_VARIANT_HALF(work, 0x2158) + rx;
    rot.vy = MODEL_VARIANT_HALF(work, 0x215C) + ry;
    rot.vz = MODEL_VARIANT_HALF(work, 0x2160);
    if ((u32)MODEL_VARIANT_WORD(work, 0x20FC) % 10 == 0) {
        MODEL_VARIANT_WORD(work, 0x215C) += 0x514;
    }
    MODEL_VARIANT_WORD(work, 0x215C) += 0x20;
    m.t[0] = MODEL_VARIANT_HALF(work, 0x20C8);
    m.t[1] = MODEL_VARIANT_HALF(work, 0x20CA);
    m.t[2] = MODEL_VARIANT_HALF(work, 0x20CC);
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
    streamer = (Variant458Streamer *)(work + 0xF4C);
    for (i = 0; i < 4; i++, streamer++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                streamer->otz[k] = RotTransPers4(&streamer->a[15], &streamer->a[16], &streamer->a[15], &streamer->a[16],
                                                  &streamer->sa[15], &streamer->sa[16],
                                                  &streamer->sa[15], &streamer->sa[16], &p, &depth[i][k]);
                RotTransPers(&streamer->b[16], &streamer->sb[16], &p, &flag);
                dx = (s16)streamer->sa[k] - (s16)streamer->sa[15];
                dy = (streamer->sa[k] >> 16) - (streamer->sa[15] >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                if (MODEL_VARIANT_WORD(work, 0x2154) > 0x200) {
                    streamer->width[k] = 3;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            } else {
                streamer->otz[k] = RotTransPers4(&streamer->a[k], &streamer->a[k + 1], &streamer->a[k], &streamer->a[k + 1],
                                                 &streamer->sa[k], &streamer->sa[k + 1],
                                                 &streamer->sa[k], &streamer->sa[k + 1], &p, &depth[i][k]);
                RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                dx = (s16)streamer->sa[k + 1] - (s16)streamer->sa[k];
                dy = (streamer->sa[k + 1] >> 16) - (streamer->sa[k] >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                if (MODEL_VARIANT_WORD(work, 0x2154) > 0x200) {
                    streamer->width[k] = 3;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            }
        }
    }
    streamer = (Variant458Streamer *)(work + 0xF4C);
    for (i = 0; i < 4; i++, streamer++) {
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
            if (streamer->otz[k] >= 0 && depth[i][k] >= 0 && MODEL_VARIANT_WORD(work, 0x2128) >= 0x1000) {
                GsSortPoly(poly, ot, streamer->otz[k]);
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x2180) >= 5) {
        MODEL_VARIANT_WORD(work, 0x2154) = MODEL_VARIANT_WORD(work, 0x2138);
    }
}
