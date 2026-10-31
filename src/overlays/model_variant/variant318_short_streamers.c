#include "../../types.h"

#include "variant458_streamers.h"

/* Builds six seventeen-point streamers that fan out around the variant's
 * axis, projects each spine and a copy of it moved along the view, and draws
 * the segments between the start and end counts at 0x2E94 and 0x2E96 as flat
 * POLY_FT4 quads. The end count then grows to sixteen, after which the start
 * count follows it and both reset once it passes sixteen. Nothing in the image
 * calls this helper. */
void func_8013D8D0(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    POLY_FT4 *poly;
    GsOT *ot;
    s16 i;
    s32 base;
    s32 phase;
    s32 spin;
    s32 wave;
    s32 turn;
    Variant458Streamer *streamer;
    s16 k;
    s32 bulge;
    s32 wobble;
    s32 length;
    s32 dx;
    s32 dy;
    s32 twist;

    work = ctx;
    ot = func_80058F10();
    streamer = (Variant458Streamer *)(work + 0x1214);
    poly = (POLY_FT4 *)(work + 0x2D18);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x2E3C), MODEL_VARIANT_WORD(work, 0x2E34)) + 0xC00;
    if ((u32)(MODEL_VARIANT_WORD(work, 0x2E4C) & 3) < 2) {
        phase = 0;
    } else {
        phase = 0x5DC;
    }
    for (i = 0, base = 0, spin = phase; i < 6; i++, streamer++, base = i * 4096 / 6) {
        /* twist is never read. Its signed division keeps an extra branch in
         * the loop step until jump optimisation, which the register allocation
         * of the sign-extended counter needs. */
        for (k = 0, wave = 0, twist = base * 2; k < 17;
             k++, twist = base * 2 + k * 1500 / 16, spin += 0x5DC, wave += 0x200) {
            bulge = rcos(wave) * 16 >> 12;
            wobble = bulge + (rcos(spin) * 8 >> 12);
            length = MODEL_VARIANT_WORD(work, 0x2E88) / 64;
            setVector(&streamer->a[k], (s32)((u32)rcos(base) << 5) * k >> 12, wobble, (s32)((u32)rsin(base) << 5) * k >> 12);
            setVector(&streamer->b[k], streamer->a[k].vx + (rcos(turn) * (s16)length >> 12), streamer->a[k].vy,
                      streamer->a[k].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    setVector(&rot, 0, 0, 0);
    m.t[0] = MODEL_VARIANT_HALF(work, 0x2E18);
    m.t[1] = MODEL_VARIANT_HALF(work, 0x2E1A);
    m.t[2] = MODEL_VARIANT_HALF(work, 0x2E1C);
    setVector(&scale, 4096, 4096, 4096);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    streamer = (Variant458Streamer *)(work + 0x1214);
    for (i = 0; i < 6; i++, streamer++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                streamer->otz[k] = RotTransPers4(&streamer->a[15], &streamer->a[k], &streamer->a[15], &streamer->a[k],
                                                 &streamer->sa[15], &streamer->sa[k],
                                                 &streamer->sa[15], &streamer->sa[k], &p, &flag);
                RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                dx = (s16)streamer->sa[k] - (s16)streamer->sa[15];
                dy = (streamer->sa[k] >> 16) - (streamer->sa[15] >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                if (MODEL_VARIANT_WORD(work, 0x2E88) > 0x200) {
                    streamer->width[k] = 2;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            } else {
                streamer->otz[k] = RotTransPers4(&streamer->a[k], &streamer->a[k + 1], &streamer->a[k], &streamer->a[k + 1],
                                                 &streamer->sa[k], &streamer->sa[k + 1],
                                                 &streamer->sa[k], &streamer->sa[k + 1], &p, &flag);
                RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                dx = (s16)streamer->sa[k + 1] - (s16)streamer->sa[k];
                dy = (streamer->sa[k + 1] >> 16) - (streamer->sa[k] >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                if (MODEL_VARIANT_WORD(work, 0x2E88) > 0x200) {
                    streamer->width[k] = 2;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            }
        }
    }
    streamer = (Variant458Streamer *)(work + 0x1214);
    for (i = 0; i < 6; i++, streamer++) {
        for (k = MODEL_VARIANT_HALF(work, 0x2E94); k < MODEL_VARIANT_HALF(work, 0x2E96); k++) {
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
            if (streamer->otz[k] > 0) {
                if (streamer->otz[k] < 2048) {
                    GsSortPoly(poly, ot, streamer->otz[k]);
                }
            }
        }
    }
    if (MODEL_VARIANT_HALF(work, 0x2E96) < 16) {
        MODEL_VARIANT_HALF(work, 0x2E96) += 2;
        if (MODEL_VARIANT_HALF(work, 0x2E96) >= 16) {
            MODEL_VARIANT_HALF(work, 0x2E96) = 16;
        }
    } else if (MODEL_VARIANT_HALF(work, 0x2E94) < 17) {
        MODEL_VARIANT_HALF(work, 0x2E94) += 2;
        if (MODEL_VARIANT_HALF(work, 0x2E94) >= 17) {
            MODEL_VARIANT_HALF(work, 0x2E94) = 0;
            MODEL_VARIANT_HALF(work, 0x2E96) = 0;
        }
    }
}
