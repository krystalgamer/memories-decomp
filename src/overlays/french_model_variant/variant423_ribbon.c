#include "../../types.h"
#include "variant423_ribbon.h"

/* The North American header-406 copy, built with the CDK compiler, indexes the
 * projected points through pointer arithmetic and reads the coordinates in its
 * projection loop as halves of the packed word, which keeps that compiler's
 * address folding and loop-invariant choices. */
#ifdef MODEL_VARIANT423_RIBBON_PACKED_SCREEN
#define RIBBON423_SCREEN(screen, n) (*((screen) + (n)))
#define RIBBON423_VX(screen, n) ((s16)RIBBON423_SCREEN(screen, n).packed)
#define RIBBON423_VY(screen, n) ((s16)(RIBBON423_SCREEN(screen, n).packed >> 16))
#define RIBBON423_HI(screen, n) RIBBON423_VY(screen, n)
#else
#define RIBBON423_SCREEN(screen, n) ((screen)[n])
#define RIBBON423_VX(screen, n) ((screen)[n].point.vx)
#define RIBBON423_VY(screen, n) ((screen)[n].point.vy)
#define RIBBON423_HI(screen, n) ((screen)[n].packed >> 16)
#endif

void func_8013CCB4(u8 *context)
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
    Ribbon423Work *work = (Ribbon423Work *)context;
    Ribbon423 *ribbon;
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
                                               &RIBBON423_SCREEN(ribbon->sa, j - 1).packed, &RIBBON423_SCREEN(ribbon->sa, j).packed,
                                               &RIBBON423_SCREEN(ribbon->sa, j - 1).packed, &RIBBON423_SCREEN(ribbon->sa, j).packed,
                                               &interpolation, &flags[i][j]);
                RotTransPers(&ribbon->b[j], &RIBBON423_SCREEN(ribbon->sb, j).packed, &interpolation, &flag);
                dx = RIBBON423_VX(ribbon->sa, j) - RIBBON423_VX(ribbon->sa, j - 1);
                dy = RIBBON423_HI(ribbon->sa, j) - RIBBON423_HI(ribbon->sa, j - 1);
                ribbon->angle[j] = ratan2(dy, dx) - 0x400;
                ribbon->width[j] = RIBBON423_VX(ribbon->sb, j) - RIBBON423_VX(ribbon->sa, j);
                ribbon->ox[j] = rcos(ribbon->angle[j]) * ribbon->width[j] >> 12;
                ribbon->oy[j] = rsin(ribbon->angle[j]) * ribbon->width[j] >> 12;
            } else {
                ribbon->otz[j] = RotTransPers4(&ribbon->a[j], &ribbon->a[j + 1], &ribbon->a[j], &ribbon->a[j + 1],
                                               &RIBBON423_SCREEN(ribbon->sa, j).packed, &RIBBON423_SCREEN(ribbon->sa, j + 1).packed,
                                               &RIBBON423_SCREEN(ribbon->sa, j).packed, &RIBBON423_SCREEN(ribbon->sa, j + 1).packed,
                                               &interpolation, &flags[i][j]);
                RotTransPers(&ribbon->b[j], &RIBBON423_SCREEN(ribbon->sb, j).packed, &interpolation, &flag);
                dx = RIBBON423_VX(ribbon->sa, j + 1) - RIBBON423_VX(ribbon->sa, j);
                dy = RIBBON423_VY(ribbon->sa, j + 1) - RIBBON423_VY(ribbon->sa, j);
                ribbon->angle[j] = ratan2(dy, dx) - 0x400;
                ribbon->width[j] = RIBBON423_VX(ribbon->sb, j) - RIBBON423_VX(ribbon->sa, j);
                ribbon->ox[j] = rcos(ribbon->angle[j]) * ribbon->width[j] >> 12;
                ribbon->oy[j] = rsin(ribbon->angle[j]) * ribbon->width[j] >> 12;
            }
        }
    }

    ribbon = work->ribbon;
    for (i = 0; i < 1; i++, ribbon++) {
        for (j = 0; j < 16; j++) {
            setRGB0(poly, 0, 0, 64);
            setRGB1(poly, 0, 0, 64);
            setRGB2(poly, 192, 192, 192);
            setRGB3(poly, 192, 192, 192);
            poly->x0 = RIBBON423_SCREEN(ribbon->sa, j).point.vx + ribbon->ox[j];
            poly->y0 = (RIBBON423_SCREEN(ribbon->sa, j).packed >> 16) + ribbon->oy[j];
            poly->x1 = RIBBON423_SCREEN(ribbon->sa, j + 1).point.vx + ribbon->ox[j + 1];
            poly->y1 = (RIBBON423_SCREEN(ribbon->sa, j + 1).packed >> 16) + ribbon->oy[j + 1];
            poly->x2 = RIBBON423_SCREEN(ribbon->sa, j).point.vx;
            poly->y2 = (RIBBON423_SCREEN(ribbon->sa, j).packed >> 16);
            poly->x3 = RIBBON423_SCREEN(ribbon->sa, j + 1).point.vx;
            poly->y3 = (RIBBON423_SCREEN(ribbon->sa, j + 1).packed >> 16);
            if (ribbon->otz[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(poly, ot, (u16)ribbon->otz[j]);
            }
            poly->x0 = RIBBON423_SCREEN(ribbon->sa, j).point.vx - ribbon->ox[j];
            poly->y0 = (RIBBON423_SCREEN(ribbon->sa, j).packed >> 16) - ribbon->oy[j];
            poly->x1 = RIBBON423_SCREEN(ribbon->sa, j + 1).point.vx - ribbon->ox[j + 1];
            poly->y1 = (RIBBON423_SCREEN(ribbon->sa, j + 1).packed >> 16) - ribbon->oy[j + 1];
            poly->x2 = RIBBON423_SCREEN(ribbon->sa, j).point.vx;
            poly->y2 = (RIBBON423_SCREEN(ribbon->sa, j).packed >> 16);
            poly->x3 = RIBBON423_SCREEN(ribbon->sa, j + 1).point.vx;
            poly->y3 = (RIBBON423_SCREEN(ribbon->sa, j + 1).packed >> 16);
            if (ribbon->otz[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(poly, ot, (u16)ribbon->otz[j]);
            }
        }
    }
    work->phase += work->step * 850;
    work->wave += work->step * 128;
    if (work->amp < 0x400) {
        work->amp += work->step * 64;
        if (work->amp >= 0x400) {
            work->amp = 0x400;
            if (work->state == 2) {
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
