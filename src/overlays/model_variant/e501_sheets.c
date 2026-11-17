#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "e501_entry_pal.h"

void FrenchModelVariant335_DrawSheets(Model335EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long projection;
    long flag;
    ModelVariantPointSheet *sheet;
    GsOT *ot;
    Model335EntryState *work;
    POLY_GT4 *poly;
    s32 i;
    s32 k;
    s32 bias;
    s32 depth;
    s32 turn;
    s32 angle;
    s32 radius;
    s32 rotation_bias;

    work = context;
    sheet = work->sheets;
    ot = Graphics_GetDefaultOrderingTable();
    poly = &work->textured[1];
    turn = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx);
    rotation_bias = work->spin + 3072;
    turn += rotation_bias;
    for (i = 0; i < 4; i++, sheet++) {
        bias = 0;
        if (work->clock.frame_count & 1) {
            bias = sheet->size / 8;
        }
        setVector(&rot, 0, 0, 0);
        if (i == 3) {
            matrix.t[0] = work->aim.target.vx;
            matrix.t[1] = work->aim.target.vy;
            matrix.t[2] = work->aim.target.vz;
        } else {
            angle = turn + (i << 12) / 3;
            radius = work->orbit_diameter / 2;
            matrix.t[0] = work->origin.vx + (rcos(angle) * radius >> 12);
            matrix.t[1] = work->origin.vy;
            matrix.t[2] = work->origin.vz + (rsin(angle) * radius >> 12);
        }
        setVector(&scale, sheet->size + bias, sheet->size + bias, sheet->size + bias);
        RotMatrix(&rot, &matrix);
        ScaleMatrix(&matrix, &scale);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        ReadRotMatrix(&light);
        RotMatrix(&rot, &light);
        ScaleMatrix(&light, &scale);
        SetRotMatrix(&light);
        for (k = 0; k < 4; k++) {
            depth = RotTransPers4(
                &sheet->points[k], &sheet->points[k + 4],
                &sheet->points[k + 8], &sheet->points[k + 12],
                (long *)&poly->x0, (long *)&poly->x1,
                (long *)&poly->x2, (long *)&poly->x3,
                &projection, &flag);
            setRGB0(poly, sheet->inner.r, sheet->inner.g, sheet->inner.b);
            setRGB1(poly, sheet->inner.r, sheet->inner.g, sheet->inner.b);
            setRGB2(poly, sheet->inner.r, sheet->inner.g, sheet->inner.b);
            setRGB3(poly, sheet->outer.r, sheet->outer.g, sheet->outer.b);
            if ((work->phase != 0 || i == 0)
                && depth >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, depth);
            }
        }
        if (i == 3) {
            if (work->phase == 5) {
                if (sheet->size < 16384) {
                    sheet->size += work->clock.step * 2048;
                    if (sheet->size >= 16384) {
                        sheet->size = 16384;
                        work->phase = 6;
                    }
                }
            } else if (work->phase >= 6) {
                if (work->clock.frame
                    > work->config->shrink_start
                    && sheet->size > 0) {
                    sheet->size = 16384 - (((work->clock.frame
                        - work->config->shrink_start) << 14)
                        / (work->config->shrink_end
                        - work->config->shrink_start));
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                    }
                }
            }
        } else {
            if (work->phase == 0) {
                if (sheet->size < 4096) {
                    sheet->size = ((work->clock.frame
                        - work->config->start) << 12)
                        / (work->config->grow_end
                        - work->config->start);
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        work->phase = 1;
                    }
                }
            } else if (work->phase == 1) {
                if (work->orbit_diameter < 1024) {
                    work->orbit_diameter = ((work->clock.frame
                        - work->config->grow_end) << 10)
                        / (work->config->orbit_end
                        - work->config->grow_end);
                    if (work->orbit_diameter >= 1024) {
                        work->orbit_diameter = 1024;
                        work->phase = 2;
                    }
                }
            } else if (work->phase == 2) {
                if (work->lift < 256) {
                    work->lift = ((work->clock.frame
                        - work->config->orbit_end) << 8)
                        / (work->config->lift_end
                        - work->config->orbit_end);
                    if (work->lift >= 256) {
                        work->lift = 256;
                        work->phase = 3;
                    }
                }
            } else if (work->phase == 3) {
                if (work->travel_angle < 1024) {
                    work->travel_angle = ((work->clock.frame
                        - work->config->lift_end) << 10)
                        / (work->config->travel_end
                        - work->config->lift_end);
                    if (work->travel_angle >= 1024) {
                        work->travel_angle = 1024;
                        work->phase = 4;
                    }
                }
            } else if (work->phase >= 6) {
                if (work->clock.frame
                    > work->config->shrink_start
                    && sheet->size > 0) {
                    sheet->size = 4096 - (((work->clock.frame
                        - work->config->shrink_start) << 12)
                        / (work->config->shrink_end
                        - work->config->shrink_start));
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        work->phase = 7;
                    }
                }
            }
        }
    }
}
