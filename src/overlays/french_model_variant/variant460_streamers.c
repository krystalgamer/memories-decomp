#include "../../types.h"
#include "../model_variant/variant443_streamers.h"

void func_8013D0B4(u8 *ctx)
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
    Variant443Streamer *streamer;
    s16 k;
    s32 bulge;
    s32 length;
    s32 wobble;
    s32 phase;
    s16 reach;
    s32 dx;
    s32 dy;
    PSXLONG *previous;

    work = ctx;
    ot = func_80058F10();
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x2E58), MODEL_VARIANT_WORD(work, 0x2E50)) + 3072;
    poly = (POLY_G4 *)(work + 0x2BB4);
    reach = 160;
    step = 1024;
    if (!(MODEL_VARIANT_WORD(work, 0x2E68) & 1)) {
        size = 4096;
        rx = 512;
        ry = 0;
    } else {
        size = 4096;
        rx = 1536;
        ry = step;
    }
    if ((u32)(MODEL_VARIANT_WORD(work, 0x2E68) & 3) < 2) {
        phase = 0;
    } else {
        phase = 1500;
    }
    streamer = (Variant443Streamer *)(work + 0x23E8);
    i = 0;
    base = MODEL_VARIANT_WORD(work, 0x2EB4);
    spin = phase;
    radius = reach;
    for (; i < 2; i++, streamer++, base += step) {
        s32 twist;

        twist = base * 2;
        for (k = 0, wave = 0; k < 17;
             k++, twist = base * 2 + k * 1500 / 16, spin += 1500, wave += 512) {
            bulge = rcos(wave) * 16 >> 12;
            wobble = bulge + (rcos(spin) * 8 >> 12);
            length = MODEL_VARIANT_WORD(work, 0x2EAC) / 64;
            setVector(&streamer->a[k],
                bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                rsin(twist) * (radius + (s16)wobble) >> 12);
            setVector(&streamer->b[k],
                streamer->a[k].vx + (rcos(turn) * (s16)length >> 12), streamer->a[k].vy,
                streamer->a[k].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    rot.vx = MODEL_VARIANT_HALF(work, 0x2EB0) + rx;
    rot.vy = MODEL_VARIANT_HALF(work, 0x2EB4) + ry;
    rot.vz = MODEL_VARIANT_HALF(work, 0x2EB8);
    if ((u32)MODEL_VARIANT_WORD(work, 0x2E68) % 10 == 0) {
        MODEL_VARIANT_WORD(work, 0x2EB4) += 1300;
    }
    MODEL_VARIANT_WORD(work, 0x2EB4) += 32;
    m.t[0] = MODEL_VARIANT_HALF(work, 0x2C34);
    m.t[1] = MODEL_VARIANT_HALF(work, 0x2C36);
    m.t[2] = MODEL_VARIANT_HALF(work, 0x2C38);
    setVector(&scale, size, size, size);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    streamer = (Variant443Streamer *)(work + 0x23E8);
    for (i = 0, previous = &streamer->sa[15]; i < 2; i++,
         previous = (PSXLONG *)((u8 *)previous + sizeof(Variant443Streamer)), streamer++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                streamer->otz[k] = RotTransPers4(
                    &streamer->a[15], &streamer->a[16], &streamer->a[15], &streamer->a[16],
                    previous, &streamer->sa[16], previous, &streamer->sa[16], &p, &flag);
                RotTransPers(&streamer->b[16], &streamer->sb[16], &p, &flag);
                dx = (s16)streamer->sa[k] - (s16)*previous;
                dy = (streamer->sa[k] >> 16) - (*previous >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 3072;
                if (MODEL_VARIANT_WORD(work, 0x2EAC) > 512) {
                    streamer->width[k] = 2;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            } else {
                streamer->otz[k] = RotTransPers4(
                    &streamer->a[k], &streamer->a[k + 1], &streamer->a[k], &streamer->a[k + 1],
                    &streamer->sa[k], &streamer->sa[k + 1], &streamer->sa[k], &streamer->sa[k + 1], &p, &flag);
                RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                dx = (s16)streamer->sa[k + 1] - (s16)streamer->sa[k];
                dy = (streamer->sa[k + 1] >> 16) - (streamer->sa[k] >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 3072;
                if (MODEL_VARIANT_WORD(work, 0x2EAC) > 512) {
                    streamer->width[k] = 2;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            }
        }
    }
    streamer = (Variant443Streamer *)(work + 0x23E8);
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
                if (streamer->otz[k] < 2048) {
                    GsSortPoly(poly, ot, (u16)streamer->otz[k]);
                }
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x2EC4) == 4 && MODEL_VARIANT_WORD(work, 0x2EAC) > 0) {
        MODEL_VARIANT_WORD(work, 0x2EAC) -= MODEL_VARIANT_WORD(work, 0x2E74) << 4;
        if (MODEL_VARIANT_WORD(work, 0x2EAC) <= 0) {
            MODEL_VARIANT_WORD(work, 0x2EAC) = 0;
            MODEL_VARIANT_WORD(work, 0x2EC4) = 5;
        }
    }
}
