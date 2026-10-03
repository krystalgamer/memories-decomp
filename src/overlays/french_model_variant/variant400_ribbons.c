#include "../../types.h"
#include "variant400_ribbons.h"

void func_8013BA88(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[5][17];
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    GsOT *ot;
    s16 i;
    s16 k;
    s32 wave;
    s32 phase;
    s32 yaw;
    s32 pitch;
    s16 len;
    Model400FirstRibbon *ribbon;
    POLY_FT4 *poly;
    s32 bend;
    s32 dx;
    s32 dy;

    work = ctx;
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x2034), MODEL_VARIANT_WORD(work, 0x202C)) + 0xC00;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x2030), MODEL_VARIANT_WORD(work, 0x202C)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x2090) > 0) {
        if (MODEL_VARIANT_WORD(work, 0x2090) < 4) {
            len = 32;
        } else {
            len = MODEL_VARIANT_WORD(work, 0x2068) / 32;
        }
        ribbon = (Model400FirstRibbon *)work;
        for (i = 0, phase = 0; i < 5; i++, ribbon++, phase = i * 4096 / 5) {
            if (MODEL_VARIANT_WORD(work, 0x2090) < 4) {
                setVector(&ribbon->delta,
                          ribbon->position.vx - MODEL_VARIANT_WORD(work, 0x1FF4),
                          ribbon->position.vy - MODEL_VARIANT_WORD(work, 0x1FF8),
                          ribbon->position.vz - MODEL_VARIANT_WORD(work, 0x1FFC));
                for (k = 0, wave = -MODEL_VARIANT_WORD(work, 0x2088); k < 17; k++, wave += 1300) {
                    if (k == 0 || k == 16) {
                        bend = 0;
                    } else {
                        bend = phase - k * 256 + MODEL_VARIANT_WORD(work, 0x208C);
                    }
                    setVector(&ribbon->a[k],
                              ribbon->delta.vx * k / 16 + (rcos(yaw) * (rsin(bend) * ((rsin(wave) * 64 >> 12) + 64) >> 12) >> 12),
                              ribbon->delta.vy * k / 16,
                              ribbon->delta.vz * k / 16 + (rsin(yaw) * (rsin(bend) * ((rsin(wave) * 64 >> 12) + 64) >> 12) >> 12));
                    setVector(&ribbon->b[k],
                              ribbon->a[k].vx + (rcos(yaw) * len >> 12),
                              ribbon->a[k].vy,
                              ribbon->a[k].vz + (rsin(yaw) * len >> 12));
                }
            } else {
                MODEL_VARIANT_WORD(work, 0x2018) = MODEL_VARIANT_HALF(work, 0x2000) - ribbon->position.vx;
                MODEL_VARIANT_WORD(work, 0x201C) = MODEL_VARIANT_HALF(work, 0x2002) - ribbon->position.vy;
                MODEL_VARIANT_WORD(work, 0x2020) = MODEL_VARIANT_HALF(work, 0x2004) - ribbon->position.vz;
                for (k = 0, wave = -MODEL_VARIANT_WORD(work, 0x2088); k < 17; k++, wave += 1300) {
                    if (k == 0 || k == 16) {
                        bend = 0;
                    } else {
                        bend = phase - k * 256 + MODEL_VARIANT_WORD(work, 0x208C);
                    }
                    setVector(&ribbon->a[k],
                              ribbon->delta.vx + MODEL_VARIANT_WORD(work, 0x2018) * k / 16
                                  + (rcos(pitch) * (rsin(bend) * ((rsin(wave) * 64 >> 12) + 64) >> 12) >> 12),
                              ribbon->delta.vy + MODEL_VARIANT_WORD(work, 0x201C) * k / 16
                                  + (rsin(pitch) * (rsin(bend) * ((rsin(wave) * 64 >> 12) + 64) >> 12) >> 12),
                              ribbon->delta.vz + MODEL_VARIANT_WORD(work, 0x2020) * k / 16);
                    setVector(&ribbon->b[k],
                              ribbon->a[k].vx + (rcos(yaw) * len >> 12),
                              ribbon->a[k].vy,
                              ribbon->a[k].vz + (rsin(yaw) * len >> 12));
                }
            }
        }
        setVector(&rot, 0, 0, 0);
        m.t[0] = MODEL_VARIANT_WORD(work, 0x1FF4);
        m.t[1] = MODEL_VARIANT_WORD(work, 0x1FF8);
        m.t[2] = MODEL_VARIANT_WORD(work, 0x1FFC);
        setVector(&scale, 4096, 4096, 4096);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        ribbon = (Model400FirstRibbon *)work;
        for (i = 0; i < 5; i++, ribbon++) {
            for (k = 0; k < 17; k++) {
                if (k == 16) {
                    ribbon->otz[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k], &ribbon->a[k - 1], &ribbon->a[k],
                                                  &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed,
                                                  &p, &flags[i][k]);
                    RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                    dx = ribbon->sa[k].point.vx - ribbon->sa[k - 1].point.vx;
                    dy = ribbon->sa[k].point.vy - ribbon->sa[k - 1].point.vy;
                    ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                    ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                    ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                } else {
                    ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                                  &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed,
                                                  &p, &flags[i][k]);
                    RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                    dx = ribbon->sa[k + 1].point.vx - ribbon->sa[k].point.vx;
                    dy = ribbon->sa[k + 1].point.vy - ribbon->sa[k].point.vy;
                    ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                    ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                    ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                }
            }
        }
        ribbon = (Model400FirstRibbon *)work;
        for (i = 0; i < 5; i++, ribbon++) {
            for (k = MODEL_VARIANT_WORD(work, 0x206C), poly = (POLY_FT4 *)(work + 0x1F80);
                 k < MODEL_VARIANT_WORD(work, 0x2070);
                 k++, poly = (POLY_FT4 *)(work + 0x1F80)) {
                if ((s16)(k % 2) == 1) {
                    poly++;
                }
                if (k + 1 == MODEL_VARIANT_WORD(work, 0x2070)) {
                    poly->x0 = ribbon->sa[k].point.vx + ribbon->ox[k];
                    poly->y0 = (ribbon->sa[k].packed >> 16) + ribbon->oy[k];
                    poly->x1 = ribbon->sa[k + 1].point.vx;
                    poly->y1 = ribbon->sa[k + 1].packed >> 16;
                    poly->x2 = ribbon->sa[k].point.vx - ribbon->ox[k];
                    poly->y2 = (ribbon->sa[k].packed >> 16) - ribbon->oy[k];
                    poly->x3 = ribbon->sa[k + 1].point.vx;
                    poly->y3 = ribbon->sa[k + 1].packed >> 16;
                } else if (k == MODEL_VARIANT_WORD(work, 0x206C)) {
                    poly->x0 = ribbon->sa[k].point.vx;
                    poly->y0 = ribbon->sa[k].packed >> 16;
                    poly->x1 = ribbon->sa[k + 1].point.vx + ribbon->ox[k + 1];
                    poly->y1 = (ribbon->sa[k + 1].packed >> 16) + ribbon->oy[k + 1];
                    poly->x2 = ribbon->sa[k].point.vx;
                    poly->y2 = ribbon->sa[k].packed >> 16;
                    poly->x3 = ribbon->sa[k + 1].point.vx - ribbon->ox[k + 1];
                    poly->y3 = (ribbon->sa[k + 1].packed >> 16) - ribbon->oy[k + 1];
                } else {
                    poly->x0 = ribbon->sa[k].point.vx + ribbon->ox[k];
                    poly->y0 = (ribbon->sa[k].packed >> 16) + ribbon->oy[k];
                    poly->x1 = ribbon->sa[k + 1].point.vx + ribbon->ox[k + 1];
                    poly->y1 = (ribbon->sa[k + 1].packed >> 16) + ribbon->oy[k + 1];
                    poly->x2 = ribbon->sa[k].point.vx - ribbon->ox[k];
                    poly->y2 = (ribbon->sa[k].packed >> 16) - ribbon->oy[k];
                    poly->x3 = ribbon->sa[k + 1].point.vx - ribbon->ox[k + 1];
                    poly->y3 = (ribbon->sa[k + 1].packed >> 16) - ribbon->oy[k + 1];
                }
                poly->r0 = ribbon->color[0];
                poly->g0 = ribbon->color[1];
                poly->b0 = ribbon->color[2];
                if (ribbon->otz[k] >= 0 && flags[i][k] >= 0) {
                    GsSortPoly(poly, ot, ribbon->otz[k]);
                }
            }
        }
        if (MODEL_VARIANT_WORD(work, 0x2090) == 1) {
            if (MODEL_VARIANT_WORD(work, 0x2070) < 16) {
                MODEL_VARIANT_WORD(work, 0x2070) += (u32)MODEL_VARIANT_WORD(work, 0x2050) * 3 / 2;
                if (MODEL_VARIANT_WORD(work, 0x2070) >= 16) {
                    MODEL_VARIANT_WORD(work, 0x2070) = 16;
                    MODEL_VARIANT_WORD(work, 0x2090) = 2;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0x2090) == 2) {
            if (MODEL_VARIANT_WORD(work, 0x206C) < 16) {
                MODEL_VARIANT_WORD(work, 0x206C) += (u32)MODEL_VARIANT_WORD(work, 0x2050) * 3 / 2;
                if (MODEL_VARIANT_WORD(work, 0x206C) >= 16) {
                    MODEL_VARIANT_WORD(work, 0x206C) = 16;
                    MODEL_VARIANT_WORD(work, 0x2070) = 16;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0x2090) == 3) {
            MODEL_VARIANT_WORD(work, 0x206C) = 0;
            MODEL_VARIANT_WORD(work, 0x2070) = 0;
        } else if (MODEL_VARIANT_WORD(work, 0x2090) == 4) {
            if (MODEL_VARIANT_WORD(work, 0x2070) < 16) {
                MODEL_VARIANT_WORD(work, 0x2070) += (u32)MODEL_VARIANT_WORD(work, 0x2050) * 3 / 2;
                if (MODEL_VARIANT_WORD(work, 0x2070) >= 16) {
                    MODEL_VARIANT_WORD(work, 0x2070) = 16;
                    MODEL_VARIANT_WORD(work, 0x2090) = 5;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0x2090) >= 6) {
            if (MODEL_VARIANT_WORD(work, 0x206C) < 16) {
                MODEL_VARIANT_WORD(work, 0x206C) += (u32)MODEL_VARIANT_WORD(work, 0x2050) * 3 / 2;
                if (MODEL_VARIANT_WORD(work, 0x206C) >= 16) {
                    MODEL_VARIANT_WORD(work, 0x206C) = 16;
                    MODEL_VARIANT_WORD(work, 0x2070) = 16;
                    MODEL_VARIANT_WORD(work, 0x2090) = 7;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0x2090) == 10) {
            if (MODEL_VARIANT_WORD(work, 0x2068) > 0) {
                MODEL_VARIANT_WORD(work, 0x2068) -= MODEL_VARIANT_WORD(work, 0x2050) * 8;
                if (MODEL_VARIANT_WORD(work, 0x2068) <= 0) {
                    MODEL_VARIANT_WORD(work, 0x2068) = 0;
                }
            }
        }
    }
    MODEL_VARIANT_WORD(work, 0x2088) += MODEL_VARIANT_WORD(work, 0x2050) * 400;
    MODEL_VARIANT_WORD(work, 0x208C) += MODEL_VARIANT_WORD(work, 0x2050) * 384;
}
