#include "../../types.h"
#include "variant335_ribbons.h"

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
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
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
    PSXLONG *projection;

    work = ctx;
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x2E3C), MODEL_VARIANT_WORD(work, 0x2E34)) + 3072;
    ratan2(MODEL_VARIANT_WORD(work, 0x2E38), MODEL_VARIANT_WORD(work, 0x2E34));
    MODEL_VARIANT_WORD(work, 0x2DE8) = MODEL_VARIANT_WORD(work, 0x2DDC)
        + (rsin(MODEL_VARIANT_WORD(work, 0x2E6C)) * MODEL_VARIANT_WORD(work, 0x2E20) >> 12);
    MODEL_VARIANT_WORD(work, 0x2DEC) = MODEL_VARIANT_WORD(work, 0x2DE0) - MODEL_VARIANT_WORD(work, 0x2E70);
    MODEL_VARIANT_WORD(work, 0x2DF0) = MODEL_VARIANT_WORD(work, 0x2DE4)
        + (rsin(MODEL_VARIANT_WORD(work, 0x2E6C)) * MODEL_VARIANT_WORD(work, 0x2E28) >> 12);
    MODEL_VARIANT_WORD(work, 0x2DF8) = MODEL_VARIANT_WORD(work, 0x2DE8);
    MODEL_VARIANT_WORD(work, 0x2DFC) = 0;
    MODEL_VARIANT_WORD(work, 0x2E00) = MODEL_VARIANT_WORD(work, 0x2DF0);
    MODEL_VARIANT_WORD(work, 0x2E08) = MODEL_VARIANT_WORD(work, 0x2DF8) - MODEL_VARIANT_WORD(work, 0x2DE8);
    MODEL_VARIANT_WORD(work, 0x2E0C) = MODEL_VARIANT_WORD(work, 0x2DFC) - MODEL_VARIANT_WORD(work, 0x2DEC);
    MODEL_VARIANT_WORD(work, 0x2E10) = MODEL_VARIANT_WORD(work, 0x2E00) - MODEL_VARIANT_WORD(work, 0x2DF0);
    sheet = (ModelVariantSheet *)(work + 0xABC);
    len = MODEL_VARIANT_HALF(work, 0x2E92) * 48 / 1024;
    ribbon = (Model335Ribbon *)work;
    for (i = 0, phase = 0; i < 3; i++, ribbon++, phase = i * 2048 / 3) {
        angle = yaw + i * 4096 / 3 + MODEL_VARIANT_WORD(work, 0x2E74);
        for (k = 0, wave = -MODEL_VARIANT_WORD(work, 0x2E98); k < 17; k++, wave += 1300) {
            radius = (16 - k) * 32;
            if (k == 0 || k == 16) {
                bend = 0;
            } else {
                bend = phase - k * 384 + MODEL_VARIANT_WORD(work, 0x2E9C);
            }
            {
                s32 point_index = k;
                setVector(&ribbon->a[point_index],
                      MODEL_VARIANT_WORD(work, 0x2E08) * point_index / 16
                          + (rcos(angle) * (radius + (rsin(bend) * 96 >> 12) + (rsin(wave) * 48 >> 12)) >> 12),
                      MODEL_VARIANT_WORD(work, 0x2E0C) * point_index / 16,
                      MODEL_VARIANT_WORD(work, 0x2E10) * point_index / 16
                          + (rsin(angle) * (radius + (rsin(bend) * 96 >> 12) + (rsin(wave) * 48 >> 12)) >> 12));
                setVector(&ribbon->b[point_index],
                      ribbon->a[point_index].vx + (rcos(yaw) * len >> 12),
                      ribbon->a[point_index].vy,
                      ribbon->a[point_index].vz + (rsin(yaw) * len >> 12));
            }
        }
    }
    setVector(&rot, 0, 0, 0);
    m.t[0] = MODEL_VARIANT_WORD(work, 0x2DE8);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x2DEC);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x2DF0);
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
                                              (PSXLONG *)previous, &ribbon->sa[k].packed,
                                              (PSXLONG *)previous, &ribbon->sa[k].packed,
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
        poly = (POLY_FT4 *)(work + 0x2D18);
        for (; k < MODEL_VARIANT_HALF(work, 0x2E8C); k++) {
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
    if (MODEL_VARIANT_WORD(work, 0x2EA0) == 4) {
        if (MODEL_VARIANT_HALF(work, 0x2E8C) <= 16) {
            MODEL_VARIANT_HALF(work, 0x2E8C) = (((u32)MODEL_VARIANT_WORD(work, 0x2E50)
                - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x1C)) << 4)
                / ((u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x20)
                - (u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x2E60), 0x1C));
            if (MODEL_VARIANT_HALF(work, 0x2E8C) >= 16) {
                MODEL_VARIANT_HALF(work, 0x2E8C) = 16;
                MODEL_VARIANT_WORD(work, 0x2EA0) = 5;
            }
        }
    } else if (MODEL_VARIANT_WORD(work, 0x2EA0) == 6 && MODEL_VARIANT_HALF(work, 0x2E92) > 0) {
        MODEL_VARIANT_HALF(work, 0x2E92) = sheet->size / 4;
        if (MODEL_VARIANT_HALF(work, 0x2E92) <= 0) {
            MODEL_VARIANT_HALF(work, 0x2E92) = 0;
        }
    }
    MODEL_VARIANT_WORD(work, 0x2E98) += MODEL_VARIANT_WORD(work, 0x2E58) * 1150;
    MODEL_VARIANT_WORD(work, 0x2E9C) += MODEL_VARIANT_WORD(work, 0x2E58) * 200;
}
