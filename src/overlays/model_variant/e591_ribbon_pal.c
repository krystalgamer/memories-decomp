#include "../../types.h"
#include "../model_variant/e591_entry_pal.h"

void FrenchModelVariant425_DrawRibbon(EntryState425 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long flags[1][17];
    long interpolation;
    long flag;
    GsOT *ot;
    s16 i, j;
    EntryState425 *work = context;
    EntryRibbon425 *ribbon;
    POLY_GT4 *poly;
    s32 angle;
    s16 len;
    s16 width;
    s32 f;
    s32 sway;
    s32 dy, dz;
    s32 dx;
    s32 n, k;
    s32 py, pz;

    ot = Graphics_GetDefaultOrderingTable();
    angle = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 0xC00;
    ratan2(work->aim.view_delta.vy, work->aim.view_delta.vx);
    poly = work->packets.textured;
    if (!(work->clock.frame_count & 1)) {
        len = work->width * 80 / 1024;
        width = work->width / 64;
    } else {
        len = work->width * 96 / 1024;
        width = work->width / 64;
    }
    ribbon = work->ribbons;
    for (i = 0; i < 1; i++, ribbon++) {
        if (i < 0) {
            dy = 0;
        }
        for (j = 0, sway = -work->ribbon_sway; j < 17; j++, sway += 768) {
            py = work->aim.direction.vy * work->progress / 1024;
            pz = work->aim.direction.vz * work->progress / 1024;
            n = k = j;
            setVector(&ribbon->spine.a[n], work->aim.direction.vx * work->progress / 1024 * j / 16, py * (k = n) / 16, pz * j / 16);
            setVector(&ribbon->spine.b[j],
                      ribbon->spine.a[j].vx + (rcos(angle) * (len + (rcos(sway) * width >> 12)) >> 12),
                      ribbon->spine.a[j].vy,
                      ribbon->spine.a[j].vz + (rsin(angle) * (len + (rsin(sway) * width >> 12)) >> 12));
        }
    }

    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = work->matrix.t[0];
    matrix.t[1] = work->matrix.t[1];
    matrix.t[2] = work->matrix.t[2];
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

    ribbon = work->ribbons;
    for (i = 0; i < 1; i++, ribbon++) {
        for (j = 0; j < 17; j++) {
            if (j == 16) {
                ribbon->offsets.otz[j] = RotTransPers4(&ribbon->spine.a[j - 1], &ribbon->spine.a[j], &ribbon->spine.a[j - 1], &ribbon->spine.a[j],
                                               &ribbon->spine.sa[j - 1], &ribbon->spine.sa[j],
                                               &ribbon->spine.sa[j - 1], &ribbon->spine.sa[j],
                                               &interpolation, &flags[i][j]);
                RotTransPers(&ribbon->spine.b[j], &ribbon->spine.sb[j], &interpolation, &flag);
                dx = (s16)ribbon->spine.sa[j] - (s16)ribbon->spine.sa[j - 1];
                dy = (ribbon->spine.sa[j] >> 16) - (ribbon->spine.sa[j - 1] >> 16);
                ribbon->spine.angle[j] = ratan2(dy, dx) - 0x400;
                ribbon->spine.width[j] = (s16)ribbon->spine.sb[j] - (s16)ribbon->spine.sa[j];
                ribbon->offsets.ox[j] = rcos(ribbon->spine.angle[j]) * ribbon->spine.width[j] >> 12;
                ribbon->offsets.oy[j] = rsin(ribbon->spine.angle[j]) * ribbon->spine.width[j] >> 12;
            } else {
                ribbon->offsets.otz[j] = RotTransPers4(&ribbon->spine.a[j], &ribbon->spine.a[j + 1], &ribbon->spine.a[j], &ribbon->spine.a[j + 1],
                                               &ribbon->spine.sa[j], &ribbon->spine.sa[j + 1],
                                               &ribbon->spine.sa[j], &ribbon->spine.sa[j + 1],
                                               &interpolation, &flags[i][j]);
                RotTransPers(&ribbon->spine.b[j], &ribbon->spine.sb[j], &interpolation, &flag);
                dx = (s16)ribbon->spine.sa[j + 1] - (s16)ribbon->spine.sa[j];
                dy = (s16)(ribbon->spine.sa[j + 1] >> 16) - (s16)(ribbon->spine.sa[j] >> 16);
                ribbon->spine.angle[j] = ratan2(dy, dx) - 0x400;
                ribbon->spine.width[j] = (s16)ribbon->spine.sb[j] - (s16)ribbon->spine.sa[j];
                ribbon->offsets.ox[j] = rcos(ribbon->spine.angle[j]) * ribbon->spine.width[j] >> 12;
                ribbon->offsets.oy[j] = rsin(ribbon->spine.angle[j]) * ribbon->spine.width[j] >> 12;
            }
        }
    }

    ribbon = work->ribbons;
    for (i = 0; i < 1; i++, ribbon++) {
        for (j = 0; j < 16; j++) {
            setRGB0(poly, 0, 0, 255);
            setRGB1(poly, 0, 0, 255);
            setRGB2(poly, 192, 192, 192);
            setRGB3(poly, 192, 192, 192);
            poly->x0 = (s16)ribbon->spine.sa[j] + ribbon->offsets.ox[j];
            poly->y0 = (ribbon->spine.sa[j] >> 16) + ribbon->offsets.oy[j];
            poly->x1 = (s16)ribbon->spine.sa[j + 1] + ribbon->offsets.ox[j + 1];
            poly->y1 = (ribbon->spine.sa[j + 1] >> 16) + ribbon->offsets.oy[j + 1];
            poly->x2 = (s16)ribbon->spine.sa[j];
            poly->y2 = (ribbon->spine.sa[j] >> 16);
            poly->x3 = (s16)ribbon->spine.sa[j + 1];
            poly->y3 = (ribbon->spine.sa[j + 1] >> 16);
            if (ribbon->offsets.otz[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(poly, ot, (u16)ribbon->offsets.otz[j]);
            }
            poly->x0 = (s16)ribbon->spine.sa[j] - ribbon->offsets.ox[j];
            poly->y0 = (ribbon->spine.sa[j] >> 16) - ribbon->offsets.oy[j];
            poly->x1 = (s16)ribbon->spine.sa[j + 1] - ribbon->offsets.ox[j + 1];
            poly->y1 = (ribbon->spine.sa[j + 1] >> 16) - ribbon->offsets.oy[j + 1];
            poly->x2 = (s16)ribbon->spine.sa[j];
            poly->y2 = (ribbon->spine.sa[j] >> 16);
            poly->x3 = (s16)ribbon->spine.sa[j + 1];
            poly->y3 = (ribbon->spine.sa[j + 1] >> 16);
            if (ribbon->offsets.otz[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(poly, ot, (u16)ribbon->offsets.otz[j]);
            }
        }
    }
    work->ribbon_sway += work->clock.step * 384;
    work->ribbon_wave += work->clock.step * 128;
    if (work->progress < 0x400 && work->config->ribbon_start <= work->clock.frame) {
        work->progress = ((work->clock.frame - work->config->ribbon_start) << 10) / (work->config->travel_start - work->config->ribbon_start);
        if (work->progress >= 0x400) {
            work->progress = 0x400;
            if (work->phase == 2) {
                work->phase = 3;
            }
        }
    }
    if (work->config->fade_start <= work->clock.frame && work->width > 0) {
        work->width = 0x400 - ((work->clock.frame - work->config->fade_start) << 10) / (work->config->fade_end - work->config->fade_start);
        if (work->width <= 0) {
            work->width = 0;
            if (work->phase == 4) {
                work->phase = 5;
            }
        }
    }
}
