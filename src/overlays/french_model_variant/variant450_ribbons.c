#include "../../types.h"
#include "variant450_ribbons.h"

void func_8013BDD8(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG flags[6][9];
    PSXLONG interpolation;
    PSXLONG flag;
    Model450RibbonView *work;
    GsOT *ot;
    s16 i;
    s32 wave;
    s32 angle;
    s32 yaw;
    s32 pitch;
    s16 length;
    Model450Ribbon *ribbon;
    POLY_FT4 *quad;
    s16 j;
    s32 bend_angle;
    s32 progress;
    s32 dx, dy;

    work = (Model450RibbonView *)context;
    ot = func_80058F10();
    yaw = ratan2(work->view_direction[2], work->view_direction[0]) + 3072;
    pitch = ratan2(work->view_direction[1], work->view_direction[0]) + 3072;
    if (!(work->flags & 1)) {
        length = work->radius / 128;
    } else {
        length = work->radius * 12 / 1024;
    }
    ribbon = work->ribbons;
    for (i = 0; i < 6; i++, ribbon++) {
        if (i == 0) {
            angle = 1024;
        } else if ((s16)(i % 2) == 1) {
            angle = i * 300 + 1024;
        } else {
            angle = 1024 - i * 300;
        }
        for (j = 0, wave = -work->ripple; j < 9; j++, wave += 1300) {
            progress = 0;
            if (ribbon->phase[j] > 0) {
                progress = 1024;
                if (ribbon->phase[j] < progress) {
                    progress = ribbon->phase[j];
                }
            }
            bend_angle = progress * 2;
            setVector(&ribbon->points[j],
                ribbon->tail.delta.vx * progress / 1024
                    - (rcos(angle) * (rsin(bend_angle) * 64 >> 12) >> 12)
                    + (rcos(pitch) * (rsin(wave) * 32 >> 12) >> 12),
                ribbon->tail.delta.vy * progress / 1024
                    - (rsin(angle) * (rsin(bend_angle) * 64 >> 12) >> 12)
                    - (rsin(bend_angle) * 128 >> 12)
                    + (rsin(pitch) * (rsin(wave) * 32 >> 12) >> 12),
                ribbon->tail.delta.vz * progress / 1024);
            setVector(&ribbon->edges[j],
                ribbon->points[j].vx + (rcos(yaw) * length >> 12),
                ribbon->points[j].vy,
                ribbon->points[j].vz + (rsin(yaw) * length >> 12));
            if (ribbon->phase[j] <= 1024) {
                ribbon->phase[j] += work->frame_step << 6;
                if (ribbon->phase[j] >= 1024) {
                    ribbon->phase[j] = 1024;
                    if (i == 0 && work->state == 1) {
                        work->state = 2;
                    }
                    if (j == 8) {
                        ribbon->tail.completed = 1;
                    }
                }
            }
        }
    }
    setVector(&rotation, 0, 0, 0);
    matrix.t[0] = work->origin[0];
    matrix.t[1] = work->origin[1];
    matrix.t[2] = work->origin[2];
    setVector(&scale, 4096, 4096, 4096);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    ribbon = work->ribbons;
    for (i = 0; i < 6; i++, ribbon++) {
        for (j = 0; j < 9; j++) {
            if (j == 8) {
                ribbon->depth[j] = RotTransPers4(
                    &ribbon->points[7], &ribbon->points[8], &ribbon->points[7], &ribbon->points[8],
                    &ribbon->screen[7].packed, &ribbon->screen[8].packed,
                    &ribbon->screen[7].packed, &ribbon->screen[8].packed, &interpolation, &flags[i][8]);
                RotTransPers(&ribbon->edges[8], &ribbon->edge_screen[8].packed, &interpolation, &flag);
                dx = ribbon->screen[j].point.vx - ribbon->screen[7].point.vx;
                dy = ribbon->screen[j].point.vy - ribbon->screen[7].point.vy;
                ribbon->angle[j] = ratan2(dy, dx) - 1024;
                ribbon->width[j] = ribbon->edge_screen[j].point.vx - ribbon->screen[j].point.vx;
                ribbon->x_offset[8] = rcos(ribbon->angle[j]) * ribbon->width[j] >> 12;
                ribbon->y_offset[8] = rsin(ribbon->angle[j]) * ribbon->width[j] >> 12;
            } else {
                ribbon->depth[j] = RotTransPers4(
                    &ribbon->points[j], &ribbon->points[j + 1], &ribbon->points[j], &ribbon->points[j + 1],
                    &ribbon->screen[j].packed, &ribbon->screen[j + 1].packed,
                    &ribbon->screen[j].packed, &ribbon->screen[j + 1].packed, &interpolation, &flags[i][j]);
                RotTransPers(&ribbon->edges[j], &ribbon->edge_screen[j].packed, &interpolation, &flag);
                dx = ribbon->screen[j + 1].point.vx - ribbon->screen[j].point.vx;
                dy = ribbon->screen[j + 1].point.vy - ribbon->screen[j].point.vy;
                ribbon->angle[j] = ratan2(dy, dx) - 1024;
                ribbon->width[j] = ribbon->edge_screen[j].point.vx - ribbon->screen[j].point.vx;
                ribbon->x_offset[j] = rcos(ribbon->angle[j]) * ribbon->width[j] >> 12;
                ribbon->y_offset[j] = rsin(ribbon->angle[j]) * ribbon->width[j] >> 12;
            }
        }
    }
    ribbon = work->ribbons;
    for (i = 0; i < 6; i++, ribbon++) {
        for (j = 0; j < 8; j++) {
            if ((s16)(j % 2) == (work->flags & 1)) {
                quad = &work->packets[0];
            } else {
                quad = &work->packets[1];
            }
            quad->x0 = ribbon->screen[j].point.vx + ribbon->x_offset[j];
            quad->y0 = (ribbon->screen[j].packed >> 16) + ribbon->y_offset[j];
            quad->x1 = ribbon->screen[j + 1].point.vx + ribbon->x_offset[j + 1];
            quad->y1 = (ribbon->screen[j + 1].packed >> 16) + ribbon->y_offset[j + 1];
            quad->x2 = ribbon->screen[j].point.vx - ribbon->x_offset[j];
            quad->y2 = (ribbon->screen[j].packed >> 16) - ribbon->y_offset[j];
            quad->x3 = ribbon->screen[j + 1].point.vx - ribbon->x_offset[j + 1];
            quad->y3 = (ribbon->screen[j + 1].packed >> 16) - ribbon->y_offset[j + 1];
            setRGB0(quad, ribbon->color.r, ribbon->color.g, ribbon->color.b);
            if (ribbon->depth[j] > 0 && flags[i][j] >= 0 && !ribbon->tail.completed) {
                GsSortPoly(quad, ot, (u16)ribbon->depth[j]);
            }
        }
    }
    work->ripple += work->frame_step * 850;
    work->ripple2 += work->frame_step << 7;
}
