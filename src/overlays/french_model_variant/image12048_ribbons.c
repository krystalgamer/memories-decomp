#include "../../types.h"
#include "image12048_ribbons.h"

void func_8013D22C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG flags[3][9];
    PSXLONG interpolation;
    PSXLONG flag;
    Ribbons12048State *work;
    GsOT *ot;
    s16 i, j;
    s32 wave, ripple;
    s32 size;
    s32 pulse;
    s32 yaw;
    s16 length;
    s32 dx, dy;
    s32 last;
    Ribbon12048 *ribbon;
    POLY_FT4 *quad;

    work = (Ribbons12048State *)context;
    ribbon = work->ribbons;
    ot = func_80058F10();
    yaw = ratan2(work->view_direction[2], work->view_direction[0]) + 3072;
    ratan2(work->view_direction[1], work->view_direction[0]);
    length = 24;
    for (i = 0; i < 3; i++, ribbon++) {
        for (j = 0, wave = ribbon->wave[0], ripple = ribbon->wave[1];
             j < 9; j++, wave = j * 768, ripple += 1700) {
            pulse = rcos(wave) * 64 >> 12;
            size = pulse + (rcos(ripple) * 32 >> 12);
            if (j == 0) {
                ribbon->points[j].vx = rcos(ribbon->bearing[0])
                    * (rsin(ribbon->bearing[1]) * 384 >> 12) >> 12;
                ribbon->points[j].vy = rcos(ribbon->bearing[1]) * 384 >> 12;
                ribbon->points[j].vz = rsin(ribbon->bearing[0])
                    * (rsin(ribbon->bearing[1]) * 384 >> 12) >> 12;
                ribbon->direction[0] = -ribbon->points[j].vx;
                ribbon->direction[1] = -ribbon->points[j].vy;
                ribbon->direction[2] = -ribbon->points[j].vz;
            } else {
                (j + ribbon->points)->vx = ribbon->points[0].vx
                    + ribbon->direction[0] * j / 4
                    + (rcos(yaw) * (rcos(ribbon->bearing[1]) * (s16)size >> 12) >> 12);
                (j + ribbon->points)->vy = ribbon->points[0].vy
                    + ribbon->direction[1] * j / 4
                    + (rsin(ribbon->bearing[1]) * (s16)size >> 12);
                (j + ribbon->points)->vz = ribbon->points[0].vz
                    + ribbon->direction[2] * j / 4
                    + (rsin(yaw) * (rcos(ribbon->bearing[1]) * (s16)size >> 12) >> 12);
            }
            setVector(&ribbon->edges[j],
                ribbon->points[j].vx + (rcos(yaw) * length >> 12),
                ribbon->points[j].vy,
                ribbon->points[j].vz + (rsin(yaw) * length >> 12));
        }
    }
    setVector(&rotation, 0, 0, 0);
    matrix.t[0] = work->translation.vx;
    matrix.t[1] = work->translation.vy;
    matrix.t[2] = work->translation.vz;
    setVector(&scale, 4096, 4096, 4096);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    ribbon = work->ribbons;
    for (i = 0; i < 3; i++, ribbon++) {
        for (j = 0; j < 9; j++) {
            if (j == 8) {
                ribbon->depth[j] = RotTransPers4(
                    &ribbon->points[7], &ribbon->points[8],
                    &ribbon->points[7], &ribbon->points[8],
                    &ribbon->screen[7].packed, &ribbon->screen[8].packed,
                    &ribbon->screen[7].packed, &ribbon->screen[8].packed,
                    &interpolation, &flags[i][8]);
                RotTransPers(&ribbon->edges[8], &ribbon->edge_screen[8].packed,
                             &interpolation, &flag);
                dx = ribbon->screen[j].point.vx - ribbon->screen[7].point.vx;
                dy = ribbon->screen[j].point.vy - ribbon->screen[7].point.vy;
                ribbon->angle[j] = ratan2(dy, dx) + 3072;
                ribbon->width[j] = ribbon->edge_screen[j].point.vx - ribbon->screen[j].point.vx;
                ribbon->x_offset[j] = rcos(ribbon->angle[j]) * ribbon->width[j] >> 12;
                ribbon->y_offset[j] = rsin(ribbon->angle[j]) * ribbon->width[j] >> 12;
            } else {
                ribbon->depth[j] = RotTransPers4(
                    &ribbon->points[j], &ribbon->points[j + 1],
                    &ribbon->points[j], &ribbon->points[j + 1],
                    &ribbon->screen[j].packed, &ribbon->screen[j + 1].packed,
                    &ribbon->screen[j].packed, &ribbon->screen[j + 1].packed,
                    &interpolation, &flags[i][j]);
                RotTransPers(&ribbon->edges[j], &ribbon->edge_screen[j].packed,
                             &interpolation, &flag);
                dx = ribbon->screen[j + 1].point.vx - ribbon->screen[j].point.vx;
                dy = ribbon->screen[j + 1].point.vy - ribbon->screen[j].point.vy;
                ribbon->angle[j] = ratan2(dy, dx) + 3072;
                ribbon->width[j] = ribbon->edge_screen[j].point.vx - ribbon->screen[j].point.vx;
                ribbon->x_offset[j] = rcos(ribbon->angle[j]) * ribbon->width[j] >> 12;
                ribbon->y_offset[j] = rsin(ribbon->angle[j]) * ribbon->width[j] >> 12;
            }
        }
    }
    ribbon = work->ribbons;
    for (i = 0; i < 3; i++, ribbon++) {
        last = 0;
        if (ribbon->last >= 0) {
            last = ribbon->last;
        }
        quad = work->quads;
        for (j = ribbon->first; j < last; j++, quad = work->quads) {
            if (!(j & 1)) {
                quad++;
            }
            if (j == 0) {
                quad->x0 = ribbon->screen[j].point.vx;
                quad->y0 = ribbon->screen[j].packed >> 16;
                quad->x1 = ribbon->screen[j + 1].point.vx + ribbon->x_offset[j + 1];
                quad->y1 = (ribbon->screen[j + 1].packed >> 16) + ribbon->y_offset[j + 1];
                quad->x2 = ribbon->screen[j].point.vx;
                quad->y2 = ribbon->screen[j].packed >> 16;
                quad->x3 = ribbon->screen[j + 1].point.vx - ribbon->x_offset[j + 1];
                quad->y3 = (ribbon->screen[j + 1].packed >> 16) - ribbon->y_offset[j + 1];
            } else if (j + 1 == 8) {
                quad->x0 = ribbon->screen[j].point.vx + ribbon->x_offset[j];
                quad->y0 = (ribbon->screen[j].packed >> 16) + ribbon->y_offset[j];
                quad->x1 = ribbon->screen[j + 1].point.vx;
                quad->y1 = ribbon->screen[j + 1].packed >> 16;
                quad->x2 = ribbon->screen[j].point.vx - ribbon->x_offset[j];
                quad->y2 = (ribbon->screen[j].packed >> 16) - ribbon->y_offset[j];
                quad->x3 = ribbon->screen[j + 1].point.vx;
                quad->y3 = ribbon->screen[j + 1].packed >> 16;
            } else {
                quad->x0 = ribbon->screen[j].point.vx + ribbon->x_offset[j];
                quad->y0 = (ribbon->screen[j].packed >> 16) + ribbon->y_offset[j];
                quad->x1 = ribbon->screen[j + 1].point.vx + ribbon->x_offset[j + 1];
                quad->y1 = (ribbon->screen[j + 1].packed >> 16) + ribbon->y_offset[j + 1];
                quad->x2 = ribbon->screen[j].point.vx - ribbon->x_offset[j];
                quad->y2 = (ribbon->screen[j].packed >> 16) - ribbon->y_offset[j];
                quad->x3 = ribbon->screen[j + 1].point.vx - ribbon->x_offset[j + 1];
                quad->y3 = (ribbon->screen[j + 1].packed >> 16) - ribbon->y_offset[j + 1];
            }
            setRGB0(quad, ribbon->color[j].r, ribbon->color[j].g, ribbon->color[j].b);
            if (ribbon->depth[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(quad, ot, (u16)ribbon->depth[j]);
            }
        }
        if (ribbon->cycles < 4) {
            if (ribbon->last < 9) {
                ribbon->last += 2;
                if (ribbon->last >= 8) {
                    ribbon->last = 8;
                    if (ribbon->first < 8) {
                        ribbon->first += 2;
                        if (ribbon->first >= 8) {
                            ribbon->last = -8;
                            ribbon->first = 0;
                            ribbon->bearing[0] += 1200;
                            ribbon->bearing[1] += 300;
                            if (work->phase >= 6) {
                                ribbon->cycles++;
                                if (i + 1 == 3 && ribbon->cycles == 4) {
                                    work->phase = 7;
                                }
                            }
                        }
                    }
                }
            }
        }
        ribbon->wave[0] += 384;
        ribbon->wave[1] += 768;
    }
}
