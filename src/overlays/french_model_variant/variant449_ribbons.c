#include "../../types.h"
#include "variant449_ribbons.h"

void func_8013C240(u8 *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[4][17];
    PSXLONG interpolation;
    PSXLONG flag;
    Variant449RibbonView *work;
    GsOT *ot;
    s16 i;
    s16 k;
    s32 sway;
    s32 phase;
    s32 base;
    s32 turn;
    s32 bend;
    s16 len;
    s32 first;
    s32 last;
    Variant449Ribbon *ribbon;
    POLY_FT4 *poly;
    s32 dx, dy;

    work = (Variant449RibbonView *)context;
    ot = func_80058F10();
    ribbon = work->ribbons;
    i = 0;
    base = 0;
    turn = ratan2(work->axis_z, work->axis_x) + 3072;
    ratan2(work->axis_y, work->axis_z);
    len = 16;
    for (; i < 4; i++, ribbon++, base = i * 1024) {
        if (!(i & 1)) {
            phase = -work->ripple;
        } else {
            phase = work->ripple;
        }
        for (k = 0; k < 17; k++, phase += 1300) {
            if (k == 0 || k == 16) {
                sway = 0;
            } else if (!(i & 1)) {
                sway = base - k * 384 + work->ripple2;
            } else {
                sway = base - k * 384 - work->ripple2;
            }
            bend = rsin(sway) * ((rsin(phase) * 16 >> 12) + 32) >> 12;
            setVector(&ribbon->a[k],
                      work->direction.vx * k / 16,
                      work->direction.vy * k / 16 + (rcos(base + work->spin) * bend >> 12),
                      work->direction.vz * k / 16 + (rsin(base + work->spin) * bend >> 12));
            setVector(&ribbon->b[k],
                      ribbon->a[k].vx + (rcos(turn) * len >> 12),
                      ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(turn) * len >> 12));
        }
    }
    setVector(&rot, 0, 0, 0);
    m.t[0] = work->center[0];
    m.t[1] = work->center[1];
    m.t[2] = work->center[2];
    setVector(&scale, 4096, 4096, 4096);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    ribbon = work->ribbons;
    for (i = 0; i < 4; i++, ribbon++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k], &ribbon->a[k - 1], &ribbon->a[k],
                                              &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed,
                                              &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed,
                                              &interpolation, &flags[i][k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &interpolation, &flag);
                dx = ribbon->sa[k].point.vx - ribbon->sa[k - 1].point.vx;
                dy = ribbon->sa[k].point.vy - ribbon->sa[k - 1].point.vy;
                ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                              &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed,
                                              &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed,
                                              &interpolation, &flags[i][k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &interpolation, &flag);
                dx = ribbon->sa[k + 1].point.vx - ribbon->sa[k].point.vx;
                dy = ribbon->sa[k + 1].point.vy - ribbon->sa[k].point.vy;
                ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            }
        }
    }
    ribbon = work->ribbons;
    for (i = 0; i < 4; i++, ribbon++) {
        first = 0;
        if (ribbon->start >= 0) {
            first = ribbon->start;
        }
        if (ribbon->end < first) {
            last = first;
        } else {
            last = ribbon->end;
        }
        for (k = first, poly = work->packets; k < last; k++, poly = work->packets) {
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
        if (!(i & 1)) {
            if (ribbon->state == 0) {
                ribbon->end += work->step;
                if (ribbon->end >= 16) {
                    ribbon->end = 16;
                    ribbon->state = 1;
                }
            } else if (ribbon->state == 1) {
                ribbon->start += work->step;
                if (ribbon->start >= 16) {
                    if (work->timing->scale_end <= work->elapsed) {
                        ribbon->state = 2;
                        ribbon->start = 16;
                        ribbon->end = 16;
                    } else {
                        ribbon->state = 0;
                        ribbon->start = 0;
                        ribbon->end = 0;
                        ribbon->count++;
                    }
                }
            }
        } else if (ribbon->state == 0) {
            ribbon->start -= work->step;
            if (ribbon->start <= 0) {
                ribbon->start = 0;
                ribbon->state = 1;
            }
        } else if (ribbon->state == 1) {
            ribbon->end -= work->step;
            if (ribbon->end <= 0) {
                if (work->timing->scale_end <= work->elapsed) {
                    ribbon->state = 2;
                    ribbon->start = 0;
                    ribbon->end = 0;
                } else {
                    ribbon->state = 0;
                    ribbon->start = 16;
                    ribbon->end = 16;
                    ribbon->count++;
                }
            }
        }
    }
    work->ripple += work->step * 192;
    work->ripple2 += work->step << 7;
    work->spin += 64;
}
