#define func_8013D7C0 func_8013D754
#include "../../types.h"
#include "../../overlays/spanish_model_variant/variant450_quads.h"

void func_8013D7C0(u8 *ctx)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG interpolation;
    PSXLONG flag;
    Variant450QuadView *work;
    Variant450QuadGroup *group;
    Variant450Primary *primary;
    Variant450Primary *primary_base;
    POLY_GT4 *quad;
    GsOT *ot;
    SVECTOR *point;
    s32 i, j;
    s32 extra;
    s32 depth;
    s32 primary_index;
    u8 r, g, b, r1, g1, b1;

    work = (Variant450QuadView *)ctx;
    group = work->groups;
    ot = func_80058F10();
    quad = &work->quad;
    primary_base = work->primary;
    primary_index = 0;
    for (i = 0; i < 7; i++, group++) {
        primary = primary_base + primary_index;
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = work->translation.vx;
            matrix.t[1] = work->translation.vy;
            matrix.t[2] = work->translation.vz;
            extra = 0;
            if (work->flags & 1) {
                extra = group->scale / 8;
            }
        } else {
            matrix.t[0] = primary->translation.vx;
            matrix.t[1] = primary->translation.vy;
            matrix.t[2] = primary->translation.vz;
            extra = 0;
            if (work->flags & 1) {
                extra = group->scale / 4;
            }
        }
        scale.vx = group->scale + extra;
        scale.vy = group->scale + extra;
        scale.vz = group->scale + extra;
        RotMatrix(&rotation, &matrix);
        coord.super = 0;
        coord.flg = 0;
        coord.coord = matrix;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        ReadRotMatrix(&ls);
        RotMatrix(&rotation, &ls);
        ScaleMatrix(&ls, &scale);
        SetRotMatrix(&ls);
        for (j = 0, point = group->points; j < 4; j++, point++) {
            depth = RotTransPers4(point, &group->points[j+4], &group->points[j+8],
                                 &group->points[j+12], (PSXLONG *)&quad->x0,
                                 (PSXLONG *)&quad->x1, (PSXLONG *)&quad->x2,
                                 (PSXLONG *)&quad->x3, &interpolation, &flag);
            if (group->fading == 0) {
                setRGB0(quad, group->color[0], group->color[1], group->color[2]);
                setRGB1(quad, group->color[0], group->color[1], group->color[2]);
                setRGB2(quad, group->color[0], group->color[1], group->color[2]);
                setRGB3(quad, group->end_color[0], group->end_color[1], group->end_color[2]);
            } else {
                r = group->color[0] * group->brightness / 1024;
                g = group->color[1] * group->brightness / 1024;
                b = group->color[2] * group->brightness / 1024;
                r1 = group->end_color[0] * group->brightness / 1024;
                g1 = group->end_color[1] * group->brightness / 1024;
                b1 = group->end_color[2] * group->brightness / 1024;
                setRGB0(quad, r, g, b);
                setRGB1(quad, r, g, b);
                setRGB2(quad, r, g, b);
                setRGB3(quad, r1, g1, b1);
            }
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, depth);
            }
        }
        if (i == 0) {
            if (work->phase == 0 && group->scale < 4096) {
                group->scale = (work->elapsed-work->config->grow_start) * 4096 /
                               (work->config->grow_end-work->config->grow_start);
                if (group->scale >= 4096) {
                    group->scale = 4096;
                    work->phase = 1;
                }
            }
            if (work->elapsed >= work->config->fade_start) {
                group->scale = 4096 - (work->elapsed-work->config->fade_start) * 4096 /
                                     (work->config->fade_end-work->config->fade_start);
                if (group->scale <= 0) {
                    group->scale = 0;
                }
            }
        } else {
            if (group->scale < 16384 && primary->threshold >= 1024) {
                group->scale += work->step * 4096;
                if (group->scale >= 16384) {
                    group->scale = 16384;
                    group->fading = 1;
                    if (i+1 == 7) {
                        work->phase = 3;
                    }
                }
            } else if (group->brightness > 0 && group->fading == 1) {
                group->brightness -= work->step * 64;
                if (group->brightness <= 0) {
                    group->brightness = 0;
                    if (i+1 == 7 && work->phase == 3) {
                        work->phase = 5;
                    }
                }
            }
            primary_index++;
        }
    }
}
