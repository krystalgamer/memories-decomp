#include "../../types.h"
#include "../model_variant/variant446_entry.h"

/* The North American header-446 build reads the projected points through
 * pointer arithmetic and tests j before resetting a spent ribbon; the
 * default spelling is the French/Spanish one. */
#ifdef MODEL_VARIANT446_RIBBONS_POINTER_SCREEN
#define RIBBONS463_SA(k) (*(ribbon->sa + (k)))
#define RIBBONS463_SB(k) (*(ribbon->sb + (k)))
#else
#define RIBBONS463_SA(k) (ribbon->sa[k])
#define RIBBONS463_SB(k) (ribbon->sb[k])
#endif

void FrenchModelVariant463_DrawRibbons(Variant446EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long flags[5][13];
    long p;
    long flag;
    Variant446EntryState *work;
    GsOT *ot;
    s16 i;
    s16 j;
    s32 wave;
    s32 base;
    s32 total;
    s32 turn;
    s16 length;
    s32 phase;
    s32 size;
    s32 amp;
    s32 sway;
    s32 dx;
    s32 dy;
    s32 n;
    Variant446EntryRibbon *ribbon;
    POLY_FT4 *poly;

    work = context;
    ot = Graphics_GetDefaultOrderingTable();
    total = 0;
    i = 0;
    base = 0;
    length = 48;
    ribbon = work->ribbons;
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    ratan2(work->view_delta.vy, work->view_delta.vx);
    for (; i < 5; ribbon++) {
        if (i < 0) {
            dy = 0;
        }
        for (j = 0; j < 13; j++) {
            if (j < 0) {
                if (j == -1) {
                    dy = 0;
                }
            }
            setVector(&ribbon->a[j], work->spans[i].vx * j / 12,
                      work->spans[i].vy * j / 12, work->spans[i].vz * j / 12);
            setVector(&ribbon->b[j], ribbon->a[j].vx + (rcos(turn) * length >> 12), ribbon->a[j].vy,
                      ribbon->a[j].vz + (rsin(turn) * length >> 12));
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->bases[i].vx;
        m.t[1] = work->bases[i].vy;
        m.t[2] = work->bases[i].vz;
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
        wave = -work->ribbon_wave;
        for (j = 0; j < 13; j++, wave += 1300) {
            if (j == 12) {
                ribbon->otz[j] = RotTransPers4(&ribbon->a[11], &ribbon->a[12], &ribbon->a[11], &ribbon->a[12],
                                               &ribbon->sa[11].packed, &ribbon->sa[12].packed,
                                               &ribbon->sa[11].packed, &ribbon->sa[12].packed, &p,
                                               &flags[i][j]);
                RotTransPers(&ribbon->b[12], &ribbon->sb[12].packed, &p, &flag);
                dx = RIBBONS463_SA(12).point.vx - RIBBONS463_SA(11).point.vx;
                dy = RIBBONS463_SA(12).point.vy - RIBBONS463_SA(11).point.vy;
            } else {
                ribbon->otz[j] = RotTransPers4(&ribbon->a[j], &ribbon->a[j + 1], &ribbon->a[j], &ribbon->a[j + 1],
                                               &ribbon->sa[j].packed, &ribbon->sa[j + 1].packed,
                                               &ribbon->sa[j].packed, &ribbon->sa[j + 1].packed, &p,
                                               &flags[i][j]);
                RotTransPers(&ribbon->b[j], &ribbon->sb[j].packed, &p, &flag);
                dx = RIBBONS463_SA(j + 1).point.vx - RIBBONS463_SA(j).point.vx;
                dy = RIBBONS463_SA(j + 1).point.vy - RIBBONS463_SA(j).point.vy;
            }
            ribbon->angle[j] = ratan2(dy, dx) - 0x400;
            ribbon->width[j] = RIBBONS463_SB(j).point.vx - RIBBONS463_SA(j).point.vx;
            size = ribbon->width_scale * ribbon->width[j] / 1024;
            ribbon->ox[j] = rcos(ribbon->angle[j]) * size >> 12;
            ribbon->oy[j] = rsin(ribbon->angle[j]) * size >> 12;
            if (j == 0 || j == 12) {
                phase = 0;
            } else {
                phase = base - j * 512 + work->ribbon_phase;
            }
            amp = ribbon->width[j] * 64 / 48;
            sway = ribbon->width[j] * 32 / 48;
            RIBBONS463_SA(j).packed = (u32)RIBBONS463_SA(j).packed + (u32)(rcos(ribbon->angle[j]) * ((rsin(phase) * amp >> 12) + (rsin(wave) * sway >> 12)) >> 12);
            RIBBONS463_SA(j).packed = (u32)RIBBONS463_SA(j).packed + ((u32)(rsin(ribbon->angle[j]) * ((rsin(phase) * amp >> 12) + (rsin(wave) * sway >> 12)) >> 12) << 16);
        }
        if (ribbon->progress < 0) {
            n = 0;
        } else {
            n = ribbon->progress;
        }
        poly = work->flat_textured;
        for (j = 0; j < n; j++) {
            if (work->clock.frame_count % 2 == 1) {
                if (!(j & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
            poly->x0 = RIBBONS463_SA(j).point.vx + ribbon->ox[j];
            poly->y0 = (RIBBONS463_SA(j).packed >> 16) + ribbon->oy[j];
            poly->x1 = RIBBONS463_SA(j + 1).point.vx + ribbon->ox[j + 1];
            poly->y1 = (RIBBONS463_SA(j + 1).packed >> 16) + ribbon->oy[j + 1];
            poly->x2 = RIBBONS463_SA(j).point.vx - ribbon->ox[j];
            poly->y2 = (RIBBONS463_SA(j).packed >> 16) - ribbon->oy[j];
            poly->x3 = RIBBONS463_SA(j + 1).point.vx - ribbon->ox[j + 1];
            poly->y3 = (RIBBONS463_SA(j + 1).packed >> 16) - ribbon->oy[j + 1];
            poly->r0 = ribbon->ca[0];
            poly->g0 = ribbon->ca[1];
            poly->b0 = ribbon->ca[2];
            if (ribbon->otz[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[j]);
            }
            if (!(work->clock.frame_count & 1)) {
                if (!(j & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
        }
        if (ribbon->progress < 12 && ribbon->state == 0) {
            ribbon->progress += work->clock.step * 2;
            if (ribbon->progress >= 12) {
                ribbon->progress = 12;
                ribbon->state = 1;
            }
        } else if (ribbon->state == 1 && ribbon->width_scale > 0) {
            ribbon->width_scale -= work->clock.step * 64;
            if (ribbon->width_scale <= 0) {
                if (work->phase >= 3) {
                    ribbon->width_scale = 0;
                    ribbon->progress = 0;
                    ribbon->state = 2;
#ifdef MODEL_VARIANT446_RIBBONS_POINTER_SCREEN
                    /* Two identical arms: the North American registers need the test. */
                } else if (j) {
                    ribbon->width_scale = 1024;
                    ribbon->progress = -50;
                    ribbon->state = 0;
#endif
                } else {
                    ribbon->width_scale = 1024;
                    ribbon->progress = -50;
                    ribbon->state = 0;
                }
            }
        }
        total += ribbon->state;
        if (total == 10 && work->phase == 3) {
            work->phase = 4;
        }
        i++;
        base = i * 0x1800 / 5;
    }
    work->ribbon_wave += work->clock.step * 128;
    work->ribbon_phase += work->clock.step * 64;
}
