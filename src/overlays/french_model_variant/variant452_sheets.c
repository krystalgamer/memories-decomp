#include "../../types.h"
#include "variant452_entry_pal.h"

void FrenchModelVariant452_DrawSheets(Variant452EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX matrix;
    MATRIX local;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant452EntryState *work = context;
    ModelVariantSheet *sheet = work->sheets;
    GsOT *ot;
    POLY_GT4 *poly;
    s32 i, k;
    s32 bias;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    poly = &work->packets.textured[1];
    for (i = 0; i < 1; i++, sheet++) {
        bias = 0;
        if (work->clock.frame_count & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        matrix.t[0] = work->position.vx;
        matrix.t[1] = work->position.vy;
        matrix.t[2] = work->position.vz;
        scale.vx = sheet->size + bias;
        scale.vy = sheet->size + bias;
        scale.vz = sheet->size + bias;
        RotMatrix(&rot, &matrix);
        coord.coord = matrix;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &local);
        GsSetLsMatrix(&local);
        ReadRotMatrix(&local);
        RotMatrix(&rot, &local);
        ScaleMatrix(&local, &scale);
        SetRotMatrix(&local);
        for (k = 0; k < 4; k++) {
            depth = RotTransPers4(
                &sheet->v0[k], &sheet->v1[k], &sheet->v2[k], &sheet->v3[k],
                (long *)&poly->x0, (long *)&poly->x1,
                (long *)&poly->x2, (long *)&poly->x3,
                &p, &flag);
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
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, depth);
            }
        }
        if (sheet->size < 4096 && work->phase == 0) {
            sheet->size = ((work->clock.frame - work->config->start) << 12)
                / (work->config->scale_end - work->config->start);
            if (sheet->size >= 4096) {
                sheet->size = 4096;
                work->phase = 1;
            }
        } else if (work->clock.frame >= work->config->fade_start) {
            sheet->size = 8192 - ((work->clock.frame - work->config->fade_start) << 13)
                / (work->config->fade_end - work->config->fade_start);
            if (sheet->size <= 0) {
                sheet->size = 0;
                if (work->phase == 3) {
                    work->phase = 4;
                }
            }
        } else if (sheet->size < 8192 && work->phase == 2) {
            sheet->size += work->clock.step << 10;
            if (sheet->size >= 8192) {
                sheet->size = 8192;
                work->phase = 3;
            }
        }
        if (work->phase == 1 && work->progress < 2048) {
            work->progress += work->clock.step << 4;
            if (work->progress >= 2048) {
                work->progress = 2048;
                work->phase = 2;
            }
        }
    }
}
