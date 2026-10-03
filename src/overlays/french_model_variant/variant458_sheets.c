#include "../../types.h"
#include "variant458_sheets.h"

void func_8013BC10(u8 *context)
{
    Variant458View *work = (Variant458View *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    /* Retail leaves 16 unaccessed bytes before the projection outputs. */
    u8 unknown_stack[16];
    PSXLONG interpolation;
    PSXLONG flag;
    GsOT *ot;
    Variant458Sheet *sheet;
    POLY_GT4 *quad;
    s32 i, j;
    s32 bias;
    s32 depth;
    SVECTOR *point;

    ot = func_80058F10();
    ratan2(work->axis_z, work->axis_x);
    ratan2(work->axis_y, work->axis_x);
    sheet = work->sheets;
    quad = work->quads;
    if (!(work->flags & 1)) {
        bias = 0;
    } else {
        bias = work->scale / 8;
    }
    quad++;
    for (i = 0; i < 7; i++, sheet++) {
        matrix.t[0] = work->origins[i].t[0] + work->delta[i].vx * work->factor / 1024;
        matrix.t[1] = work->origins[i].t[1] + work->delta[i].vy * work->factor / 1024;
        matrix.t[2] = work->origins[i].t[2] + work->delta[i].vz * work->factor / 1024;
        scale.vx = work->scale + bias;
        scale.vy = work->scale + bias;
        scale.vz = work->scale + bias;
        setVector(&rotation, 0, 0, 0);
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
        for (j = 0, point = sheet->points[0]; j < 4; j++, point++) {
            depth = RotTransPers4(point, &sheet->points[1][j],
                                 &sheet->points[2][j], &sheet->points[3][j],
                                 (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                 (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                 &interpolation, &flag);
            quad->r0 = sheet->color[1].r;
            quad->g0 = sheet->color[1].g;
            quad->b0 = sheet->color[1].b;
            quad->r1 = sheet->color[1].r;
            quad->g1 = sheet->color[1].g;
            quad->b1 = sheet->color[1].b;
            quad->r2 = sheet->color[1].r;
            quad->g2 = sheet->color[1].g;
            quad->b2 = sheet->color[1].b;
            quad->r3 = sheet->color[0].r;
            quad->g3 = sheet->color[0].g;
            quad->b3 = sheet->color[0].b;
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, depth);
            }
        }
    }
    if (work->phase == 0 && work->elapsed >= work->timing->scale_start) {
        if (work->scale < 2048) {
        work->scale = ((work->elapsed - work->timing->scale_start) << 11) /
                      (work->timing->scale_end - work->timing->scale_start);
        if (work->scale >= 2048) {
            work->scale = 2048;
            work->phase = 1;
        }
        }
    } else if (work->phase == 1 && work->elapsed >= work->timing->move_start) {
        work->factor = ((work->elapsed - work->timing->move_start) << 10) /
                   (work->timing->move_end - work->timing->move_start);
        if (work->factor >= 1024) {
        work->factor = 1024;
        work->phase = 2;
        }
    } else if (work->phase == 2) {
        if (work->scale < 8192) {
            work->scale += work->step << 10;
            if (work->scale >= 8192) {
                work->scale = 8192;
                work->phase = 3;
            }
        }
    } else if (work->phase == 4 && work->timing->fade_start <= work->elapsed) {
        if (work->scale > 0) {
        work->scale -= work->step << 6;
        if (work->scale <= 0) {
            work->scale = 0;
            work->phase = 7;
        }
        }
    }
}
