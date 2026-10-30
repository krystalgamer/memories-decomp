#include "../../types.h"
#include "variant437_ribbons.h"

/* The North American header-420 ribbons address the closing segment's
 * projected points with fixed indices and the following point as an offset
 * array; the default spelling matches the French and Spanish header 437. */
#ifdef MODEL_VARIANT437_RIBBONS_FIXED_TAIL
#define RIBBONS437_TAIL(k) 8
#define RIBBONS437_BEFORE_TAIL(k) 7
#define RIBBONS437_AT(points, k) (*((k) + (points)))
#define RIBBONS437_NEXT(points, k) (((points) + 1)[k])
#else
#define RIBBONS437_TAIL(k) (k)
#define RIBBONS437_BEFORE_TAIL(k) ((k) - 1)
#define RIBBONS437_AT(points, k) ((points)[k])
#define RIBBONS437_NEXT(points, k) ((points)[(k) + 1])
#endif

void func_8013CB08(u8 *ctx)
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
    s32 wave;
    s32 phase;
    s32 yaw;
    s32 pitch;
    s16 len;
    Ribbons437Ribbon *ribbon;
    POLY_FT4 *poly;
    s32 bend;
    s32 curve;
    s32 amp;
    s32 dist;
    s32 dx;
    s32 dy;
    s32 lo;
    s32 hi;
    s32 odd;

    work = ctx;
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x15CC), MODEL_VARIANT_WORD(work, 0x15C4)) + 0xC00;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x15C8), MODEL_VARIANT_WORD(work, 0x15CC)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x163C) >= 4) {
        len = MODEL_VARIANT_WORD(work, 0x1610) / 32;
    } else {
        len = 32;
    }
    ribbon = (Ribbons437Ribbon *)work;
    for (i = 0, phase = 0; i < 4; i++, ribbon++, phase = i * 1024) {
        if (MODEL_VARIANT_WORD(work, 0x15E0) < (*(Ribbons437Timing *G32 *)(work + 0x15F4))->wave_end && !(i & 1)) {
            wave = -MODEL_VARIANT_WORD(work, 0x1634);
        } else {
            wave = MODEL_VARIANT_WORD(work, 0x1634);
        }
        odd = i & 1;
        for (k = 0; k < 9; k++, wave += 1300) {
            if (MODEL_VARIANT_WORD(work, 0x163C) < 3) {
                if (k == 0 || k == 8) {
                    bend = 0;
                } else if (!odd) {
                    bend = phase - k * 768 + MODEL_VARIANT_WORD(work, 0x1638);
                } else {
                    bend = phase - k * 768 - MODEL_VARIANT_WORD(work, 0x1638);
                }
                amp = rsin(bend) * ((rsin(wave) * 16 >> 12) + 32) >> 12;
                setVector(&ribbon->a[k],
                          MODEL_VARIANT_WORD(work, 0x1580) * k / 8,
                          MODEL_VARIANT_WORD(work, 0x1584) * k / 8 + (rcos(phase + MODEL_VARIANT_WORD(work, 0x161C)) * amp >> 12),
                          MODEL_VARIANT_WORD(work, 0x1588) * k / 8 + (rsin(phase + MODEL_VARIANT_WORD(work, 0x161C)) * amp >> 12));
            } else {
                if (k == 0 || k == 8) {
                    curve = 256;
                    bend = 0;
                } else if (!odd) {
                    curve = k * 128;
                    bend = k * 768 - MODEL_VARIANT_WORD(work, 0x1638);
                } else {
                    curve = k * 128;
                    bend = k * 768 - MODEL_VARIANT_WORD(work, 0x1638);
                }
                dist = MODEL_VARIANT_WORD(work, 0x1594) / 4 * (0x400 - MODEL_VARIANT_WORD(work, 0x1590)) / 1024;
                amp = (rsin(curve) * 64 >> 12) + (rsin(bend) * ((rsin(wave) * 16 >> 12) + 32) >> 12);
                setVector(&ribbon->a[k],
                          rcos(phase + MODEL_VARIANT_WORD(work, 0x161C)) * amp >> 12,
                          -dist * k / 8,
                          rsin(phase + MODEL_VARIANT_WORD(work, 0x161C)) * amp >> 12);
            }
            setVector(&ribbon->b[k],
                      ribbon->a[k].vx + (rcos(yaw) * len >> 12),
                      ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(yaw) * len >> 12));
        }
    }
    setVector(&rot, 0, 0, 0);
    m.t[0] = MODEL_VARIANT_WORD(work, 0x1554);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x1558);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x155C);
    setVector(&scale, 4096, 4096, 4096);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    ribbon = (Ribbons437Ribbon *)work;
    for (i = 0; i < 4; i++, ribbon++) {
        for (k = 0; k < 9; k++) {
            if (k == 8) {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k], &ribbon->a[k - 1], &ribbon->a[k],
                                              &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed,
                                              &p, &flag);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                dx = ribbon->sa[RIBBONS437_TAIL(k)].point.vx - ribbon->sa[RIBBONS437_BEFORE_TAIL(k)].point.vx;
                dy = ribbon->sa[RIBBONS437_TAIL(k)].point.vy - ribbon->sa[RIBBONS437_BEFORE_TAIL(k)].point.vy;
                ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                ribbon->width[k] = ribbon->sb[RIBBONS437_TAIL(k)].point.vx - ribbon->sa[RIBBONS437_TAIL(k)].point.vx;
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                              &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed,
                                              &p, &flag);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                dx = RIBBONS437_NEXT(ribbon->sa, k).point.vx - ribbon->sa[k].point.vx;
                dy = RIBBONS437_NEXT(ribbon->sa, k).point.vy - ribbon->sa[k].point.vy;
                ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            }
        }
    }
    ribbon = (Ribbons437Ribbon *)work;
    for (i = 0; i < 4; i++, ribbon++) {
        lo = 0;
        if (ribbon->start >= 0) {
            lo = ribbon->start;
        }
        hi = ribbon->end < lo ? lo : ribbon->end;
        for (k = lo, poly = (POLY_FT4 *)(work + 0x1458); k < hi; k++, poly = (POLY_FT4 *)(work + 0x1458)) {
            if ((s16)(k % 2) == 1) {
                poly++;
            }
            poly->x0 = RIBBONS437_AT(ribbon->sa, k).point.vx + ribbon->ox[k];
            poly->y0 = (ribbon->sa[k].packed >> 16) + ribbon->oy[k];
            poly->x1 = RIBBONS437_NEXT(ribbon->sa, k).point.vx + ribbon->ox[k + 1];
            poly->y1 = (RIBBONS437_NEXT(ribbon->sa, k).packed >> 16) + ribbon->oy[k + 1];
            poly->x2 = ribbon->sa[k].point.vx - ribbon->ox[k];
            poly->y2 = (ribbon->sa[k].packed >> 16) - ribbon->oy[k];
            poly->x3 = RIBBONS437_NEXT(ribbon->sa, k).point.vx - ribbon->ox[k + 1];
            poly->y3 = (RIBBONS437_NEXT(ribbon->sa, k).packed >> 16) - ribbon->oy[k + 1];
            poly->r0 = ribbon->color[0];
            poly->g0 = ribbon->color[1];
            poly->b0 = ribbon->color[2];
            if (ribbon->otz[k] > 0) {
                Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)ribbon->otz[k], 1);
            }
        }
        if (MODEL_VARIANT_WORD(work, 0x15E0) < (*(Ribbons437Timing *G32 *)(work + 0x15F4))->wave_end) {
            if (!(i & 1)) {
                if (ribbon->state == 0) {
                    ribbon->end += (u32)MODEL_VARIANT_WORD(work, 0x15E8) / 2;
                    if (ribbon->end >= 8) {
                        ribbon->end = 8;
                        ribbon->state = 1;
                    }
                } else {
                    ribbon->start += (u32)MODEL_VARIANT_WORD(work, 0x15E8) / 2;
                    if (ribbon->start >= 8) {
                        ribbon->start = 0;
                        ribbon->end = 0;
                        ribbon->count++;
                        if (ribbon->count < 5) {
                            ribbon->state = 0;
                        }
                    }
                }
            } else {
                if (ribbon->state == 0) {
                    ribbon->start -= (u32)MODEL_VARIANT_WORD(work, 0x15E8) / 2;
                    if (ribbon->start <= 0) {
                        ribbon->start = 0;
                        ribbon->state = 1;
                    }
                } else {
                    ribbon->end -= (u32)MODEL_VARIANT_WORD(work, 0x15E8) / 2;
                    if (ribbon->end <= 0) {
                        ribbon->start = 8;
                        ribbon->end = 8;
                        ribbon->count++;
                        if (ribbon->count < 5) {
                            ribbon->state = 0;
                        }
                    }
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0x15E0) < (*(Ribbons437Timing *G32 *)(work + 0x15F4))->reset_end) {
            ribbon->state = 0;
            ribbon->count = 0;
            ribbon->start = 0;
            ribbon->end = -(i * 4);
        } else if (MODEL_VARIANT_WORD(work, 0x163C) < 4) {
            if (ribbon->state == 0) {
                ribbon->end += MODEL_VARIANT_WORD(work, 0x15E8);
                if (ribbon->end >= 8) {
                    ribbon->end = 8;
                    ribbon->state = 1;
                }
            } else {
                ribbon->start += MODEL_VARIANT_WORD(work, 0x15E8);
                if (ribbon->start >= 8) {
                    ribbon->start = 0;
                    ribbon->end = 0;
                    ribbon->count++;
                    ribbon->state = 0;
                }
            }
        }
    }
    MODEL_VARIANT_WORD(work, 0x1634) += MODEL_VARIANT_WORD(work, 0x15E8) * 192;
    MODEL_VARIANT_WORD(work, 0x1638) += MODEL_VARIANT_WORD(work, 0x15E8) * 128;
    MODEL_VARIANT_WORD(work, 0x161C) += 0x40;
}
