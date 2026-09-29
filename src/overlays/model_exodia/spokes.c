#include "../../types.h"
#include "spokes.h"

void func_8013BC0C(u8 *context)
{
    ExodiaSpokeState *work;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX local;
    MATRIX screen;
    GsCOORDINATE2 coordinate;
    SVECTOR unused[16][2];
    SVECTOR points[16][2];
    s32 projected[16][2];
    s32 angles[16][2];
    SVECTOR displaced[16][2];
    s32 displaced_projected[16][2];
    s32 widths[16][2];
    s32 flags[16][2];
    s32 depth[16][2];
    s16 dx[16][2], dy[16][2];
    s32 interpolation, flag;
    GsOT *ot;
    s16 i, j;
    s16 width, matrix_scale;
    s32 longitude, latitude, angle;
    s32 x_delta, y_delta;
    u8 r0, g0, b0, r1, g1, b1;
    POLY_GT4 *quad;

    work = (ExodiaSpokeState *)context;
    quad = &work->quad;
    ot = func_80058F10();
    angle = ratan2(work->direction.vz, work->direction.vx) + 3072;
    for (i = 0; i < 16; i++) {
        if (!(i & 1)) {
            longitude = i * 256 + work->angle;
            latitude = i * 512 + work->angle;
        } else {
            longitude = i * 256 - work->angle;
            latitude = i * 512 + work->angle;
        }
        for (j = 0; j < 2; j++) {
            if (!(work->frame_count & 1)) {
                width = (j * 128 + 32) * work->width / 1024;
            } else {
                width = (j * 160 + 48) * work->width / 1024;
            }
            if (j == 0) {
                setVector(&points[i][j], 0, 0, 0);
            } else {
                (j + points[i])->vx = rcos(longitude) * (((rsin(latitude) * 3 << 10) * j) >> 12) >> 12;
                (j + points[i])->vy = (rcos(latitude) * 3 << 10) * j >> 12;
                (j + points[i])->vz = rsin(longitude) * (((rsin(latitude) * 3 << 10) * j) >> 12) >> 12;
            }
            (j + displaced[i])->vx = points[i][j].vx + (rcos(angle) * width >> 12);
            (j + displaced[i])->vy = points[i][j].vy;
            (j + displaced[i])->vz = points[i][j].vz + (rsin(angle) * width >> 12);
        }
    }
    if (!(work->frame_count & 1)) {
        matrix_scale = work->scale;
    } else {
        matrix_scale = work->scale + work->scale / 8;
    }
    i = 0;
    setVector(&rotation, 0, 0, 0);
    local.t[0] = work->origin.vx;
    local.t[1] = work->origin.vy;
    local.t[2] = work->origin.vz;
    setVector(&scale, matrix_scale, matrix_scale, matrix_scale);
    RotMatrix(&rotation, &local);
    ScaleMatrix(&local, &scale);
    coordinate.coord = local;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &screen);
    GsSetLsMatrix(&screen);
    for (; i < 16; i++) {
        for (j = 0; j < 2; j++) {
            if (j == 1) {
                depth[i][j] = RotTransPers4(&points[i][0], &points[i][1],
                    &points[i][0], &points[i][1],
                    (long *)&projected[i][0], (long *)&projected[i][1],
                    (long *)&projected[i][0], (long *)&projected[i][1],
                    (long *)&interpolation, (long *)&flags[i][j]);
                RotTransPers(&displaced[i][j], (long *)&displaced_projected[i][j],
                             (long *)&interpolation, (long *)&flag);
                x_delta = (s16)projected[i][j] - (s16)projected[i][0];
                y_delta = (projected[i][j] >> 16) - (projected[i][0] >> 16);
                angles[i][j] = ratan2(y_delta, x_delta) + 3072;
                widths[i][j] = (s16)displaced_projected[i][j] - (s16)projected[i][j];
                dx[i][j] = rcos(angles[i][j]) * widths[i][j] >> 12;
                dy[i][j] = rsin(angles[i][j]) * widths[i][j] >> 12;
            } else {
                depth[i][j] = RotTransPers4(&points[i][j], &points[i][j + 1],
                    &points[i][j], &points[i][j + 1],
                    (long *)&projected[i][j], (long *)&projected[i][j + 1],
                    (long *)&projected[i][j], (long *)&projected[i][j + 1],
                    (long *)&interpolation, (long *)&flags[i][j]);
                RotTransPers(&displaced[i][j], (long *)&displaced_projected[i][j],
                             (long *)&interpolation, (long *)&flag);
                x_delta = (s16)projected[i][j + 1] - (s16)projected[i][j];
                y_delta = (projected[i][j + 1] >> 16) - (projected[i][j] >> 16);
                angles[i][j] = ratan2(y_delta, x_delta) + 3072;
                widths[i][j] = (s16)displaced_projected[i][j] - (s16)projected[i][j];
                dx[i][j] = rcos(angles[i][j]) * widths[i][j] >> 12;
                dy[i][j] = rsin(angles[i][j]) * widths[i][j] >> 12;
            }
        }
    }
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 1; j++) {
            quad->x0 = projected[i][j] + dx[i][j];
            quad->y0 = (projected[i][j] >> 16) + dy[i][j];
            quad->x1 = projected[i][j + 1] + dx[i][j + 1];
            quad->y1 = (projected[i][j + 1] >> 16) + dy[i][j + 1];
            quad->x2 = projected[i][j];
            quad->y2 = projected[i][j] >> 16;
            quad->x3 = projected[i][j + 1];
            quad->y3 = projected[i][j + 1] >> 16;
            r0 = 128 - j * 64;
            g0 = 96 - j * 64;
            b0 = 0;
            r1 = 160 - j * 64;
            g1 = 160 - j * 64;
            b1 = 128 - j * 64;
            if (j + 1 == 1) {
                setRGB0(quad, r0, g0, b0);
                setRGB1(quad, 0, 0, 0);
                setRGB2(quad, r1, g1, b1);
                setRGB3(quad, 0, 0, 0);
            } else {
                setRGB0(quad, r0, g0, b0);
                setRGB1(quad, r0, g0, b0);
                setRGB2(quad, r1, g1, b1);
                setRGB3(quad, r1, g1, b1);
            }
            depth[i][j] = 0;
            flags[i][j] = 0;
            if (depth[i][j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(quad, ot, depth[i][j]);
            }
            quad->x0 = projected[i][j] - dx[i][j];
            quad->y0 = (projected[i][j] >> 16) - dy[i][j];
            quad->x1 = projected[i][j + 1] - dx[i][j + 1];
            quad->y1 = (projected[i][j + 1] >> 16) - dy[i][j + 1];
            quad->x2 = projected[i][j];
            quad->y2 = projected[i][j] >> 16;
            quad->x3 = projected[i][j + 1];
            quad->y3 = projected[i][j + 1] >> 16;
            if (j + 1 == 1) {
                setRGB0(quad, r0, g0, b0);
                setRGB1(quad, 0, 0, 0);
                setRGB2(quad, r1, g1, b1);
                setRGB3(quad, 0, 0, 0);
            } else {
                setRGB0(quad, r0, g0, b0);
                setRGB1(quad, r0, g0, b0);
                setRGB2(quad, r1, g1, b1);
                setRGB3(quad, r1, g1, b1);
            }
            depth[i][j] = 0;
            flags[i][j] = 0;
            if (depth[i][j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(quad, ot, depth[i][j]);
            }
        }
    }
    if (work->timing->ramp_end <= work->frame && work->scale < 4096) {
        work->scale = ((work->frame - work->timing->ramp_end) << 12) /
            (work->timing->growth_end - work->timing->ramp_end);
        if (work->scale >= 4096) {
            work->scale = 4096;
        }
    }
    if (work->timing->growth_end <= work->frame && work->width > 0) {
        work->width = 1024 - ((work->frame - work->timing->growth_end) << 10) /
            (work->timing->shrink_end - work->timing->growth_end);
        if (work->width <= 0) {
            work->width = 0;
        }
    }
    work->angle += 48;
}
