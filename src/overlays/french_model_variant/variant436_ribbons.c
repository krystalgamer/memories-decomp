#include "../../types.h"
#include "variant436_ribbons.h"
#include "variant436_entry.h"

/* The North American header-419 build reads the projected points through
 * pointer arithmetic, except for the closing segment's two differences;
 * the default spelling is the French/Spanish one. */
#ifdef MODEL_VARIANT419_RIBBONS_POINTER_SCREEN
#define RIBBONS436_SA(k) (*(ribbon->sa + (k)))
#define RIBBONS436_SB(k) (*(ribbon->sb + (k)))
#else
#define RIBBONS436_SA(k) (ribbon->sa[k])
#define RIBBONS436_SB(k) (ribbon->sb[k])
#endif

void FrenchModelVariant436_DrawRibbons(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long flags[4][9];
    long p;
    long flag;
    EntryState436 *work;
    GsOT *ot;
    s16 i;
    s16 k;
    s32 wave;
    s32 phase;
    s32 yaw;
    s32 pitch;
    s16 len;
    Ribbons436Ribbon *ribbon;
    POLY_FT4 *poly;
    Ribbons436Screen screen;
    s32 bend;
    s32 curve;
    s32 amp;
    s32 dist;
    s32 dx;
    s32 dy;
    s32 lo;
    s32 hi;
    s32 odd;

    work = (EntryState436 *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    pitch = ratan2(work->view_delta.vy, work->view_delta.vz) + 0xC00;
    if (work->tail.phase > 0) {
        len = work->field_1610 * 10 / 1024;
    } else {
        len = 10;
    }
    ribbon = (Ribbons436Ribbon *)work;
    for (i = 0, phase = 0; i < 4; i++, ribbon++, phase = i * 1024) {
        if (work->elapsed < (*(Ribbons436Timing **)((u8 *)work + 0x15F4))->wave_end && !(i & 1)) {
            wave = -work->phase_a;
        } else {
            wave = work->phase_a;
        }
        for (k = 0, odd = i & 1; k < 9; k++, wave += 1300) {
            if (work->tail.phase <= 0) {
                if (k == 0 || k == 8) {
                    bend = 0;
                } else if (!odd) {
                    bend = phase - k * 768 + work->phase_b;
                } else {
                    bend = phase - k * 768 - work->phase_b;
                }
                amp = rsin(bend) * ((rsin(wave) * 16 >> 12) + 32) >> 12;
                setVector(&ribbon->a[k],
                          work->direction.vx * k / 8,
                          work->direction.vy * k / 8 + (rcos(phase + work->spin) * amp >> 12) - 24,
                          work->direction.vz * k / 8 + (rsin(phase + work->spin) * amp >> 12));
            } else {
                if (k == 0 || k == 8) {
                    curve = 256;
                    bend = 0;
                } else if (!odd) {
                    curve = k * 128;
                    bend = k * 768 - work->phase_b;
                } else {
                    curve = k * 128;
                    bend = k * 768 - work->phase_b;
                }
                dist = work->lift / 4 * (0x400 - work->factor) / 1024;
                amp = (rsin(curve) * 64 >> 12) + (rsin(bend) * ((rsin(wave) * 16 >> 12) + 32) >> 12);
                setVector(&ribbon->a[k],
                          rcos(phase + work->spin) * amp >> 12,
                          -dist * k / 8,
                          rsin(phase + work->spin) * amp >> 12);
            }
            setVector(&ribbon->b[k],
                      ribbon->a[k].vx + (rcos(yaw) * len >> 12),
                      ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(yaw) * len >> 12));
        }
    }
    setVector(&rot, 0, 0, 0);
    m.t[0] = work->origin_a.t[0];
    m.t[1] = work->origin_a.t[1];
    m.t[2] = work->origin_a.t[2];
    setVector(&scale, 4096, 4096, 4096);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    ribbon = (Ribbons436Ribbon *)work;
    for (i = 0; i < 4; i++, ribbon++) {
        for (k = 0; k < 9; k++) {
            if (k == 8) {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k], &ribbon->a[k - 1], &ribbon->a[k],
                                              &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed,
                                              &p, &flags[i][k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                dx = ribbon->sa[k].point.vx - ribbon->sa[k - 1].point.vx;
                screen = ribbon->sa[k];
                dy = screen.point.vy - ribbon->sa[k - 1].point.vy;
                ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                ribbon->width[k] = RIBBONS436_SB(k).point.vx - RIBBONS436_SA(k).point.vx;
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                              &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed,
                                              &p, &flags[i][k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                dx = RIBBONS436_SA(k + 1).point.vx - RIBBONS436_SA(k).point.vx;
                dy = RIBBONS436_SA(k + 1).point.vy - RIBBONS436_SA(k).point.vy;
                ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                ribbon->width[k] = RIBBONS436_SB(k).point.vx - RIBBONS436_SA(k).point.vx;
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            }
        }
    }
    ribbon = (Ribbons436Ribbon *)work;
    for (i = 0; i < 4; i++, ribbon++) {
        lo = 0;
        if (ribbon->start >= 0) {
            lo = ribbon->start;
        }
        hi = ribbon->end < lo ? lo : ribbon->end;
        for (k = lo, poly = work->extra; k < hi; k++, poly = work->extra) {
            if ((s16)(k % 2) == 1) {
                poly++;
            }
            poly->x0 = RIBBONS436_SA(k).point.vx + ribbon->ox[k];
            poly->y0 = (RIBBONS436_SA(k).packed >> 16) + ribbon->oy[k];
            poly->x1 = RIBBONS436_SA(k + 1).point.vx + ribbon->ox[k + 1];
            poly->y1 = (RIBBONS436_SA(k + 1).packed >> 16) + ribbon->oy[k + 1];
            poly->x2 = RIBBONS436_SA(k).point.vx - ribbon->ox[k];
            poly->y2 = (RIBBONS436_SA(k).packed >> 16) - ribbon->oy[k];
            poly->x3 = RIBBONS436_SA(k + 1).point.vx - ribbon->ox[k + 1];
            poly->y3 = (RIBBONS436_SA(k + 1).packed >> 16) - ribbon->oy[k + 1];
            poly->r0 = ribbon->color[0];
            poly->g0 = ribbon->color[1];
            poly->b0 = ribbon->color[2];
            if (ribbon->otz[k] >= 0 && flags[i][k] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[k]);
            }
        }
        if (work->tail.phase == 0) {
            if (!(i & 1)) {
                if (ribbon->state == 0) {
                    ribbon->end += work->step / 2;
                    if (ribbon->end >= 8) {
                        ribbon->end = 8;
                        ribbon->state = 1;
                    }
                } else {
                    ribbon->start += work->step / 2;
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
                    ribbon->start -= work->step / 2;
                    if (ribbon->start <= 0) {
                        ribbon->start = 0;
                        ribbon->state = 1;
                    }
                } else {
                    ribbon->end -= work->step / 2;
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
        } else if (work->tail.phase == 1) {
            ribbon->state = 0;
            ribbon->count = 0;
            ribbon->start = 0;
            if (!(i & 1)) {
                ribbon->end = -(i * 2);
            } else {
                ribbon->end = i * 2;
            }
            if (i + 1 == 4) {
                work->tail.phase = 2;
            }
        } else if (work->tail.phase < 4) {
            if (ribbon->state == 0) {
                ribbon->end += work->step;
                if (ribbon->end >= 8) {
                    ribbon->end = 8;
                    ribbon->state = 1;
                }
            } else {
                ribbon->start += work->step;
                if (ribbon->start >= 8) {
                    if ((*(Ribbons436Timing **)((u8 *)work + 0x15F4))->loop_end <= work->elapsed) {
                        ribbon->start = 8;
                        ribbon->end = 8;
                        ribbon->count++;
                    } else {
                        ribbon->start = 0;
                        ribbon->end = 0;
                        ribbon->count++;
                        ribbon->state = 0;
                    }
                }
            }
        }
    }
    work->phase_a += work->step * 192;
    work->phase_b += work->step * 128;
    work->spin += 0x40;
}
