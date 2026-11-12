#include "../../types.h"

#include "model_variant.h"
#include "variant416_entry.h"

void ModelVariant416_DrawSheets(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    Variant416EntryState *work;
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
    s32 level;
    u8 *object;
    s32 otz;

    work = (Variant416EntryState *)ctx;
    object = ctx + 0xF60;
    sheet = (ModelVariantSheet *)(ctx + 0x10B0);
    ot = Graphics_GetDefaultOrderingTable();
    poly = (POLY_GT4 *)(ctx + 0x191C);
    for (i = 0; i < 3; i++, sheet++, object += 0xA8) {
        bias = 0;
        if (work->clock.frame_count & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i == 2) {
            m.t[0] = work->aim.target.vx;
            m.t[1] = work->aim.target.vy;
            m.t[2] = work->aim.target.vz;
            scale.vx = sheet->size + bias;
            scale.vy = sheet->size + bias;
            scale.vz = sheet->size + bias;
        } else {
            level = 0;
            if (MODEL_VARIANT_WORD(object, 0x68) > 0) {
                level = 0x400;
                if (MODEL_VARIANT_WORD(object, 0x68) < 0x400) {
                    level = MODEL_VARIANT_WORD(object, 0x68);
                }
            }
            m.t[0] = work->matrix.t[0] + work->aim.direction.vx * level / 1024;
            m.t[1] = work->matrix.t[1] + work->aim.direction.vy * level / 1024;
            m.t[2] = work->matrix.t[2] + work->aim.direction.vz * level / 1024;
            if (work->tail.phase < 3) {
                scale.vx = 0x800;
                scale.vy = 0x800;
                scale.vz = 0x800;
            } else {
                scale.vx = 0;
                scale.vy = 0;
                scale.vz = 0;
            }
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
        if (i == 2) {
            if (work->tail.phase == 2) {
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
