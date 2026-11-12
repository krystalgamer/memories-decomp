#include "../../types.h"
#include "variant443_ribbons.h"
#include "variant443_entry.h"

/* Header 426 reads its projected points through pointer arithmetic and
 * packed-word halves (see variant423_ribbon.c). */
#ifdef MODEL_VARIANT443_RIBBONS_PACKED_SCREEN
#define RIBBONS443_SCREEN(screen, n) (*((screen) + (n)))
#define RIBBONS443_VX(screen, n) ((s16)RIBBONS443_SCREEN(screen, n).packed)
#define RIBBONS443_VY(screen, n) ((s16)(RIBBONS443_SCREEN(screen, n).packed >> 16))
#else
#define RIBBONS443_SCREEN(screen, n) ((screen)[n])
#define RIBBONS443_VX(screen, n) ((screen)[n].point.vx)
#define RIBBONS443_VY(screen, n) ((screen)[n].point.vy)
#endif

void FrenchModelVariant443_DrawRibbons(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long flags[3][17];
    long p;
    long flag;
    EntryState443 *work;
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

    work = (EntryState443 *)ctx;
    sheet = work->sheets;
    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    pitch = ratan2(work->view_delta.vy, work->view_delta.vx) + 0xC00;
    len = sheet->size * 48 / 4096;
    ribbon = (Model400FirstRibbon *)work;
    for (i = 0, phase = 0; i < 3; i++, ribbon++, phase = i * 4096 / 3) {
        setVector(&ribbon->delta,
                  ((Ribbons443State *)work)->origins[i].vx - work->origin.vx,
                  ((Ribbons443State *)work)->origins[i].vy - work->origin.vy,
                  ((Ribbons443State *)work)->origins[i].vz - work->origin.vz);
        for (k = 0, wave = work->ribbon_wave; k < 17; k++, wave += 1300) {
            if (k == 0 || k == 16) {
                bend = 0;
            } else {
                bend = phase - k * 256 - work->ribbon_phase;
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
    m.t[0] = work->origin.vx;
    m.t[1] = work->origin.vy;
    m.t[2] = work->origin.vz;
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
                                              &RIBBONS443_SCREEN(ribbon->sa, k - 1).packed, &RIBBONS443_SCREEN(ribbon->sa, k).packed,
                                              &RIBBONS443_SCREEN(ribbon->sa, k - 1).packed, &RIBBONS443_SCREEN(ribbon->sa, k).packed,
                                              &p, &flags[i][k]);
                RotTransPers(&ribbon->b[k], &RIBBONS443_SCREEN(ribbon->sb, k).packed, &p, &flag);
                dx = RIBBONS443_VX(ribbon->sa, k) - RIBBONS443_VX(ribbon->sa, k - 1);
                dy = RIBBONS443_VY(ribbon->sa, k) - RIBBONS443_VY(ribbon->sa, k - 1);
                ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                ribbon->width[k] = RIBBONS443_VX(ribbon->sb, k) - RIBBONS443_VX(ribbon->sa, k);
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                              &RIBBONS443_SCREEN(ribbon->sa, k).packed, &RIBBONS443_SCREEN(ribbon->sa, k + 1).packed,
                                              &RIBBONS443_SCREEN(ribbon->sa, k).packed, &RIBBONS443_SCREEN(ribbon->sa, k + 1).packed,
                                              &p, &flags[i][k]);
                RotTransPers(&ribbon->b[k], &RIBBONS443_SCREEN(ribbon->sb, k).packed, &p, &flag);
                dx = RIBBONS443_VX(ribbon->sa, k + 1) - RIBBONS443_VX(ribbon->sa, k);
                dy = RIBBONS443_VY(ribbon->sa, k + 1) - RIBBONS443_VY(ribbon->sa, k);
                ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                ribbon->width[k] = RIBBONS443_VX(ribbon->sb, k) - RIBBONS443_VX(ribbon->sa, k);
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            }
        }
    }
    ribbon = (Model400FirstRibbon *)work;
    for (i = 0; i < 3; i++, ribbon++) {
        for (k = work->ribbon_start, poly = work->additional;
             k < 16;
             k++, poly = work->additional) {
            if ((s16)(k % 2) == 1) {
                poly++;
            }
            poly->x0 = RIBBONS443_SCREEN(ribbon->sa, k).point.vx + ribbon->ox[k];
            poly->y0 = (RIBBONS443_SCREEN(ribbon->sa, k).packed >> 16) + ribbon->oy[k];
            poly->x1 = RIBBONS443_SCREEN(ribbon->sa, k + 1).point.vx + ribbon->ox[k + 1];
            poly->y1 = (RIBBONS443_SCREEN(ribbon->sa, k + 1).packed >> 16) + ribbon->oy[k + 1];
            poly->x2 = RIBBONS443_SCREEN(ribbon->sa, k).point.vx - ribbon->ox[k];
            poly->y2 = (RIBBONS443_SCREEN(ribbon->sa, k).packed >> 16) - ribbon->oy[k];
            poly->x3 = RIBBONS443_SCREEN(ribbon->sa, k + 1).point.vx - ribbon->ox[k + 1];
            poly->y3 = (RIBBONS443_SCREEN(ribbon->sa, k + 1).packed >> 16) - ribbon->oy[k + 1];
            poly->r0 = ribbon->color[0];
            poly->g0 = ribbon->color[1];
            poly->b0 = ribbon->color[2];
            if (ribbon->otz[k] >= 0 && flags[i][k] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[k]);
            }
        }
    }
    if (work->phase == 2 && work->ribbon_start > 0) {
        timing = *(Ribbons443Timing **)((u8 *)work + 0x3634);
        work->ribbon_start = 16 - ((u32)work->elapsed - timing->spread_start) * 16
                                                    / (timing->spread_end - timing->spread_start);
        if (work->ribbon_start <= 0) {
            work->ribbon_start = 0;
            work->phase = 3;
        }
    }
    work->ribbon_wave += work->step * 192;
    work->ribbon_phase += work->step * 128;
}
