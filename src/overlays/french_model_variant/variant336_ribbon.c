#include "../../types.h"
#include "variant336_ribbon.h"

void func_8013BA9C(u8 *ctx)
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
    s16 len;
    ModelVariant336Ribbon *ribbon;
    POLY_FT4 *poly;
    s16 k;
    s32 bend;
    s32 dx;
    s32 dy;

    work = ctx;
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x1FB4), MODEL_VARIANT_WORD(work, 0x1FAC)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x2000) > 0) {
        ribbon = (ModelVariant336Ribbon *)work;
        for (i = 0, phase = 0; i < 4; i++, ribbon++, phase = i * 0x400) {
            len = ribbon->extent * 48 / 1024;
            ribbon->origin.vx = MODEL_VARIANT_WORD(work, 0x1F98) * (i + 1) / 4;
            ribbon->origin.vy = -1024;
            ribbon->origin.vz = MODEL_VARIANT_WORD(work, 0x1FA0) * (i + 1) / 4;
            ribbon->end.vx = ribbon->origin.vx;
            ribbon->end.vy = 0;
            ribbon->end.vz = ribbon->origin.vz;
            MODEL_VARIANT_WORD(work, 0x1F80) = ribbon->end.vx - ribbon->origin.vx;
            MODEL_VARIANT_WORD(work, 0x1F84) = ribbon->end.vy - ribbon->origin.vy;
            MODEL_VARIANT_WORD(work, 0x1F88) = ribbon->end.vz - ribbon->origin.vz;
            for (k = 0, wave = phase - MODEL_VARIANT_WORD(work, 0x1FF4); k < 17; k++, wave += 1100) {
                if (k == 0 || k == 16) {
                    bend = 0;
                } else {
                    bend = phase - k * 384 + MODEL_VARIANT_WORD(work, 0x1FF8);
                }
                setVector(&ribbon->a[k],
                          ribbon->origin.vx + MODEL_VARIANT_WORD(work, 0x1F80) * k / 16
                              + (rcos(yaw) * ((rsin(bend) * 64 >> 12) + (rsin(wave) * 48 >> 12)) >> 12),
                          ribbon->origin.vy + MODEL_VARIANT_WORD(work, 0x1F84) * k / 16,
                          ribbon->origin.vz + MODEL_VARIANT_WORD(work, 0x1F88) * k / 16
                              + (rsin(yaw) * ((rsin(bend) * 64 >> 12) + (rsin(wave) * 48 >> 12)) >> 12));
                setVector(&ribbon->b[k], ribbon->a[k].vx + (rcos(yaw) * len >> 12), ribbon->a[k].vy,
                          ribbon->a[k].vz + (rsin(yaw) * len >> 12));
            }
        }
        setVector(&rot, 0, 0, 0);
        m.t[0] = MODEL_VARIANT_WORD(work, 0x1F34);
        m.t[1] = 0;
        m.t[2] = MODEL_VARIANT_WORD(work, 0x1F3C);
        setVector(&scale, 4096, 4096, 4096);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        ribbon = (ModelVariant336Ribbon *)work;
        for (i = 0; i < 4; i++, ribbon++) {
            for (k = 0; k < 17; k++) {
                if (k == 16) {
                    ribbon->otz[k] = RotTransPers4(&ribbon->a[15], &ribbon->a[k], &ribbon->a[15], &ribbon->a[k],
                                                 &ribbon->sa[15], &ribbon->sa[k], &ribbon->sa[15], &ribbon->sa[k],
                                                 &p, &ribbon->flag[k]);
                    RotTransPers(&ribbon->b[k], &ribbon->sb[k], &p, &flag);
                    dx = (s16)ribbon->sa[k] - (s16)ribbon->sa[15];
                    dy = (ribbon->sa[k] >> 16) - (ribbon->sa[15] >> 16);
                    ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                    ribbon->width[k] = (s16)ribbon->sb[k] - (s16)ribbon->sa[k];
                    ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
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
        ribbon = (ModelVariant336Ribbon *)work;
        for (i = 0; i < 4; i++, ribbon++) {
            k = 0;
            poly = (POLY_FT4 *)(work + 0x1E70);
            for (; k < ribbon->count; k++) {
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
            if (ribbon->state == 1) {
                if (ribbon->count < 16) {
                    ribbon->count += MODEL_VARIANT_WORD(work, 0x1FD0) * 2;
                    if (ribbon->count >= 16) {
                        ribbon->count = 16;
                        MODEL_VARIANT_WORD(work, 0x1FEC) = i * 2 + 3;
                        if (MODEL_VARIANT_WORD(work, 0x1FEC) >= 8) {
                            MODEL_VARIANT_WORD(work, 0x1FEC) = 8;
                        }
                    }
                }
            } else if (ribbon->state == 2 && ribbon->extent > 0) {
                ribbon->extent -= MODEL_VARIANT_WORD(work, 0x1FD0) * 8;
                if (ribbon->extent <= 0) {
                    ribbon->extent = 0;
                    if (i == 3) {
                        MODEL_VARIANT_WORD(work, 0x2000) = 3;
                    }
                }
            }
        }
    }
    MODEL_VARIANT_WORD(work, 0x1FF4) += MODEL_VARIANT_WORD(work, 0x1FD0) * 650;
    MODEL_VARIANT_WORD(work, 0x1FF8) += MODEL_VARIANT_WORD(work, 0x1FD0) * 100;
}
