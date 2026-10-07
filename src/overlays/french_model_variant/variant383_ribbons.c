#include "../../types.h"
#include "variant383_ribbons.h"

void func_8013C204(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag[8][2];
    PSXLONG edge_flag;
    Family383RibbonView *work;
    Family383RibbonColors *colors;
    GsOT *ot;
    s16 i;
    s32 turn;
    s32 tilt;
    Family383Ribbon *ribbon;
    POLY_G3 *triangle;
    s16 j;
    s16 angle;
    s16 length;
    s32 flags;
    s16 radius;
    s32 dx;
    s32 dy;

    work = (Family383RibbonView *)context;
    ot = func_80058F10();
    turn = ratan2(work->view_direction[2], work->view_direction[0]) + 3072;
    ratan2(work->view_direction[1], work->view_direction[0]);
    ribbon = work->ribbons;
    tilt = ratan2(work->delta[2], work->delta[1]) + 1024;
    ratan2(work->delta[2], work->delta[0]);
    colors = &work->colors;
    triangle = &work->triangle;
    if (work->mode == 1) {
        turn = -turn;
    }
    if (work->phase > 0) {
        length = 16;
        flags = work->flags;
        for (i = 0, angle = work->angle; i < 8;
             i++, angle = work->angle + i * 512, ribbon++) {
            for (j = 0; j < 2; j++) {
                radius = 128;
                if (j == 0) {
                    radius = 40;
                }
                setVector(&ribbon->points[j], rcos(angle) * radius >> 12,
                          rsin(angle) * radius >> 12, j * ((flags & 1) * 32 + 192));
                setVector(&ribbon->edges[j],
                          ribbon->points[j].vx + (rcos(turn) * length >> 12),
                          ribbon->points[j].vy,
                          ribbon->points[j].vz + (rsin(turn) * length >> 12));
            }
        }
        setVector(&rotation, tilt, 0, 0);
        matrix.t[0] = work->origin[0] + work->delta[0] * work->factor / 1024;
        matrix.t[1] = work->origin[1] + work->delta[1] * work->factor / 1024;
        matrix.t[2] = work->origin[2] + work->delta[2] * work->factor / 1024;
        if (work->scale < 4096) {
            scale.vx = work->scale;
            scale.vy = work->scale;
            scale.vz = work->scale;
        } else {
            scale.vx = 4096;
            scale.vy = 4096;
            scale.vz = 4096;
        }
        RotMatrix(&rotation, &matrix);
        ScaleMatrix(&matrix, &scale);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        ribbon = work->ribbons;
        for (i = 0; i < 8; i++, ribbon++) {
            for (j = 0; j < 2; j++) {
                if (j == 0) {
                    ribbon->depth[0] = RotTransPers4(
                        &ribbon->points[0], &ribbon->points[1],
                        &ribbon->points[0], &ribbon->points[1],
                        &ribbon->projected[0], &ribbon->projected[1],
                        &ribbon->projected[0], &ribbon->projected[1], &interpolation, &flag[i][j]);
                    dx = (s16)ribbon->projected[j + 1] - (s16)ribbon->projected[0];
                    dy = (ribbon->projected[j + 1] >> 16) - (ribbon->projected[0] >> 16);
                } else {
                    ribbon->depth[j] = RotTransPers4(
                        &ribbon->points[j - 1], &ribbon->points[j],
                        &ribbon->points[j - 1], &ribbon->points[j],
                        &ribbon->projected[j - 1], &ribbon->projected[j],
                        &ribbon->projected[j - 1], &ribbon->projected[j], &interpolation, &flag[i][j]);
                    dx = (s16)ribbon->projected[j] - (s16)ribbon->projected[j - 1];
                    dy = (ribbon->projected[j] >> 16) - (ribbon->projected[j - 1] >> 16);
                }
                RotTransPers(&ribbon->edges[j], &ribbon->edge_projected[j], &interpolation, &edge_flag);
                ribbon->angle[j] = ratan2(dy, dx) + 3072;
                ribbon->width[j] = (s16)ribbon->edge_projected[j] - (s16)ribbon->projected[j];
                ribbon->x_offset[j] = rcos(ribbon->angle[j]) * ribbon->width[j] >> 12;
                ribbon->y_offset[j] = rsin(ribbon->angle[j]) * ribbon->width[j] >> 12;
            }
        }
        ribbon = work->ribbons;
        for (i = 0; i < 8; i++, ribbon++) {
            for (j = 0; j < 1; j++) {
                triangle->x0 = ribbon->projected[j] + ribbon->x_offset[j];
                triangle->y0 = (ribbon->projected[j] >> 16) + ribbon->y_offset[j];
                triangle->x1 = ribbon->projected[j + 1];
                triangle->y1 = ribbon->projected[j + 1] >> 16;
                triangle->x2 = ribbon->projected[j] - ribbon->x_offset[j];
                triangle->y2 = (ribbon->projected[j] >> 16) - ribbon->y_offset[j];
                triangle->r0 = colors->outer;
                triangle->g0 = colors->outer;
                triangle->b0 = colors->outer;
                triangle->r1 = colors->inner;
                triangle->g1 = colors->inner;
                triangle->b1 = colors->inner;
                triangle->r2 = colors->outer;
                triangle->g2 = colors->outer;
                triangle->b2 = colors->outer;
                if (ribbon->depth[j] >= 0 && flag[i][j] >= 0) {
                    func_8005B260((u32 *)triangle, ot, (u16)ribbon->depth[j], 1);
                }
            }
        }
    }
    if (work->index_154C + 1 == work->config->count_0C) {
        work->angle += work->step * 50;
    }
}
