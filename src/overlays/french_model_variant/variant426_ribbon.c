#include "../../types.h"
#include "variant426_ribbon.h"

/* The North American header-409 build reads the projected points through
 * pointer arithmetic; the default spelling is the French/Spanish one. */
#ifdef MODEL_VARIANT409_RIBBON_POINTER_SCREEN
#define RIBBON426_SA(k) (*(ribbon->sa + (k)))
#define RIBBON426_SB(k) (*(ribbon->sb + (k)))
#else
#define RIBBON426_SA(k) (ribbon->sa[k])
#define RIBBON426_SB(k) (ribbon->sb[k])
#endif

void func_8013CDF0(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG flags[1][17];
    PSXLONG interpolation;
    PSXLONG flag;
    GsOT *ot;
    s16 i, j;
    Ribbon426Work *work = (Ribbon426Work *)context;
    Ribbon426 *ribbon;
    Ribbon426Screen screen;
    s16 prev;
    POLY_GT4 *poly;
    s32 angle;
    s16 len;
    s32 dy, dz;
    s32 dx;
    s32 n, k;
    s32 py, pz;

    ot = func_80058F10();
    angle = ratan2(work->target[2], work->target[0]) + 0xC00;
    ratan2(work->target[1], work->target[0]);
    poly = &work->poly;
    if (!(work->frame & 1)) {
        len = work->fade / 16;
    } else {
        len = work->fade * 72 / 1024;
    }
    ribbon = work->ribbon;
    for (i = 0; i < 1; i++, ribbon++) {
        if (i < 0) {
            dy = 0;
        }
        for (j = 0; j < 17; j++) {
            py = work->vec[1] * work->amp / 1024;
            pz = work->vec[2] * work->amp / 1024;
            n = k = j;
            setVector(&ribbon->a[n], work->vec[0] * work->amp / 1024 * j / 16, py * (k = n) / 16, pz * j / 16);
            setVector(&ribbon->b[j],
                      ribbon->a[j].vx + (rcos(angle) * len >> 12),
                      ribbon->a[j].vy,
                      ribbon->a[j].vz + (rsin(angle) * len >> 12));
        }
    }

    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = work->origin[0];
    matrix.t[1] = work->origin[1];
    matrix.t[2] = work->origin[2];
    scale.vx = 4096;
    scale.vy = 4096;
    scale.vz = 4096;
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);

    ribbon = work->ribbon;
    for (i = 0; i < 1; i++, ribbon++) {
        for (j = 0; j < 17; j++) {
            if (j == 16) {
                ribbon->otz[j] = RotTransPers4(&ribbon->a[j - 1], &ribbon->a[j], &ribbon->a[j - 1], &ribbon->a[j],
                                               &ribbon->sa[j - 1].packed, &ribbon->sa[j].packed,
                                               &ribbon->sa[j - 1].packed, &ribbon->sa[j].packed,
                                               &interpolation, &flags[i][j]);
                RotTransPers(&ribbon->b[j], &ribbon->sb[j].packed, &interpolation, &flag);
                dx = RIBBON426_SA(j).point.vx - RIBBON426_SA(j - 1).point.vx;
                screen = ribbon->sa[j];
                prev = ribbon->sa[j - 1].packed >> 16;
                dy = (screen.packed >> 16) - prev;
                ribbon->angle[j] = ratan2(dy, dx) - 0x400;
                ribbon->width[j] = RIBBON426_SB(j).point.vx - RIBBON426_SA(j).point.vx;
                ribbon->ox[j] = rcos(ribbon->angle[j]) * ribbon->width[j] >> 12;
                ribbon->oy[j] = rsin(ribbon->angle[j]) * ribbon->width[j] >> 12;
            } else {
                ribbon->otz[j] = RotTransPers4(&ribbon->a[j], &ribbon->a[j + 1], &ribbon->a[j], &ribbon->a[j + 1],
                                               &ribbon->sa[j].packed, &ribbon->sa[j + 1].packed,
                                               &ribbon->sa[j].packed, &ribbon->sa[j + 1].packed,
                                               &interpolation, &flags[i][j]);
                RotTransPers(&ribbon->b[j], &ribbon->sb[j].packed, &interpolation, &flag);
                dx = RIBBON426_SA(j + 1).point.vx - RIBBON426_SA(j).point.vx;
                dy = RIBBON426_SA(j + 1).point.vy - RIBBON426_SA(j).point.vy;
                ribbon->angle[j] = ratan2(dy, dx) - 0x400;
                ribbon->width[j] = RIBBON426_SB(j).point.vx - RIBBON426_SA(j).point.vx;
                ribbon->ox[j] = rcos(ribbon->angle[j]) * ribbon->width[j] >> 12;
                ribbon->oy[j] = rsin(ribbon->angle[j]) * ribbon->width[j] >> 12;
            }
        }
    }

    ribbon = work->ribbon;
    for (i = 0; i < 1; i++, ribbon++) {
        for (j = 0; j < 16; j++) {
            setRGB0(poly, 0, 0, 255);
            setRGB1(poly, 0, 0, 255);
            setRGB2(poly, 192, 192, 192);
            setRGB3(poly, 192, 192, 192);
            poly->x0 = RIBBON426_SA(j).point.vx + ribbon->ox[j];
            poly->y0 = (RIBBON426_SA(j).packed >> 16) + ribbon->oy[j];
            poly->x1 = RIBBON426_SA(j + 1).point.vx + ribbon->ox[j + 1];
            poly->y1 = (RIBBON426_SA(j + 1).packed >> 16) + ribbon->oy[j + 1];
            poly->x2 = RIBBON426_SA(j).point.vx;
            poly->y2 = (RIBBON426_SA(j).packed >> 16);
            poly->x3 = RIBBON426_SA(j + 1).point.vx;
            poly->y3 = (RIBBON426_SA(j + 1).packed >> 16);
            if (ribbon->otz[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(poly, ot, (u16)ribbon->otz[j]);
            }
            poly->x0 = RIBBON426_SA(j).point.vx - ribbon->ox[j];
            poly->y0 = (RIBBON426_SA(j).packed >> 16) - ribbon->oy[j];
            poly->x1 = RIBBON426_SA(j + 1).point.vx - ribbon->ox[j + 1];
            poly->y1 = (RIBBON426_SA(j + 1).packed >> 16) - ribbon->oy[j + 1];
            poly->x2 = RIBBON426_SA(j).point.vx;
            poly->y2 = (RIBBON426_SA(j).packed >> 16);
            poly->x3 = RIBBON426_SA(j + 1).point.vx;
            poly->y3 = (RIBBON426_SA(j + 1).packed >> 16);
            if (ribbon->otz[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(poly, ot, (u16)ribbon->otz[j]);
            }
        }
    }
    work->phase += work->step * 850;
    work->wave += work->step * 128;
    if (work->amp < 0x400 && work->timing->grow_start <= work->clock) {
        work->amp = ((work->clock - work->timing->grow_start) << 10) / (work->timing->grow_end - work->timing->grow_start);
        if (work->amp >= 0x400) {
            work->amp = 0x400;
            if (work->state < 3) {
                work->state = 3;
            }
        }
    }
    if (work->timing->fade_start <= work->clock && work->fade > 0) {
        work->fade = 0x400 - ((work->clock - work->timing->fade_start) << 10) / (work->timing->fade_end - work->timing->fade_start);
        if (work->fade <= 0) {
            work->fade = 0;
            if (work->state == 4) {
                work->state = 5;
            }
        }
    }
}
