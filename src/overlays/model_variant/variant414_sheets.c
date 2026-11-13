#include "../../types.h"

#include "model_variant.h"
#include "../french_model_variant/variant431_entry.h"

void FrenchModelVariant431_DrawSheets(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    Variant431EntryState *work;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant431EntryBand *object;
    GsOT *ot;
    POLY_GT4 *poly;
    ModelVariantSheet *sheet;
    s32 i;
    s32 k;
    s32 bias;
    s32 level;
    s32 otz;

    work = (Variant431EntryState *)ctx;
    object = work->bands;
    sheet = work->sheets;
    ot = Graphics_GetDefaultOrderingTable();
    poly = &work->packets.textured[1];
    for (i = 0; i < 6; i++, sheet++, object++) {
        level = 0;
        if (object->progress[0] > 0) {
            level = object->progress[0];
        }
        bias = 0;
        if (work->frame_count & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i == 5) {
            m.t[0] = work->target.vx;
            m.t[1] = work->target.vy;
            m.t[2] = work->target.vz;
            scale.vx = sheet->size + bias;
            scale.vy = sheet->size + bias;
            scale.vz = sheet->size + bias;
        } else {
            m.t[0] = work->matrices[i].t[0] + work->directions[i].vx * level / 1024;
            m.t[1] = work->matrices[i].t[1] + work->directions[i].vy * level / 1024;
            m.t[2] = work->matrices[i].t[2] + work->directions[i].vz * level / 1024;
            scale.vx = 0x400;
            scale.vy = 0x400;
            scale.vz = 0x400;
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
        for (k = 0; k < 4; k++) {
            otz = RotTransPers4(&sheet->v0[k], &sheet->v1[k], &sheet->v2[k], &sheet->v3[k],
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
                if (i == 5) {
                    GsSortPoly(poly, ot, otz);
                } else if (object->progress[4] < 0x400) {
                    GsSortPoly(poly, ot, otz);
                }
            }
        }
        if (i == 5) {
            if (work->tail.phase == 2) {
                if (sheet->size < 0x1000) {
                    sheet->size += work->step << 9;
                    if (sheet->size >= 0x1000) {
                        sheet->size = 0x1000;
                    }
                }
            } else if (work->tail.phase == 3) {
                if (sheet->size > 0) {
                    sheet->size -= work->step << 5;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        work->tail.phase = 5;
                    }
                }
            }
        }
    }
}
