#include "../../types.h"
#include "variant383_strip.h"

void func_8013BCBC(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Family383StripView *work;
    Family383Strip *strip;
    POLY_GT4 *quad;
    GsOT *ot;
    s16 i, j;
    s32 angle;
    s32 size;
    s32 width;

    work = (Family383StripView *)context;
    ot = func_80058F10();
    angle = ratan2(work->angle_delta[1], work->angle_delta[0]);
    quad = &work->quad;
    angle += 2048;
    if (work->phase > 0) {
        strip = &work->strip;
        width = work->width;
        if (width < 0) {
            width += 31;
        }
        i = 0;
        size = width >> 5;
        for (; i < 1; i++, strip++) {
            for (j = 0; j < 2; j++) {
                j[strip->points[0]].vx = rcos(1024) * size >> 12;
                j[strip->points[0]].vy = rsin(1024) * size >> 12;
                j[strip->points[0]].vz = 0;
                setVector(&strip->points[1][j], 0, 0, 0);
                setVector(&strip->points[2][j], rcos(3072) * size >> 12,
                          rsin(3072) * size >> 12, 0);
                setVector(&rotation, 0, 0, angle);
                if (j == 0) {
                    matrix.t[0] = work->origin[0];
                    matrix.t[1] = work->origin[1];
                    matrix.t[2] = work->origin[2];
                } else {
                    matrix.t[0] = work->origin[0] + work->delta[0] * work->factor / 1024;
                    matrix.t[1] = work->origin[1] + work->delta[1] * work->factor / 1024;
                    matrix.t[2] = work->origin[2] + work->delta[2] * work->factor / 1024;
                }
                scale.vz = scale.vy = scale.vx = 4096;
                RotMatrix(&rotation, &matrix);
                coordinate.coord = matrix;
                coordinate.super = 0;
                coordinate.flg = 0;
                GsGetLs(&coordinate, &light);
                GsSetLsMatrix(&light);
                ReadRotMatrix(&light);
                RotMatrix(&rotation, &light);
                ScaleMatrix(&light, &scale);
                SetRotMatrix(&light);
                strip->depth[j] = RotTransPers3(&strip->points[0][j], &strip->points[1][j],
                                                &strip->points[2][j], &strip->projected[0][j],
                                                &strip->projected[1][j], &strip->projected[2][j],
                                                &interpolation, &flag);
            }
        }
        strip = &work->strip;
        for (i = 0; i < 1; i++, strip++) {
            for (j = 0; j < 1; j++) {
                setXY4(quad, (u16)strip->projected[0][j], strip->projected[0][j] >> 16,
                       (u16)strip->projected[0][j + 1], strip->projected[0][j + 1] >> 16,
                       (u16)strip->projected[1][j], strip->projected[1][j] >> 16,
                       (u16)strip->projected[1][j + 1], strip->projected[1][j + 1] >> 16);
                setRGB0(quad, 128, 0, 128);
                setRGB1(quad, 128, 0, 128);
                setRGB2(quad, 192, 192, 192);
                setRGB3(quad, 192, 192, 192);
                setXY4(quad, (u16)strip->projected[2][j], strip->projected[2][j] >> 16,
                       (u16)strip->projected[2][j + 1], strip->projected[2][j + 1] >> 16,
                       (u16)strip->projected[1][j], strip->projected[1][j] >> 16,
                       (u16)strip->projected[1][j + 1], strip->projected[1][j + 1] >> 16);
                setRGB0(quad, 128, 0, 128);
                setRGB1(quad, 128, 0, 128);
                setRGB2(quad, 192, 192, 192);
                setRGB3(quad, 192, 192, 192);
                if (strip->depth[j] > 0) {
                    if (strip->depth[j] < 2048) {
                        GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF);
                    }
                }
            }
        }
        if (work->index_154C + 1 == work->config->count_0C) {
            if (work->phase == 1) {
                if (work->factor <= 1024) {
                    work->factor += work->step * 32;
                    if (work->factor >= 1024) {
                        work->factor = 1024;
                        work->phase = 2;
                    }
                }
            }
            if (work->phase == 5 && work->width > 0) {
                work->width -= work->step * 8;
                if (work->width <= 0) {
                    work->width = 0;
                }
            }
        }
    }
}
