#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "e502_entry_pal.h"

void FrenchModelVariant336_DrawSheets(Variant336EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long projection;
    long flag;
    Variant336EntryRecord *node;
    ModelVariantPointSheet *sheet;
    GsOT *ot;
    Variant336EntryState *work;
    POLY_GT4 *poly;
    s32 i;
    s32 k;
    s32 bias;
    s32 depth;

    work = context;
    node = work->records;
    sheet = work->sheets;
    ot = Graphics_GetDefaultOrderingTable();
    ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx);
    poly = &work->packets.textured[1];
    for (i = 0; i < work->sheet_count; i++, sheet++) {
        bias = 0;
        if (work->clock.frame_count & 1) {
            bias = sheet->size / 8;
        }
        setVector(&rot, 0, 0, 0);
        if ((i & 1) == 0) {
            matrix.t[0] = work->origin[0] + node->origin.vx;
            matrix.t[1] = node->origin.vy;
            matrix.t[2] = work->origin[2] + node->origin.vz;
        } else {
            matrix.t[0] = work->origin[0] + node->end.vx;
            matrix.t[1] = node->end.vy;
            matrix.t[2] = work->origin[2] + node->end.vz;
        }
        setVector(&scale, sheet->size + bias, sheet->size + bias, sheet->size + bias);
        RotMatrix(&rot, &matrix);
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
            poly->r0 = sheet->inner.r;
            poly->g0 = sheet->inner.g;
            poly->b0 = sheet->inner.b;
            poly->r1 = sheet->inner.r;
            poly->g1 = sheet->inner.g;
            poly->b1 = sheet->inner.b;
            poly->r2 = sheet->inner.r;
            poly->g2 = sheet->inner.g;
            poly->b2 = sheet->inner.b;
            poly->r3 = sheet->outer.r;
            poly->g3 = sheet->outer.g;
            poly->b3 = sheet->outer.b;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, depth);
            }
        }
        if ((i & 1) == 0) {
            if (node->state == 0) {
                if (sheet->size < 4096) {
                    sheet->size += work->clock.step * 256;
                    if (sheet->size >= 4096) {
                        sheet->size = 4096;
                        node->state = 1;
                        if (work->phase == 0) {
                            work->phase = 1;
                        }
                    }
                }
            } else if (node->state == 2) {
                sheet->size = node->extent * 4;
            }
        } else {
            if (node->state == 1) {
                if (sheet->size < 8192) {
                    sheet->size += work->clock.step * 1024;
                    if (sheet->size >= 8192) {
                        sheet->size = 8192;
                        node->state = 2;
                    }
                }
            } else if (node->state == 2) {
                sheet->size = node->extent * 8;
            }
            if (i + 1 == 8 && node->state == 1
                && work->phase == 1) {
                work->phase = 2;
            }
            node++;
        }
    }
}
