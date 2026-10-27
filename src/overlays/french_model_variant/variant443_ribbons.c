#include "../../types.h"
#include "variant443_ribbons.h"

void func_8013CADC(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[3][17];
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
    Ribbons443Timing *timing;
    ModelVariantSheet *sheet;

    work = ctx;
    sheet = (ModelVariantSheet *)(work + 0x2A68);
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x3610), MODEL_VARIANT_WORD(work, 0x3608)) + 0xC00;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x360C), MODEL_VARIANT_WORD(work, 0x3608)) + 0xC00;
    len = sheet->size * 48 / 4096;
    ribbon = (Model400FirstRibbon *)work;
    for (i = 0, phase = 0; i < 3; i++, ribbon++, phase = i * 4096 / 3) {
        setVector(&ribbon->delta,
                  ((Ribbons443State *)work)->origins[i].vx - MODEL_VARIANT_WORD(work, 0x35A0),
                  ((Ribbons443State *)work)->origins[i].vy - MODEL_VARIANT_WORD(work, 0x35A4),
                  ((Ribbons443State *)work)->origins[i].vz - MODEL_VARIANT_WORD(work, 0x35A8));
        for (k = 0, wave = MODEL_VARIANT_WORD(work, 0x3670); k < 17; k++, wave += 1300) {
            if (k == 0 || k == 16) {
                bend = 0;
            } else {
                bend = phase - k * 256 - MODEL_VARIANT_WORD(work, 0x3674);
            }
            setVector(&ribbon->a[k],
                      ribbon->delta.vx * k / 16 + (rcos(yaw) * (rsin(bend) * ((rsin(wave) * 32 >> 12) + 32) >> 12) >> 12),
                      ribbon->delta.vy * k / 16,
                      ribbon->delta.vz * k / 16 + (rsin(yaw) * (rsin(bend) * ((rsin(wave) * 32 >> 12) + 32) >> 12) >> 12));
            setVector(&ribbon->b[k],
                      ribbon->a[k].vx + (rcos(yaw) * len >> 12),
                      ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(yaw) * len >> 12));
        }
    }
    setVector(&rot, 0, 0, 0);
    m.t[0] = MODEL_VARIANT_WORD(work, 0x35A0);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x35A4);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x35A8);
    setVector(&scale, 4096, 4096, 4096);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    ribbon = (Model400FirstRibbon *)work;
    for (i = 0; i < 3; i++, ribbon++) {
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
    for (i = 0; i < 3; i++, ribbon++) {
        for (k = MODEL_VARIANT_WORD(work, 0x3668), poly = (POLY_FT4 *)(work + 0x34BC);
             k < 16;
             k++, poly = (POLY_FT4 *)(work + 0x34BC)) {
            if ((s16)(k % 2) == 1) {
                poly++;
            }
            poly->x0 = ribbon->sa[k].point.vx + ribbon->ox[k];
            poly->y0 = (ribbon->sa[k].packed >> 16) + ribbon->oy[k];
            poly->x1 = ribbon->sa[k + 1].point.vx + ribbon->ox[k + 1];
            poly->y1 = (ribbon->sa[k + 1].packed >> 16) + ribbon->oy[k + 1];
            poly->x2 = ribbon->sa[k].point.vx - ribbon->ox[k];
            poly->y2 = (ribbon->sa[k].packed >> 16) - ribbon->oy[k];
            poly->x3 = ribbon->sa[k + 1].point.vx - ribbon->ox[k + 1];
            poly->y3 = (ribbon->sa[k + 1].packed >> 16) - ribbon->oy[k + 1];
            poly->r0 = ribbon->color[0];
            poly->g0 = ribbon->color[1];
            poly->b0 = ribbon->color[2];
            if (ribbon->otz[k] >= 0 && flags[i][k] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[k]);
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x3698) == 2 && MODEL_VARIANT_WORD(work, 0x3668) > 0) {
        timing = *(Ribbons443Timing *G32 *)(work + 0x3634);
        MODEL_VARIANT_WORD(work, 0x3668) = 16 - ((u32)MODEL_VARIANT_WORD(work, 0x3624) - timing->spread_start) * 16
                                                    / (timing->spread_end - timing->spread_start);
        if (MODEL_VARIANT_WORD(work, 0x3668) <= 0) {
            MODEL_VARIANT_WORD(work, 0x3668) = 0;
            MODEL_VARIANT_WORD(work, 0x3698) = 3;
        }
    }
    MODEL_VARIANT_WORD(work, 0x3670) += MODEL_VARIANT_WORD(work, 0x362C) * 192;
    MODEL_VARIANT_WORD(work, 0x3674) += MODEL_VARIANT_WORD(work, 0x362C) * 128;
}
