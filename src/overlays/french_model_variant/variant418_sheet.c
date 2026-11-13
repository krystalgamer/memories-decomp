#include "../../types.h"

#include "../model_variant/model_variant.h"
#include "../model_variant/variant401_entry.h"

void FrenchModelVariant418_DrawSheets(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    Variant401EntryState *work;
    Variant401EntryBand *record;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    GsOT *ot;
    POLY_GT4 *poly;
    ModelVariantSheet *sheet;
    s32 i;
    s32 k;
    SVECTOR *v;
    s32 bias;
    s32 otz;

    work = (Variant401EntryState *)ctx;
    record = work->bands;
    sheet = work->sheets;
    ot = Graphics_GetDefaultOrderingTable();
    poly = &work->packets.textured[1];
    for (i = 0; i < 2; i++, sheet++) {
        bias = 0;
        if (work->clock.frame_count & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i == 0) {
            s32 amount;

            amount = 0;
            if (record->offset[8] > 0) {
                amount = record->offset[8];
            }
            m.t[0] = work->matrix.t[0] + work->aim.direction.vx * amount / 0x400;
            m.t[1] = work->matrix.t[1] + work->aim.direction.vy * amount / 0x400;
            m.t[2] = work->matrix.t[2] + work->aim.direction.vz * amount / 0x400;
            scale.vx = sheet->size;
            scale.vy = sheet->size;
            scale.vz = sheet->size;
        } else {
            s32 amount;

            amount = 0;
            if (record->offset[0] > 0) {
                amount = record->offset[0];
            }
            m.t[0] = work->matrix.t[0] + work->aim.direction.vx * amount / 0x400;
            m.t[1] = work->matrix.t[1] + work->aim.direction.vy * amount / 0x400;
            m.t[2] = work->matrix.t[2] + work->aim.direction.vz * amount / 0x400;
            scale.vx = sheet->size + bias;
            scale.vy = sheet->size + bias;
            scale.vz = sheet->size + bias;
        }
        RotMatrix(&rot, &m);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        ReadRotMatrix(&ls);
        RotMatrix(&rot, &ls);
        ScaleMatrix(&ls, &scale);
        SetRotMatrix(&ls);
        for (k = 0, v = sheet->v0; k < 4; k++, v++) {
            otz = RotTransPers4(v, &sheet->v1[k], &sheet->v2[k], &sheet->v3[k],
                                (long *)&poly->x0, (long *)&poly->x1,
                                (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
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
            otz = otz * 8 / 10;
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
        if (i == 0) {
            sheet->size = 0x800;
            if (work->tail.phase >= 3) {
                sheet->size = 0;
            }
        } else {
            if (work->tail.phase == 0) {
                if (sheet->size < 0x1000) {
                    ModelVariantWebConfig *timing;

                    timing = work->config;
                    sheet->size = ((work->clock.frame - timing->start) << 12) /
                                  (timing->burst_start - timing->start);
                    if (sheet->size >= 0x1000) {
                        sheet->size = 0x1000;
                        work->tail.phase = 1;
                    }
                }
            } else if (work->tail.phase < 2) {
                sheet->size = 0x1000;
            } else if (work->tail.phase == 2) {
                if (sheet->size < 0x8000) {
                    sheet->size += work->clock.step << 12;
                    if (sheet->size >= 0x8000) {
                        sheet->size = 0x8000;
                    }
                }
            } else if (work->tail.phase == 3) {
                if (sheet->size > 0) {
                    sheet->size -= work->clock.step << 8;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        work->tail.phase = 4;
                    }
                }
            }
        }
    }
}
