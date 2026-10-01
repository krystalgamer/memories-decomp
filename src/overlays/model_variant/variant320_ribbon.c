#include "../../types.h"

#include "variant320_ribbon.h"

/* Builds a seventeen-point ribbon that bends around the variant's axis and a
 * copy of it moved along the view, projects both, and draws the first
 * `work[0xD40]` segments as flat POLY_FT4 quads as wide as the projected
 * offset. In phase 1 the drawn length follows the timing record's clock up to
 * 16; in phase 3 the copy's offset shrinks with it. The per-ribbon turn and the
 * coil angle of the header-321 form are still computed but unused here. */
void func_8013B9A8(u8 *ctx)
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
    Variant320Ribbon *ribbon;
    POLY_FT4 *poly;
    s16 k;
    s32 bend;
    s32 coil;
    s32 dx;
    s32 dy;

    work = ctx;
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0xD10), MODEL_VARIANT_WORD(work, 0xD08)) + 0xC00;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0xD0C), MODEL_VARIANT_WORD(work, 0xD08)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0xD6C) > 0) {
        if (!(MODEL_VARIANT_WORD(work, 0xD20) & 1)) {
            len = MODEL_VARIANT_HALF(work, 0xD46) / 16;
        } else {
            len = MODEL_VARIANT_HALF(work, 0xD46) * 80 / 1024;
        }
        ribbon = (Variant320Ribbon *)work;
        for (i = 0, phase = 0; i < 1; i++, ribbon++, phase = i * 0x1800) {
            if (i == 0) {
                turn = 0x400;
            } else if (i % 2 == 1) {
                turn = i * 360 + 0x400;
            } else {
                turn = 0x400 - i * 360;
            }
            for (k = 0, wave = -MODEL_VARIANT_WORD(work, 0xD5C); k < 17; k++, wave += 0x300) {
                if (k == 0 || k == 16) {
                    bend = 0;
                } else {
                    bend = phase - k * 384 + MODEL_VARIANT_WORD(work, 0xD60);
                }
                coil = k * 128;
                setVector(&ribbon->a[k],
                          MODEL_VARIANT_WORD(work, 0xCF4) * k / 16 +
                              (rcos(pitch) * (rsin(bend) * ((rsin(wave) * 64 >> 12) + 0x40) >> 12) >> 12),
                          MODEL_VARIANT_WORD(work, 0xCF8) * k / 16 +
                              (rsin(pitch) * (rsin(bend) * ((rsin(wave) * 64 >> 12) + 0x40) >> 12) >> 12),
                          MODEL_VARIANT_WORD(work, 0xCFC) * k / 16);
                setVector(&ribbon->b[k], ribbon->a[k].vx + (rcos(yaw) * len >> 12), ribbon->a[k].vy,
                          ribbon->a[k].vz + (rsin(yaw) * len >> 12));
            }
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work, 0xCE0);
        m.t[1] = MODEL_VARIANT_WORD(work, 0xCE4);
        m.t[2] = MODEL_VARIANT_WORD(work, 0xCE8);
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
        ribbon = (Variant320Ribbon *)work;
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
        ribbon = (Variant320Ribbon *)work;
        for (i = 0; i < 1; i++, ribbon++) {
            k = 0;
            poly = (POLY_FT4 *)(work + 0xC6C);
            for (; k < MODEL_VARIANT_HALF(work, 0xD40); k++) {
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
        if (MODEL_VARIANT_WORD(work, 0xD6C) == 1) {
            if (MODEL_VARIANT_HALF(work, 0xD40) < 0x11) {
                start = *(u32 *)(*(u8 **)(work + 0xD34) + 0x14);
                MODEL_VARIANT_HALF(work, 0xD40) = ((u32)(MODEL_VARIANT_WORD(work, 0xD24) - start) << 4) /
                                                  (u32)(*(u32 *)(*(u8 **)(work + 0xD34) + 0x18) - start);
                if (MODEL_VARIANT_HALF(work, 0xD40) >= 0x10) {
                    MODEL_VARIANT_HALF(work, 0xD40) = 0x10;
                    MODEL_VARIANT_WORD(work, 0xD6C) = 2;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0xD6C) == 3) {
            if (MODEL_VARIANT_HALF(work, 0xD46) > 0) {
                start = *(u32 *)(*(u8 **)(work + 0xD34) + 0x1C);
                now = MODEL_VARIANT_WORD(work, 0xD24);
                if ((u32)now >= (u32)start) {
                    MODEL_VARIANT_HALF(work, 0xD46) =
                        0x400 - ((u32)(now - start) << 10) / (u32)(*(u32 *)(*(u8 **)(work + 0xD34) + 0x20) - start);
                }
                if (MODEL_VARIANT_HALF(work, 0xD46) <= 0) {
                    MODEL_VARIANT_HALF(work, 0xD46) = 0;
                }
            }
        }
    }
    MODEL_VARIANT_WORD(work, 0xD5C) += MODEL_VARIANT_WORD(work, 0xD2C) * 850;
    MODEL_VARIANT_WORD(work, 0xD60) += MODEL_VARIANT_WORD(work, 0xD2C) << 7;
}
