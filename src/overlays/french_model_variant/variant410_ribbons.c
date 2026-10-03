#include "../../types.h"
#include "variant410_ribbons.h"

void func_8013BF58(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG flags[9][17];
    PSXLONG interpolation;
    PSXLONG flag;
    Family410RibbonView *work;
    GsOT *ot;
    s16 i;
    s32 angle;
    s32 yaw;
    s32 pitch;
    s16 length;
    Family410Ribbon *ribbon;
    POLY_G4 *quad;
    s16 k;
    s32 progress;
    s32 dx;
    s32 dy;

    work = (Family410RibbonView *)context;
    ot = func_80058F10();
    yaw = ratan2(work->view_direction[2], work->view_direction[0]) + 3072;
    pitch = ratan2(work->view_direction[1], work->view_direction[0]);
    ribbon = work->ribbons;
    quad = &work->quad;
    if (work->phase >= 0) {
        for (i = 0; i < 9; i++, ribbon++) {
            if (i == 0) {
                angle = 1024;
            } else if ((s16)(i % 2) == 1) {
                angle = 1024 + (i + 1) * 200;
            } else {
                angle = 1024 - i * 200;
            }
            length = 8;
            for (k = 0; k < 17; k++) {
                progress = ribbon->progress[k];
                if (progress <= 0) {
                    progress = 0;
                }
                setVector(&ribbon->a[k],
                          -work->direction[0] * progress / 1024
                              + (rcos(angle) * (-(rsin(progress * 2) * 128) >> 12) >> 12),
                          -work->direction[1] * progress / 1024
                              + (rsin(angle) * (-(rsin(progress * 2) * 128) >> 12) >> 12),
                          -work->direction[2] * progress / 1024);
                setVector(&ribbon->b[k],
                          ribbon->a[k].vx + (rcos(yaw) * length >> 12),
                          ribbon->a[k].vy,
                          ribbon->a[k].vz + (rsin(yaw) * length >> 12));
                if (ribbon->progress[k] < 1024) {
                    ribbon->progress[k] += work->step * 40;
                    if (ribbon->progress[k] >= 1024) {
                        ribbon->progress[k] = 1024;
                        if (k == 0 && work->phase == 1) {
                            work->phase = 2;
                        }
                    }
                }
            }
        }
        setVector(&rotation, 0, 0, 0);
        matrix.t[0] = work->target.vx;
        matrix.t[1] = work->target.vy;
        matrix.t[2] = work->target.vz;
        setVector(&scale, 4096, 4096, 4096);
        RotMatrix(&rotation, &matrix);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        ribbon = work->ribbons;
        for (i = 0; i < 9; i++, ribbon++) {
            for (k = 0; k < 17; k++) {
                if (k == 16) {
                    ribbon->depth[k] = RotTransPers4(&ribbon->a[k - 1], &ribbon->a[k],
                                                   &ribbon->a[k - 1], &ribbon->a[k],
                                                   (PSXLONG *)&ribbon->sa[k - 1], (PSXLONG *)&ribbon->sa[k],
                                                   (PSXLONG *)&ribbon->sa[k - 1], (PSXLONG *)&ribbon->sa[k],
                                                   &interpolation, &flags[i][k]);
                    RotTransPers(&ribbon->b[k], (PSXLONG *)&ribbon->sb[k], &interpolation, &flag);
                    dx = ribbon->sa[k].point.vx - ribbon->sa[k - 1].point.vx;
                    dy = ribbon->sa[k].point.vy - ribbon->sa[k - 1].point.vy;
                    ribbon->angle[k] = ratan2(dy, dx) - 3072;
                    ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                    ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                } else {
                    ribbon->depth[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1],
                                                   &ribbon->a[k], &ribbon->a[k + 1],
                                                   (PSXLONG *)&ribbon->sa[k], (PSXLONG *)&ribbon->sa[k + 1],
                                                   (PSXLONG *)&ribbon->sa[k], (PSXLONG *)&ribbon->sa[k + 1],
                                                   &interpolation, &flags[i][k]);
                    RotTransPers(&ribbon->b[k], (PSXLONG *)&ribbon->sb[k], &interpolation, &flag);
                    dx = ribbon->sa[k + 1].point.vx - ribbon->sa[k].point.vx;
                    dy = ribbon->sa[k + 1].point.vy - ribbon->sa[k].point.vy;
                    ribbon->angle[k] = ratan2(dy, dx) - 3072;
                    ribbon->width[k] = ribbon->sb[k].point.vx - ribbon->sa[k].point.vx;
                    ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                    ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
                }
            }
        }
        ribbon = work->ribbons;
        for (i = 0; i < 9; i++, ribbon++) {
            for (k = 0; k < 16; k++) {
                quad->x0 = ribbon->sa[k].point.vx + ribbon->ox[k];
                quad->y0 = (ribbon->sa[k].packed >> 16) + ribbon->oy[k];
                quad->x1 = ribbon->sa[k + 1].point.vx + ribbon->ox[k + 1];
                quad->y1 = (ribbon->sa[k + 1].packed >> 16) + ribbon->oy[k + 1];
                quad->x2 = ribbon->sa[k].point.vx - ribbon->ox[k];
                quad->y2 = (ribbon->sa[k].packed >> 16) - ribbon->oy[k];
                quad->x3 = ribbon->sa[k + 1].point.vx - ribbon->ox[k + 1];
                quad->y3 = (ribbon->sa[k + 1].packed >> 16) - ribbon->oy[k + 1];
                if (k + 1 == 16) {
                    setRGB0(quad, ribbon->outer.r, ribbon->outer.g, ribbon->outer.b);
                    setRGB1(quad, 0, 0, 0);
                    setRGB2(quad, ribbon->inner.r, ribbon->inner.g, ribbon->inner.b);
                    setRGB3(quad, 0, 0, 0);
                } else {
                    setRGB0(quad, ribbon->outer.r, ribbon->outer.g, ribbon->outer.b);
                    setRGB1(quad, ribbon->outer.r, ribbon->outer.g, ribbon->outer.b);
                    setRGB2(quad, ribbon->inner.r, ribbon->inner.g, ribbon->inner.b);
                    setRGB3(quad, ribbon->inner.r, ribbon->inner.g, ribbon->inner.b);
                }
                if (ribbon->depth[k] >= 0 && flags[i][k] >= 0) {
                    func_8005B260((u32 *)quad, ot, (u16)ribbon->depth[k], 1);
                }
            }
            if (i + 1 == 9 && ribbon->progress[16] >= 1024 && work->phase == 2) {
                work->phase = 3;
            }
        }
    }
    work->animation_angle += work->step * 384;
    work->animation_angle2 += work->step << 6;
}
