#include "../../types.h"
#include "variant474_ribbons.h"

/* The North American header-457 copy, built with the CDK compiler, names the
 * closing segment's points directly and reads the following screen point as
 * element k of the array starting one past sa. */
#ifdef MODEL_VARIANT474_RIBBONS_DIRECT_CLOSING
#define RIBBONS474_LAST 16
#define RIBBONS474_NEXT_SA (ribbon->sa + 1)[k]
#else
#define RIBBONS474_LAST k
#define RIBBONS474_NEXT_SA ribbon->sa[k + 1]
#endif

void func_8013BE38(u8 *ctx)
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
    s16 k;
    s32 yaw;
    s32 pitch;
    s16 len;
    s32 shift;
    s16 r;
    s32 dx;
    s32 dy;
    s32 prev;
    s32 cur;
    Ribbon474 *ribbon;
    POLY_GT4 *poly;
    Ribbon474Timing *timing;

    work = ctx;
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x1B80), MODEL_VARIANT_WORD(work, 0x1B78)) + 0xC00;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x1B7C), MODEL_VARIANT_WORD(work, 0x1B78)) + 0xC00;
    shift = (MODEL_VARIANT_HALF(work, 0x1C34) != 0) << 11;
    if (!(MODEL_VARIANT_WORD(work, 0x1B90) & 1)) {
        len = MODEL_VARIANT_WORD(work, 0x1BB4) / 16;
    } else {
        len = MODEL_VARIANT_WORD(work, 0x1BB4) * 72 / 1024;
    }
    ribbon = (Ribbon474 *)(work + 0x230);
    for (i = 0; i < 1; i++, ribbon++) {
        for (k = 0; k < 17; k++) {
            r = (0x400 - ribbon->amp[k]) / 4;
            setVector(&ribbon->a[k],
                      (rcos(-ribbon->phase[k] - MODEL_VARIANT_WORD(work, 0x1BB8) + shift) * r >> 12)
                          + MODEL_VARIANT_WORD(work, 0x1B64) * ribbon->amp[k] / 1024,
                      MODEL_VARIANT_WORD(work, 0x1B68) * ribbon->amp[k] / 1024,
                      (rsin(-ribbon->phase[k] - MODEL_VARIANT_WORD(work, 0x1BB8) + shift) * r >> 12)
                          + MODEL_VARIANT_WORD(work, 0x1B6C) * ribbon->amp[k] / 1024);
            setVector(&ribbon->b[k],
                      ribbon->a[k].vx + (rcos(yaw) * len >> 12),
                      ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(yaw) * len >> 12));
            if (MODEL_VARIANT_WORD(work, 0x1C1C) > 0) {
                if (ribbon->phase[k] < 0) {
                    ribbon->phase[k] += MODEL_VARIANT_WORD(work, 0x1B9C) * 128;
                } else if (ribbon->amp[k] < 0x800) {
                    ribbon->amp[k] += MODEL_VARIANT_WORD(work, 0x1B9C) * 64;
                    if (ribbon->amp[0] >= 0x400 && MODEL_VARIANT_WORD(work, 0x1C1C) == 1) {
                        MODEL_VARIANT_WORD(work, 0x1C1C) = 2;
                    }
                    if (ribbon->amp[k] >= 0x800) {
                        ribbon->amp[k] = 0x800;
                    }
                    if (ribbon->amp[16] >= 0x800 && MODEL_VARIANT_WORD(work, 0x1C1C) == 2) {
                        MODEL_VARIANT_WORD(work, 0x1C1C) = 6;
                    }
                }
            }
        }
    }
    setVector(&rot, 0, 0, 0);
    m.t[0] = MODEL_VARIANT_WORD(work, 0x1B50);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x1B54);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x1B58);
    setVector(&scale, MODEL_VARIANT_WORD(work, 0x1BBC), MODEL_VARIANT_WORD(work, 0x1BBC), MODEL_VARIANT_WORD(work, 0x1BBC));
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    ribbon = (Ribbon474 *)(work + 0x230);
    for (i = 0; i < 1; i++, ribbon++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                ribbon->otz[RIBBONS474_LAST] =
                    RotTransPers4(&ribbon->a[RIBBONS474_LAST - 1], &ribbon->a[RIBBONS474_LAST],
                                  &ribbon->a[RIBBONS474_LAST - 1], &ribbon->a[RIBBONS474_LAST],
                                  &ribbon->sa[RIBBONS474_LAST - 1].packed, &ribbon->sa[RIBBONS474_LAST].packed,
                                  &ribbon->sa[RIBBONS474_LAST - 1].packed, &ribbon->sa[RIBBONS474_LAST].packed,
                                  &p, &ribbon->flags[RIBBONS474_LAST]);
                RotTransPers(&ribbon->b[RIBBONS474_LAST], &ribbon->sb[RIBBONS474_LAST].packed, &p, &flag);
                dx = ribbon->sa[RIBBONS474_LAST].point.vx - ribbon->sa[RIBBONS474_LAST - 1].point.vx;
                dy = (ribbon->sa[RIBBONS474_LAST].packed >> 16) - (ribbon->sa[RIBBONS474_LAST - 1].packed >> 16);
                ribbon->angle[RIBBONS474_LAST] = ratan2(dy, dx) - 0x400;
                ribbon->width[RIBBONS474_LAST] =
                    ribbon->sb[RIBBONS474_LAST].point.vx - ribbon->sa[RIBBONS474_LAST].point.vx;
                ribbon->ox[RIBBONS474_LAST] =
                    rcos(ribbon->angle[RIBBONS474_LAST]) * ribbon->width[RIBBONS474_LAST] >> 12;
                ribbon->oy[RIBBONS474_LAST] =
                    rsin(ribbon->angle[RIBBONS474_LAST]) * ribbon->width[RIBBONS474_LAST] >> 12;
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                              &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed,
                                              &p, &ribbon->flags[k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                dx = RIBBONS474_NEXT_SA.point.vx - ribbon->sa[k].point.vx;
                dy = RIBBONS474_NEXT_SA.point.vy - ribbon->sa[k].point.vy;
                ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            }
        }
    }
    ribbon = (Ribbon474 *)(work + 0x230);
    for (i = 0; i < 1; i++, ribbon++) {
        poly = (POLY_GT4 *)(work + 0x19CC);
        for (k = 0; k < 16; k++) {
            setRGB0(poly, ribbon->color[k].r, ribbon->color[k].g, ribbon->color[k].b);
            setRGB1(poly, ribbon->color[k + 1].r, ribbon->color[k + 1].g, ribbon->color[k + 1].b);
            setRGB2(poly, ribbon->color[k].r, ribbon->color[k].g, ribbon->color[k].b);
            setRGB3(poly, ribbon->color[k + 1].r, ribbon->color[k + 1].g, ribbon->color[k + 1].b);
            poly->x0 = ribbon->sa[k].point.vx + ribbon->ox[k];
            poly->y0 = (ribbon->sa[k].packed >> 16) + ribbon->oy[k];
            poly->x1 = RIBBONS474_NEXT_SA.point.vx + ribbon->ox[k + 1];
            poly->y1 = (RIBBONS474_NEXT_SA.packed >> 16) + ribbon->oy[k + 1];
            poly->x2 = ribbon->sa[k].point.vx - ribbon->ox[k];
            poly->y2 = (ribbon->sa[k].packed >> 16) - ribbon->oy[k];
            poly->x3 = RIBBONS474_NEXT_SA.point.vx - ribbon->ox[k + 1];
            poly->y3 = (RIBBONS474_NEXT_SA.packed >> 16) - ribbon->oy[k + 1];
            if (ribbon->otz[k] >= 0 && ribbon->flags[k] >= 0) {
                GsSortPoly(poly, ot, (u16)ribbon->otz[k]);
            }
            if (k == 0) {
                poly++;
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x1BBC) < 0x1000) {
        timing = *(Ribbon474Timing *G32 *)(work + 0x1BA4);
        MODEL_VARIANT_WORD(work, 0x1BBC) = ((MODEL_VARIANT_WORD(work, 0x1B94) - timing->grow_start) << 12) / (timing->grow_end - timing->grow_start);
        if (MODEL_VARIANT_WORD(work, 0x1BBC) >= 0x1000) {
            MODEL_VARIANT_WORD(work, 0x1BBC) = 0x1000;
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x1B94) < (*(Ribbon474Timing *G32 *)(work + 0x1BA4))->spin_end) {
        MODEL_VARIANT_WORD(work, 0x1BB8) += MODEL_VARIANT_WORD(work, 0x1B9C) * 128;
    } else if (MODEL_VARIANT_WORD(work, 0x1C1C) == 0) {
        prev = MODEL_VARIANT_WORD(work, 0x1BB8) / 4096;
        MODEL_VARIANT_WORD(work, 0x1BB8) += MODEL_VARIANT_WORD(work, 0x1B9C) * 128;
        cur = MODEL_VARIANT_WORD(work, 0x1BB8) / 4096;
        if (prev + 1 == cur) {
            MODEL_VARIANT_WORD(work, 0x1C1C) = 1;
        }
    }
}
