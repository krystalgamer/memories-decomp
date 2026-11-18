#include "../../types.h"
#include "h410_entry_pal.h"

/* The North American header-410 build reads the projected points through
 * pointer arithmetic so CDK GCC folds sa[k + 1] into a fixed offset from
 * sa[k]; the default spelling is the French/Spanish one. */
#ifdef MODEL_VARIANT410_RIBBONS_POINTER_SCREEN
#define RIBBONS427_SA(k, n) (*(ribbon->sa + (k) + (n)))
#define RIBBONS427_SB(k) (*(ribbon->sb + (k)))
#else
#define RIBBONS427_SA(k, n) (ribbon->sa[(k) + (n)])
#define RIBBONS427_SB(k) (ribbon->sb[k])
#endif

void FrenchModelVariant427_DrawRibbons(EntryState427 *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    EntryState427 *work;
    GsOT *ot;
    s16 i;
    s16 k;
    s32 yaw;
    s32 pitch;
    s16 len;
    EntryRibbon427 *ribbon;
    POLY_GT4 *poly;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 sy;
    EntryConfig427 *timing;
    s32 n;
    s32 kk;
    s32 py, pz;

    work = context;
    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    pitch = ratan2(work->view_delta.vy, work->view_delta.vx) + 0xC00;
    poly = work->packets.textured;
    if (!(work->clock.frame_count & 1)) {
        len = work->width / 16;
    } else {
        len = work->width * 72 / 1024;
    }
    ribbon = work->ribbons;
    for (i = 0; i < 3; i++, ribbon++) {
        if (ribbon->progress > 0) {
            if (i < 0) {
                dy = 0;
            }
            for (k = 0; k < 17; k++) {
                dy = work->ribbon_directions[i].vy * ribbon->progress / 1024;
                dz = work->ribbon_directions[i].vz * ribbon->progress / 1024;
                n = kk = k;
                setVector(&ribbon->a[n], work->ribbon_directions[i].vx * ribbon->progress / 1024 * k / 16, dy * (kk = n) / 16, dz * k / 16);
                setVector(&ribbon->b[k],
                          ribbon->a[k].vx + (rcos(yaw) * len >> 12),
                          ribbon->a[k].vy,
                          ribbon->a[k].vz + (rsin(yaw) * len >> 12));
            }
        }
        setVector(&rot, 0, 0, 0);
        m.t[0] = work->matrices[i % 3].t[0];
        m.t[1] = work->matrices[i % 3].t[1];
        m.t[2] = work->matrices[i % 3].t[2];
        setVector(&scale, 4096, 4096, 4096);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        if (ribbon->progress > 0) {
            for (k = 0; k < 17; k++) {
                if (k == 16) {
                    ribbon->offsets.otz[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k], &ribbon->a[k - 1], &ribbon->a[k],
                                                  &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k - 1].packed, &ribbon->sa[k].packed,
                                                  &p, &ribbon->flags[k]);
                    RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                    dx = RIBBONS427_SA(k, 0).point.vx - RIBBONS427_SA(k, -1).point.vx;
                    sy = (RIBBONS427_SA(k, 0).packed >> 16) - (RIBBONS427_SA(k, -1).packed >> 16);
                    ribbon->angle[k] = ratan2(sy, dx) - 0x400;
                    ribbon->width[k] = RIBBONS427_SB(k).point.vx - RIBBONS427_SA(k, 0).point.vx;
                    ribbon->offsets.ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->offsets.oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                } else {
                    ribbon->offsets.otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                                  &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed, &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed,
                                                  &p, &ribbon->flags[k]);
                    RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, &p, &flag);
                    dx = RIBBONS427_SA(k, 1).point.vx - RIBBONS427_SA(k, 0).point.vx;
                    sy = RIBBONS427_SA(k, 1).point.vy - RIBBONS427_SA(k, 0).point.vy;
                    ribbon->angle[k] = ratan2(sy, dx) - 0x400;
                    ribbon->width[k] = RIBBONS427_SB(k).point.vx - RIBBONS427_SA(k, 0).point.vx;
                    ribbon->offsets.ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->offsets.oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                }
            }
        }
    }
    ribbon = work->ribbons;
    for (i = 0; i < 3; i++, ribbon++) {
        if (ribbon->progress > 0) {
            for (k = 0; k < 16; k++) {
                setRGB0(poly, 0, 192, 0);
                setRGB1(poly, 0, 192, 0);
                setRGB2(poly, 192, 192, 192);
                setRGB3(poly, 192, 192, 192);
                poly->x0 = RIBBONS427_SA(k, 0).point.vx + ribbon->offsets.ox[k];
                poly->y0 = (RIBBONS427_SA(k, 0).packed >> 16) + ribbon->offsets.oy[k];
                poly->x1 = RIBBONS427_SA(k, 1).point.vx + ribbon->offsets.ox[k + 1];
                poly->y1 = (RIBBONS427_SA(k, 1).packed >> 16) + ribbon->offsets.oy[k + 1];
                poly->x2 = RIBBONS427_SA(k, 0).point.vx;
                poly->y2 = (RIBBONS427_SA(k, 0).packed >> 16);
                poly->x3 = RIBBONS427_SA(k, 1).point.vx;
                poly->y3 = (RIBBONS427_SA(k, 1).packed >> 16);
                if (ribbon->offsets.otz[k] >= 0 && ribbon->flags[k] >= 0) {
                    GsSortPoly(poly, ot, (u16)ribbon->offsets.otz[k]);
                }
                poly->x0 = RIBBONS427_SA(k, 0).point.vx - ribbon->offsets.ox[k];
                poly->y0 = (RIBBONS427_SA(k, 0).packed >> 16) - ribbon->offsets.oy[k];
                poly->x1 = RIBBONS427_SA(k, 1).point.vx - ribbon->offsets.ox[k + 1];
                poly->y1 = (RIBBONS427_SA(k, 1).packed >> 16) - ribbon->offsets.oy[k + 1];
                poly->x2 = RIBBONS427_SA(k, 0).point.vx;
                poly->y2 = (RIBBONS427_SA(k, 0).packed >> 16);
                poly->x3 = RIBBONS427_SA(k, 1).point.vx;
                poly->y3 = (RIBBONS427_SA(k, 1).packed >> 16);
                if (ribbon->offsets.otz[k] >= 0 && ribbon->flags[k] >= 0) {
                    GsSortPoly(poly, ot, (u16)ribbon->offsets.otz[k]);
                }
            }
        }
        if (ribbon->progress < 0x400) {
            ribbon->progress += work->clock.step * 64;
            if (ribbon->progress >= 0x400) {
                ribbon->progress = 0x400;
                if (work->phase == 2) {
                    work->phase = 3;
                }
            }
        }
    }
    work->ribbon_sway += work->clock.step * 850;
    work->ribbon_wave += work->clock.step * 128;
    timing = work->config;
    if (timing->fade_start <= work->clock.frame && work->width > 0) {
        work->width = 0x400 - ((work->clock.frame - timing->fade_start) << 10) / (timing->fade_end - timing->fade_start);
        if (work->width <= 0) {
            work->width = 0;
            if (work->phase == 4) {
                work->phase = 5;
            }
        }
    }
}
