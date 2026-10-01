#include "../../types.h"

#include "model_variant.h"

void func_8013C814(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    u8 *work;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
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

    work = ctx;
    object = ctx + 0xF60;
    sheet = (ModelVariantSheet *)(ctx + 0x10B0);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(ctx + 0x191C);
    for (i = 0; i < 3; i++, sheet++, object += 0xA8) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x1A4C) & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i == 2) {
            m.t[0] = MODEL_VARIANT_HALF(work, 0x1A18);
            m.t[1] = MODEL_VARIANT_HALF(work, 0x1A1A);
            m.t[2] = MODEL_VARIANT_HALF(work, 0x1A1C);
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
            m.t[0] = MODEL_VARIANT_WORD(work, 0x1A0C) + MODEL_VARIANT_WORD(work, 0x1A20) * level / 1024;
            m.t[1] = MODEL_VARIANT_WORD(work, 0x1A10) + MODEL_VARIANT_WORD(work, 0x1A24) * level / 1024;
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1A14) + MODEL_VARIANT_WORD(work, 0x1A28) * level / 1024;
            if (MODEL_VARIANT_WORD(work, 0x1A84) < 3) {
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
                                (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
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
            if (MODEL_VARIANT_WORD(work, 0x1A84) == 2) {
                if (sheet->size < 0x8000) {
                    sheet->size += MODEL_VARIANT_WORD(work, 0x1A58) << 12;
                    if (sheet->size >= 0x8000) {
                        sheet->size = 0x8000;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x1A84) == 3) {
                if (sheet->size > 0) {
                    sheet->size -= MODEL_VARIANT_WORD(work, 0x1A58) << 8;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        MODEL_VARIANT_WORD(work, 0x1A84) = 4;
                    }
                }
            }
        }
    }
}
