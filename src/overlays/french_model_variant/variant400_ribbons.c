#include "../../types.h"
#include "variant400_ribbons.h"
#include "variant400_entry.h"

/* The North American header-383 build reads the projected points through
 * pointer arithmetic; the default spelling is the French/Spanish one. */
#ifdef MODEL_VARIANT383_RIBBONS_POINTER_SCREEN
#define RIBBONS400_SA(k) (*(ribbon->sa + (k)))
#define RIBBONS400_SB(k) (*(ribbon->sb + (k)))
#else
#define RIBBONS400_SA(k) (ribbon->sa[k])
#define RIBBONS400_SB(k) (ribbon->sb[k])
#endif

void FrenchModelVariant400_DrawRibbons(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long flags[5][17];
    long p;
    long flag;
    Variant400EntryState *work;
    GsOT *ot;
    s16 i;
    s16 k;
    s32 wave;
    s32 phase;
    s32 yaw;
    s32 pitch;
    s16 len;
    Model400FirstRibbon *ribbon;
    Model400Screen screen;
    POLY_FT4 *poly;
    s32 bend;
    s32 dx;
    s32 dy;

    work = (Variant400EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    pitch = ratan2(work->view_delta.vy, work->view_delta.vx) + 0xC00;
    if (work->tail.phase > 0) {
        if (work->tail.phase < 4) {
            len = 32;
        } else {
            len = work->width / 32;
        }
        ribbon = (Model400FirstRibbon *)work;
        for (i = 0, phase = 0; i < 5; i++, ribbon++, phase = i * 4096 / 5) {
            if (work->tail.phase < 4) {
                setVector(&ribbon->delta,
                          ribbon->position.vx - work->matrix.t[0],
                          ribbon->position.vy - work->matrix.t[1],
                          ribbon->position.vz - work->matrix.t[2]);
                for (k = 0, wave = -work->ribbon_wave; k < 17; k++, wave += 1300) {
                    if (k == 0 || k == 16) {
                        bend = 0;
                    } else {
                        bend = phase - k * 256 + work->ribbon_phase;
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
                work->field_2018 = work->target.vx - ribbon->position.vx;
                work->field_201C = work->target.vy - ribbon->position.vy;
                work->field_2020 = work->target.vz - ribbon->position.vz;
                for (k = 0, wave = -work->ribbon_wave; k < 17; k++, wave += 1300) {
                    if (k == 0 || k == 16) {
                        bend = 0;
                    } else {
                        bend = phase - k * 256 + work->ribbon_phase;
                    }
                    setVector(&ribbon->a[k],
                              ribbon->delta.vx + work->field_2018 * k / 16
                                  + (rcos(pitch) * (rsin(bend) * ((rsin(wave) * 64 >> 12) + 64) >> 12) >> 12),
                              ribbon->delta.vy + work->field_201C * k / 16
                                  + (rsin(pitch) * (rsin(bend) * ((rsin(wave) * 64 >> 12) + 64) >> 12) >> 12),
                              ribbon->delta.vz + work->field_2020 * k / 16);
                    setVector(&ribbon->b[k],
                              ribbon->a[k].vx + (rcos(yaw) * len >> 12),
                              ribbon->a[k].vy,
                              ribbon->a[k].vz + (rsin(yaw) * len >> 12));
                }
            }
        }
        setVector(&rot, 0, 0, 0);
        m.t[0] = work->matrix.t[0];
        m.t[1] = work->matrix.t[1];
        m.t[2] = work->matrix.t[2];
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
                    dx = RIBBONS400_SA(k).point.vx - RIBBONS400_SA(k - 1).point.vx;
                    screen = ribbon->sa[k];
                    dy = screen.point.vy - ribbon->sa[k - 1].point.vy;
                    ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                    ribbon->width[k] = RIBBONS400_SB(k).point.vx - RIBBONS400_SA(k).point.vx;
                    ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                } else {
                    ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                                  &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed,
                                                  &p, &flags[i][k]);
                    RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                    dx = RIBBONS400_SA(k + 1).point.vx - RIBBONS400_SA(k).point.vx;
                    dy = RIBBONS400_SA(k + 1).point.vy - RIBBONS400_SA(k).point.vy;
                    ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                    ribbon->width[k] = RIBBONS400_SB(k).point.vx - RIBBONS400_SA(k).point.vx;
                    ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                }
            }
        }
        ribbon = (Model400FirstRibbon *)work;
        for (i = 0; i < 5; i++, ribbon++) {
            for (k = work->segment_start, poly = work->flat_textured;
                 k < work->segment_end;
                 k++, poly = work->flat_textured) {
                if ((s16)(k % 2) == 1) {
                    poly++;
                }
                if (k + 1 == work->segment_end) {
                    poly->x0 = RIBBONS400_SA(k).point.vx + ribbon->ox[k];
                    poly->y0 = (RIBBONS400_SA(k).packed >> 16) + ribbon->oy[k];
                    poly->x1 = RIBBONS400_SA(k + 1).point.vx;
                    poly->y1 = RIBBONS400_SA(k + 1).packed >> 16;
                    poly->x2 = RIBBONS400_SA(k).point.vx - ribbon->ox[k];
                    poly->y2 = (RIBBONS400_SA(k).packed >> 16) - ribbon->oy[k];
                    poly->x3 = RIBBONS400_SA(k + 1).point.vx;
                    poly->y3 = RIBBONS400_SA(k + 1).packed >> 16;
                } else if (k == work->segment_start) {
                    poly->x0 = RIBBONS400_SA(k).point.vx;
                    poly->y0 = RIBBONS400_SA(k).packed >> 16;
                    poly->x1 = RIBBONS400_SA(k + 1).point.vx + ribbon->ox[k + 1];
                    poly->y1 = (RIBBONS400_SA(k + 1).packed >> 16) + ribbon->oy[k + 1];
                    poly->x2 = RIBBONS400_SA(k).point.vx;
                    poly->y2 = RIBBONS400_SA(k).packed >> 16;
                    poly->x3 = RIBBONS400_SA(k + 1).point.vx - ribbon->ox[k + 1];
                    poly->y3 = (RIBBONS400_SA(k + 1).packed >> 16) - ribbon->oy[k + 1];
                } else {
                    poly->x0 = RIBBONS400_SA(k).point.vx + ribbon->ox[k];
                    poly->y0 = (RIBBONS400_SA(k).packed >> 16) + ribbon->oy[k];
                    poly->x1 = RIBBONS400_SA(k + 1).point.vx + ribbon->ox[k + 1];
                    poly->y1 = (RIBBONS400_SA(k + 1).packed >> 16) + ribbon->oy[k + 1];
                    poly->x2 = RIBBONS400_SA(k).point.vx - ribbon->ox[k];
                    poly->y2 = (RIBBONS400_SA(k).packed >> 16) - ribbon->oy[k];
                    poly->x3 = RIBBONS400_SA(k + 1).point.vx - ribbon->ox[k + 1];
                    poly->y3 = (RIBBONS400_SA(k + 1).packed >> 16) - ribbon->oy[k + 1];
                }
                poly->r0 = ribbon->color[0];
                poly->g0 = ribbon->color[1];
                poly->b0 = ribbon->color[2];
                if (ribbon->otz[k] >= 0 && flags[i][k] >= 0) {
                    GsSortPoly(poly, ot, ribbon->otz[k]);
                }
            }
        }
        if (work->tail.phase == 1) {
            if (work->segment_end < 16) {
                work->segment_end += (u32)work->step * 3 / 2;
                if (work->segment_end >= 16) {
                    work->segment_end = 16;
                    work->tail.phase = 2;
                }
            }
        } else if (work->tail.phase == 2) {
            if (work->segment_start < 16) {
                work->segment_start += (u32)work->step * 3 / 2;
                if (work->segment_start >= 16) {
                    work->segment_start = 16;
                    work->segment_end = 16;
                }
            }
        } else if (work->tail.phase == 3) {
            work->segment_start = 0;
            work->segment_end = 0;
        } else if (work->tail.phase == 4) {
            if (work->segment_end < 16) {
                work->segment_end += (u32)work->step * 3 / 2;
                if (work->segment_end >= 16) {
                    work->segment_end = 16;
                    work->tail.phase = 5;
                }
            }
        } else if (work->tail.phase >= 6) {
            if (work->segment_start < 16) {
                work->segment_start += (u32)work->step * 3 / 2;
                if (work->segment_start >= 16) {
                    work->segment_start = 16;
                    work->segment_end = 16;
                    work->tail.phase = 7;
                }
            }
        } else if (work->tail.phase == 10) {
            if (work->width > 0) {
                work->width -= work->step * 8;
                if (work->width <= 0) {
                    work->width = 0;
                }
            }
        }
    }
    work->ribbon_wave += work->step * 400;
    work->ribbon_phase += work->step * 384;
}
