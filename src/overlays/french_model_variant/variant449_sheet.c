#include "../../types.h"
#include "../model_variant/variant432_entry.h"

void FrenchModelVariant449_DrawSheet(Variant432EntryState *context)
{
    Variant432EntryState *work = context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    /* Retail leaves 16 unaccessed bytes before the projection outputs. */
    u8 unknown_stack[16];
    long interpolation;
    long flag;
    GsOT *ot;
    ModelVariantQuadSheet *sheet;
    POLY_GT4 *quad;
    s32 i, j;
    s32 bias;
    s32 depth;
    SVECTOR *point;

    ot = Graphics_GetDefaultOrderingTable();
    ratan2(work->view_delta.vz, work->view_delta.vx);
    ratan2(work->view_delta.vy, work->view_delta.vx);
    sheet = work->sheets;
    quad = work->packets.textured;
    if (!(work->frame_count & 1)) {
        bias = 0;
    } else {
        bias = work->scale / 8;
    }
    quad++;
    for (i = 0; i < 1; i++, sheet++) {
        matrix.t[0] = work->origin.vx + work->path.vx * work->factor / 1024;
        matrix.t[1] = work->origin.vy + work->path.vy * work->factor / 1024;
        matrix.t[2] = work->origin.vz + work->path.vz * work->factor / 1024;
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
                                 (long *)&quad->x0, (long *)&quad->x1,
                                 (long *)&quad->x2, (long *)&quad->x3,
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
    if (work->tail.phase == 0 && work->elapsed >= work->config->scale_start) {
        if (work->scale < 4096) {
        work->scale = ((work->elapsed - work->config->scale_start) << 12) /
                      (work->config->scale_end - work->config->scale_start);
        if (work->scale >= 4096) {
            work->scale = 4096;
            work->tail.phase = 1;
        }
        }
    } else if (work->tail.phase == 1 && work->elapsed >= work->config->move_start) {
        work->factor = ((work->elapsed - work->config->move_start) << 10) /
                   (work->config->move_end - work->config->move_start);
        if (work->factor >= 1024) {
        work->factor = 1024;
        work->tail.phase = 2;
        }
    } else if (work->tail.phase == 2) {
        if (work->scale < 16384) {
            work->scale += work->step << 11;
            if (work->scale >= 16384) {
                work->scale = 16384;
                work->tail.phase = 3;
            }
        }
    } else if (work->tail.phase == 3 && work->config->fade_start <= work->elapsed) {
        if (work->scale > 0) {
        work->scale -= work->step << 7;
        if (work->scale <= 0) {
            work->scale = 0;
            work->tail.phase = 7;
        }
        }
    }
}
