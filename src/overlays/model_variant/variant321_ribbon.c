#include "../../types.h"

#include "variant321_ribbon.h"

/* Builds five seventeen-point ribbons, each turned by its own angle and bent
 * around the variant's axis, plus a copy of each moved along the view; projects
 * both and draws the first `count` segments of each ribbon as flat POLY_FT4
 * quads as wide as the projected offset, with straight first and last ends.
 * The drawn length grows by 1.5 steps a frame up to 16; in phase 3 the view
 * offset shrinks with the timing record's clock. */
void func_8013BBA4(u8 *ctx)
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
    s32 wave;
    s32 phase;
    s32 turn;
    s32 yaw;
    s32 pitch;
    s32 start;
    s32 now;
    s16 len;
    Variant321Ribbon *ribbon;
    POLY_FT4 *poly;
    s16 k;
    s32 bend;
    s32 coil;
    s32 dx;
    s32 dy;

    work = ctx;
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x1F18), MODEL_VARIANT_WORD(work, 0x1F10)) + 0xC00;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x1F14), MODEL_VARIANT_WORD(work, 0x1F10)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x1F68) > 0) {
        len = MODEL_VARIANT_HALF(work, 0x1F4E) / 32;
        ribbon = (Variant321Ribbon *)work;
        for (i = 0, phase = 0; i < 5; i++, ribbon++, phase = i * 0x1800 / 5) {
            if (i == 0) {
                turn = 0x400;
            } else if (i % 2 == 1) {
                turn = i * 360 + 0x400;
            } else {
                turn = 0x400 - i * 360;
            }
            for (k = 0, wave = -MODEL_VARIANT_WORD(work, 0x1F60); k < 17; k++, wave += 0x514) {
                if (k == 0 || k == 16) {
                    bend = 0;
                } else {
                    bend = phase - k * 384 + MODEL_VARIANT_WORD(work, 0x1F64);
                }
                coil = k * 128;
                setVector(&ribbon->a[k],
                          MODEL_VARIANT_WORD(work, 0x1EFC) * k / 16 - (rcos(turn) * (rsin(coil) * 384 >> 12) >> 12) +
                              (rcos(pitch) * (rsin(bend) * ((rsin(wave) * 64 >> 12) + 0x40) >> 12) >> 12),
                          MODEL_VARIANT_WORD(work, 0x1F00) * k / 16 - (rsin(turn) * (rsin(coil) * 384 >> 12) >> 12) +
                              (rsin(pitch) * (rsin(bend) * ((rsin(wave) * 64 >> 12) + 0x40) >> 12) >> 12),
                          MODEL_VARIANT_WORD(work, 0x1F04) * k / 16);
                setVector(&ribbon->b[k], ribbon->a[k].vx + (rcos(yaw) * len >> 12), ribbon->a[k].vy,
                          ribbon->a[k].vz + (rsin(yaw) * len >> 12));
            }
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work, 0x1ED8);
        m.t[1] = MODEL_VARIANT_WORD(work, 0x1EDC);
        m.t[2] = MODEL_VARIANT_WORD(work, 0x1EE0);
        scale.vx = 0x1000;
        scale.vy = 0x1000;
        scale.vz = 0x1000;
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        ribbon = (Variant321Ribbon *)work;
        for (i = 0; i < 5; i++, ribbon++) {
            for (k = 0; k < 17; k++) {
                if (k == 16) {
                    ribbon->otz[16] = RotTransPers4(&ribbon->a[15], &ribbon->a[16], &ribbon->a[15], &ribbon->a[16],
                                                    &ribbon->sa[15], &ribbon->sa[16], &ribbon->sa[15], &ribbon->sa[16],
                                                    &p, &ribbon->flag[16]);
                    RotTransPers(&ribbon->b[16], &ribbon->sb[16], &p, &flag);
                    dx = (s16)ribbon->sa[16] - (s16)ribbon->sa[15];
                    dy = (ribbon->sa[16] >> 16) - (ribbon->sa[15] >> 16);
                    ribbon->angle[16] = ratan2(dy, dx) - 0x400;
                    ribbon->width[16] = (s16)ribbon->sb[16] - (s16)ribbon->sa[16];
                    ribbon->ox[16] = rcos(ribbon->angle[16]) * ribbon->width[16] >> 12;
                    ribbon->oy[16] = rsin(ribbon->angle[16]) * ribbon->width[16] >> 12;
                } else {
                    ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                                   &ribbon->sa[k], &ribbon->sa[k + 1], &ribbon->sa[k], &ribbon->sa[k + 1],
                                                   &p, &ribbon->flag[k]);
                    RotTransPers(&ribbon->b[k], &ribbon->sb[k], &p, &flag);
                    dx = (s16)ribbon->sa[k + 1] - (s16)ribbon->sa[k];
                    dy = (ribbon->sa[k + 1] >> 16) - (ribbon->sa[k] >> 16);
                    ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                    ribbon->width[k] = (s16)ribbon->sb[k] - (s16)ribbon->sa[k];
                    ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                }
            }
        }
        ribbon = (Variant321Ribbon *)work;
        for (i = 0; i < 5; i++, ribbon++) {
            k = 0;
            poly = (POLY_FT4 *)(work + 0x1E14);
            for (; k < ribbon->count; k++) {
                if ((u32)(MODEL_VARIANT_WORD(work, 0x1F28) & 1) == 1) {
                    if (!(k & 1)) {
                        poly++;
                    } else {
                        poly--;
                    }
                }
                if (k == 0) {
                    poly->x0 = ribbon->sa[0];
                    poly->y0 = ribbon->sa[0] >> 16;
                    poly->x1 = ribbon->sa[1] + ribbon->ox[1];
                    poly->y1 = (ribbon->sa[1] >> 16) + ribbon->oy[1];
                    poly->x2 = ribbon->sa[0];
                    poly->y2 = ribbon->sa[0] >> 16;
                    poly->x3 = ribbon->sa[1] - ribbon->ox[1];
                    poly->y3 = (ribbon->sa[1] >> 16) - ribbon->oy[1];
                }
                if (k + 1 == ribbon->count) {
                    poly->x0 = ribbon->sa[k] + ribbon->ox[k];
                    poly->y0 = (ribbon->sa[k] >> 16) + ribbon->oy[k];
                    poly->x1 = ribbon->sa[k + 1];
                    poly->y1 = ribbon->sa[k + 1] >> 16;
                    poly->x2 = ribbon->sa[k] - ribbon->ox[k];
                    poly->y2 = (ribbon->sa[k] >> 16) - ribbon->oy[k];
                    poly->x3 = ribbon->sa[k + 1];
                    poly->y3 = ribbon->sa[k + 1] >> 16;
                } else {
                    poly->x0 = ribbon->sa[k] + ribbon->ox[k];
                    poly->y0 = (ribbon->sa[k] >> 16) + ribbon->oy[k];
                    poly->x1 = ribbon->sa[k + 1] + ribbon->ox[k + 1];
                    poly->y1 = (ribbon->sa[k + 1] >> 16) + ribbon->oy[k + 1];
                    poly->x2 = ribbon->sa[k] - ribbon->ox[k];
                    poly->y2 = (ribbon->sa[k] >> 16) - ribbon->oy[k];
                    poly->x3 = ribbon->sa[k + 1] - ribbon->ox[k + 1];
                    poly->y3 = (ribbon->sa[k + 1] >> 16) - ribbon->oy[k + 1];
                }
                poly->r0 = ribbon->color[0];
                poly->g0 = ribbon->color[1];
                poly->b0 = ribbon->color[2];
                if (ribbon->otz[k] >= 0 && ribbon->flag[k] >= 0) {
                    GsSortPoly(poly, ot, ribbon->otz[k]);
                }
                if (!(MODEL_VARIANT_WORD(work, 0x1F28) & 1)) {
                    if (!(k & 1)) {
                        poly++;
                    } else {
                        poly--;
                    }
                }
            }
            if (ribbon->count < 0x11) {
                ribbon->count += (u32)(MODEL_VARIANT_WORD(work, 0x1F34) * 3) >> 1;
                if (ribbon->count >= 0x10) {
                    ribbon->count = 0x10;
                    if (i == 0 && MODEL_VARIANT_WORD(work, 0x1F68) == 1) {
                        MODEL_VARIANT_WORD(work, 0x1F68) = 2;
                    }
                }
            }
        }
        if (MODEL_VARIANT_WORD(work, 0x1F68) == 3 && MODEL_VARIANT_HALF(work, 0x1F4E) > 0) {
            start = *(u32 *)(*(u8 *G32 *)(work + 0x1F3C) + 0x2C);
            now = MODEL_VARIANT_WORD(work, 0x1F2C);
            if ((u32)now >= (u32)start) {
                MODEL_VARIANT_HALF(work, 0x1F4E) =
                    0x400 - ((u32)(now - start) << 10) / (u32)(*(u32 *)(*(u8 *G32 *)(work + 0x1F3C) + 0x30) - start);
            }
            if (MODEL_VARIANT_HALF(work, 0x1F4E) <= 0) {
                MODEL_VARIANT_HALF(work, 0x1F4E) = 0;
            }
        }
    }
    MODEL_VARIANT_WORD(work, 0x1F60) += MODEL_VARIANT_WORD(work, 0x1F34) * 850;
    MODEL_VARIANT_WORD(work, 0x1F64) += MODEL_VARIANT_WORD(work, 0x1F34) << 7;
}
