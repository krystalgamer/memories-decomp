#include "../../types.h"
#include "variant452_sheets.h"

void func_8013C0E0(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX matrix;
    MATRIX local;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    Variant452SheetView *work = (Variant452SheetView *)ctx;
    ModelVariantSheet *sheet = work->sheets;
    GsOT *ot;
    POLY_GT4 *poly;
    s32 i, k;
    s32 bias;
    s32 depth;

    ot = func_80058F10();
    poly = &work->poly;
    for (i = 0; i < 1; i++, sheet++) {
        bias = 0;
        if (work->flags & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        matrix.t[0] = work->translation[0];
        matrix.t[1] = work->translation[1];
        matrix.t[2] = work->translation[2];
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
                (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3,
                &p, &flag);
            poly->r0 = sheet->inner[0];
            poly->g0 = sheet->inner[1];
            poly->b0 = sheet->inner[2];
            poly->r1 = sheet->inner[0];
            poly->g1 = sheet->inner[1];
            poly->b1 = sheet->inner[2];
            poly->r2 = sheet->inner[0];
            poly->g2 = sheet->inner[1];
            poly->b2 = sheet->inner[2];
            poly->r3 = sheet->outer[0];
            poly->g3 = sheet->outer[1];
            poly->b3 = sheet->outer[2];
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, depth);
            }
        }
        if (sheet->size < 4096 && work->phase == 0) {
            sheet->size = ((work->elapsed - work->timing->scale_start) << 12)
                / (work->timing->scale_end - work->timing->scale_start);
            if (sheet->size >= 4096) {
                sheet->size = 4096;
                work->phase = 1;
            }
        } else if (work->elapsed >= work->timing->fade_start) {
            sheet->size = 8192 - ((work->elapsed - work->timing->fade_start) << 13)
                / (work->timing->fade_end - work->timing->fade_start);
            if (sheet->size <= 0) {
                sheet->size = 0;
                if (work->phase == 3) {
                    work->phase = 4;
                }
            }
        } else if (sheet->size < 8192 && work->phase == 2) {
            sheet->size += work->step << 10;
            if (sheet->size >= 8192) {
                sheet->size = 8192;
                work->phase = 3;
            }
        }
        if (work->phase == 1 && work->field3394 < 2048) {
            work->field3394 += work->step << 4;
            if (work->field3394 >= 2048) {
                work->field3394 = 2048;
                work->phase = 2;
            }
        }
    }
}
