#include "../../types.h"

#include "variant376_ribbon.h"

/* Builds a seventeen-point ribbon that twists around the variant's axis and a
 * copy of it moved along the view, projects both, and draws the first
 * `work[0xDCC]` segments as flat POLY_FT4 quads as wide as the projected
 * offset. In phase 1 the drawn length grows by the frame step up to 16; later
 * the copy's offset shrinks with the timing record's progress. */
void func_8013BDE0(u8 *ctx)
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
    s32 yaw;
    s32 pitch;
    s32 twist;
    s32 start;
    s32 now;
    s16 len;
    Variant376Ribbon *ribbon;
    POLY_FT4 *poly;
    s16 k;
    s32 bend;
    s32 coil;
    s32 dx;
    s32 dy;

    work = ctx;
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0xD90), MODEL_VARIANT_WORD(work, 0xD88)) + 0xC00;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0xD8C), MODEL_VARIANT_WORD(work, 0xD88)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0xDF8) > 0) {
        twist = (MODEL_VARIANT_WORD(work, 0xDC8) << 12) / *(u16 *)(*(u8 *G32 *)(work + 0xDB4) + 0x1C) + yaw;
        len = MODEL_VARIANT_HALF(work, 0xDD2) * 40 / 1024;
        ribbon = (Variant376Ribbon *)work;
        for (i = 0, phase = 0; i < 1; i++, ribbon++, phase = i << 12) {
            for (k = 0, wave = phase - MODEL_VARIANT_WORD(work, 0xDE8); k < 17; k++, wave += 0x300) {
                if (k == 0 || k == 16) {
                    bend = 0;
                } else {
                    bend = phase - k * 384;
                }
                coil = k * 128 + phase;
                setVector(&ribbon->a[k],
                          MODEL_VARIANT_WORD(work, 0xD74) * k / 16 +
                              (rcos(pitch) * (rsin(coil) * ((rcos(twist) * 192 >> 12) +
                                                            (rsin(bend) * ((rsin(wave) * 96 >> 12) + 0x60) >> 12)) >> 12) >> 12),
                          MODEL_VARIANT_WORD(work, 0xD78) * k / 16 +
                              (rsin(pitch) * (rsin(coil) * ((rsin(twist) * 192 >> 12) +
                                                            (rsin(bend) * ((rsin(wave) * 96 >> 12) + 0x60) >> 12)) >> 12) >> 12),
                          MODEL_VARIANT_WORD(work, 0xD7C) * k / 16);
                setVector(&ribbon->b[k], ribbon->a[k].vx + (rcos(yaw) * len >> 12), ribbon->a[k].vy,
                          ribbon->a[k].vz + (rsin(yaw) * len >> 12));
            }
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work, 0xD30);
        m.t[1] = MODEL_VARIANT_WORD(work, 0xD34);
        m.t[2] = MODEL_VARIANT_WORD(work, 0xD38);
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
        ribbon = (Variant376Ribbon *)work;
        for (i = 0; i < 1; i++, ribbon++) {
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
        ribbon = (Variant376Ribbon *)work;
        for (i = 0; i < 1; i++, ribbon++) {
            k = 0;
            poly = (POLY_FT4 *)(work + 0xC6C);
            for (; k < MODEL_VARIANT_HALF(work, 0xDCC); k++) {
                poly->x0 = ribbon->sa[k] + ribbon->ox[k];
                poly->y0 = (ribbon->sa[k] >> 16) + ribbon->oy[k];
                poly->x1 = ribbon->sa[k + 1] + ribbon->ox[k + 1];
                poly->y1 = (ribbon->sa[k + 1] >> 16) + ribbon->oy[k + 1];
                poly->x2 = ribbon->sa[k] - ribbon->ox[k];
                poly->y2 = (ribbon->sa[k] >> 16) - ribbon->oy[k];
                poly->x3 = ribbon->sa[k + 1] - ribbon->ox[k + 1];
                poly->y3 = (ribbon->sa[k + 1] >> 16) - ribbon->oy[k + 1];
                poly->r0 = ribbon->color[0];
                poly->g0 = ribbon->color[1];
                poly->b0 = ribbon->color[2];
                if (ribbon->otz[k] >= 0 && ribbon->flag[k] >= 0) {
                    GsSortPoly(poly, ot, ribbon->otz[k]);
                }
                if (!(k & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
        }
        if (MODEL_VARIANT_WORD(work, 0xDF8) == 1) {
            if (MODEL_VARIANT_HALF(work, 0xDCC) < 0x11) {
                MODEL_VARIANT_HALF(work, 0xDCC) += MODEL_VARIANT_HALF(work, 0xDAC);
                if (MODEL_VARIANT_HALF(work, 0xDCC) >= 0x10) {
                    MODEL_VARIANT_HALF(work, 0xDCC) = 0x10;
                    MODEL_VARIANT_WORD(work, 0xDF8) = 2;
                }
            }
        } else {
            start = *(u32 *)(*(u8 *G32 *)(work + 0xDB4) + 0x30);
            now = MODEL_VARIANT_WORD(work, 0xDA4);
            if ((u32)now >= (u32)start && MODEL_VARIANT_HALF(work, 0xDD2) > 0) {
                MODEL_VARIANT_HALF(work, 0xDD2) =
                    0x400 - ((u32)(now - start) << 10) / (u32)(*(u32 *)(*(u8 *G32 *)(work + 0xDB4) + 0x34) - start);
                if (MODEL_VARIANT_HALF(work, 0xDD2) <= 0) {
                    MODEL_VARIANT_HALF(work, 0xDD2) = 0;
                }
            }
        }
    }
    MODEL_VARIANT_WORD(work, 0xDE8) -= MODEL_VARIANT_WORD(work, 0xDAC) * 384;
    MODEL_VARIANT_WORD(work, 0xDEC) -= MODEL_VARIANT_WORD(work, 0xDAC) << 5;
}
