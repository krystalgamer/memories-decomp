#include "../../types.h"
#include "variant335_ribbons.h"
#include "variant335_entry.h"

/* Header 318 projects its seventeenth point with fixed indices and reaches
 * each segment's screen points through the current element's address; the
 * defaults keep header 335's previous-point walk and direct indexing. */
#ifdef MODEL_VARIANT335_RIBBONS_SCREEN_ROW
#define MODEL_VARIANT335_RIBBON_SCREEN(ribbon, k) (&(ribbon)->sa[k])[0]
#define MODEL_VARIANT335_RIBBON_NEXT_SCREEN(ribbon, k) (&(ribbon)->sa[k])[1]
#else
#define MODEL_VARIANT335_RIBBON_SCREEN(ribbon, k) (ribbon)->sa[k]
#define MODEL_VARIANT335_RIBBON_NEXT_SCREEN(ribbon, k) (ribbon)->sa[(k) + 1]
#endif

void FrenchModelVariant335_DrawRibbons(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Model335EntryState *work;
    ModelVariantSheet *sheet;
    GsOT *ot;
    s16 i;
    s32 wave;
    s32 phase;
    s32 yaw;
    s32 angle;
    s16 len;
    Model335Ribbon *ribbon;
    POLY_FT4 *poly;
    s16 k;
    s32 bend;
    s32 radius;
    s32 dx;
    s32 dy;
    DVECTOR *previous;
    long *projection;

    work = (Model335EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 3072;
    ratan2(work->aim.view_delta.vy, work->aim.view_delta.vx);
    work->origin.vx = work->matrix.t[0]
        + (rsin(work->travel_angle) * work->aim.direction.vx >> 12);
    work->origin.vy = work->matrix.t[1] - work->lift;
    work->origin.vz = work->matrix.t[2]
        + (rsin(work->travel_angle) * work->aim.direction.vz >> 12);
    work->end.vx = work->origin.vx;
    work->end.vy = 0;
    work->end.vz = work->origin.vz;
    work->delta.vx = work->end.vx - work->origin.vx;
    work->delta.vy = work->end.vy - work->origin.vy;
    work->delta.vz = work->end.vz - work->origin.vz;
    sheet = (ModelVariantSheet *)work->sheets;
    len = work->width * 48 / 1024;
    ribbon = (Model335Ribbon *)work;
    for (i = 0, phase = 0; i < 3; i++, ribbon++, phase = i * 2048 / 3) {
        angle = yaw + i * 4096 / 3 + work->spin;
        for (k = 0, wave = -work->wave0; k < 17; k++, wave += 1300) {
            radius = (16 - k) * 32;
            if (k == 0 || k == 16) {
                bend = 0;
            } else {
                bend = phase - k * 384 + work->wave1;
            }
            {
                s32 point_index = k;
                setVector(&ribbon->a[point_index],
                      work->delta.vx * point_index / 16
                          + (rcos(angle) * (radius + (rsin(bend) * 96 >> 12) + (rsin(wave) * 48 >> 12)) >> 12),
                      work->delta.vy * point_index / 16,
                      work->delta.vz * point_index / 16
                          + (rsin(angle) * (radius + (rsin(bend) * 96 >> 12) + (rsin(wave) * 48 >> 12)) >> 12));
                setVector(&ribbon->b[point_index],
                      ribbon->a[point_index].vx + (rcos(yaw) * len >> 12),
                      ribbon->a[point_index].vy,
                      ribbon->a[point_index].vz + (rsin(yaw) * len >> 12));
            }
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
    ribbon = (Model335Ribbon *)work;
#ifdef MODEL_VARIANT335_RIBBONS_FIXED_TIP
    for (i = 0, projection = &p; i < 3; i++, ribbon++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                ribbon->otz[16] = RotTransPers4(&ribbon->a[15], &ribbon->a[16], &ribbon->a[15], &ribbon->a[16],
                                               &ribbon->sa[15].packed, &ribbon->sa[16].packed,
                                               &ribbon->sa[15].packed, &ribbon->sa[16].packed,
                                               projection, &ribbon->flag[16]);
                RotTransPers(&ribbon->b[16], &ribbon->sb[16].packed, projection, &flag);
                dx = ribbon->sa[16].point.vx - ribbon->sa[15].point.vx;
                dy = ribbon->sa[16].point.vy - ribbon->sa[15].point.vy;
                ribbon->angle[16] = ratan2(dy, dx) - 1024;
                ribbon->width[16] = ribbon->sb[16].point.vx - ribbon->sa[16].point.vx;
                ribbon->ox[16] = rcos(ribbon->angle[16]) * ribbon->width[16] >> 12;
                ribbon->oy[16] = rsin(ribbon->angle[16]) * ribbon->width[16] >> 12;
            } else
#else
    for (i = 0, projection = &p, previous = &ribbon->sa[15].point; i < 3; i++,
         previous = (DVECTOR *)((u8 *)previous + sizeof(Model335Ribbon)), ribbon++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[15], &ribbon->a[k], &ribbon->a[15], &ribbon->a[k],
                                              (long *)previous, &ribbon->sa[k].packed,
                                              (long *)previous, &ribbon->sa[k].packed,
                                              projection, &ribbon->flag[k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, projection, &flag);
                dx = ribbon->sa[k].point.vx - previous->vx;
                dy = ribbon->sa[k].point.vy - previous->vy;
                ribbon->angle[k] = ratan2(dy, dx) - 1024;
                ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                {
                    Model335Ribbon *offsets = (Model335Ribbon *)((s16 *)ribbon + k);
                    offsets->ox[0] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    offsets->oy[0] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                }
            } else
#endif
            {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                              &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed,
                                              &ribbon->sa[k].packed, &ribbon->sa[k + 1].packed,
                                              projection, &ribbon->flag[k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k].packed, projection, &flag);
                dx = MODEL_VARIANT335_RIBBON_NEXT_SCREEN(ribbon, k).point.vx - ribbon->sa[k].point.vx;
                dy = MODEL_VARIANT335_RIBBON_NEXT_SCREEN(ribbon, k).point.vy - ribbon->sa[k].point.vy;
                ribbon->angle[k] = ratan2(dy, dx) - 1024;
                ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            }
        }
    }
    ribbon = (Model335Ribbon *)work;
    for (i = 0; i < 3; i++, ribbon++) {
        k = 0;
        poly = work->flat_textured;
        for (; k < work->count; k++) {
            poly->x0 = MODEL_VARIANT335_RIBBON_SCREEN(ribbon, k).point.vx + ribbon->ox[k];
            poly->y0 = (MODEL_VARIANT335_RIBBON_SCREEN(ribbon, k).packed >> 16) + ribbon->oy[k];
            poly->x1 = MODEL_VARIANT335_RIBBON_NEXT_SCREEN(ribbon, k).point.vx + ribbon->ox[k + 1];
            poly->y1 = (MODEL_VARIANT335_RIBBON_NEXT_SCREEN(ribbon, k).packed >> 16) + ribbon->oy[k + 1];
            poly->x2 = MODEL_VARIANT335_RIBBON_SCREEN(ribbon, k).point.vx - ribbon->ox[k];
            poly->y2 = (MODEL_VARIANT335_RIBBON_SCREEN(ribbon, k).packed >> 16) - ribbon->oy[k];
            poly->x3 = MODEL_VARIANT335_RIBBON_NEXT_SCREEN(ribbon, k).point.vx - ribbon->ox[k + 1];
            poly->y3 = (MODEL_VARIANT335_RIBBON_NEXT_SCREEN(ribbon, k).packed >> 16) - ribbon->oy[k + 1];
            setRGB0(poly, ribbon->color[0], ribbon->color[1], ribbon->color[2]);
            if (ribbon->otz[k] >= 0 && ribbon->flag[k] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[k]);
            }
            if (!(k & 1)) {
                poly++;
            } else {
                poly--;
            }
        }
    }
    if (work->phase == 4) {
        if (work->count <= 16) {
            work->count = (((u32)work->clock.frame
                - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x1C)) << 4)
                / ((u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x20)
                - (u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x2E60), 0x1C));
            if (work->count >= 16) {
                work->count = 16;
                work->phase = 5;
            }
        }
    } else if (work->phase == 6 && work->width > 0) {
        work->width = sheet->size / 4;
        if (work->width <= 0) {
            work->width = 0;
        }
    }
    work->wave0 += work->clock.step * 1150;
    work->wave1 += work->clock.step * 200;
}
