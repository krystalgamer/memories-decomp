#include "../../types.h"

#include "../model_variant/model_variant.h"

void func_8013C760(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    u8 *work;
    u8 *record;
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
    s32 otz;

    work = ctx;
    record = ctx + 0xF60;
    sheet = (ModelVariantSheet *)(ctx + 0x11DC);
    ot = func_80058F10();
    poly = (POLY_GT4 *)(ctx + 0x19B0);
    for (i = 0; i < 2; i++, sheet++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0x1AE0) & 1) {
            bias = sheet->size / 8;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (i == 0) {
            s32 amount;

            amount = 0;
            if (MODEL_VARIANT_WORD(record, 0x1F4) > 0) {
                amount = MODEL_VARIANT_WORD(record, 0x1F4);
            }
            m.t[0] = MODEL_VARIANT_WORD(work, 0x1AA0) + MODEL_VARIANT_WORD(work, 0x1AB4) * amount / 0x400;
            m.t[1] = MODEL_VARIANT_WORD(work, 0x1AA4) + MODEL_VARIANT_WORD(work, 0x1AB8) * amount / 0x400;
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1AA8) + MODEL_VARIANT_WORD(work, 0x1ABC) * amount / 0x400;
            scale.vx = sheet->size;
            scale.vy = sheet->size;
            scale.vz = sheet->size;
        } else {
            s32 amount;

            amount = 0;
            if (MODEL_VARIANT_WORD(record, 0x1D4) > 0) {
                amount = MODEL_VARIANT_WORD(record, 0x1D4);
            }
            m.t[0] = MODEL_VARIANT_WORD(work, 0x1AA0) + MODEL_VARIANT_WORD(work, 0x1AB4) * amount / 0x400;
            m.t[1] = MODEL_VARIANT_WORD(work, 0x1AA4) + MODEL_VARIANT_WORD(work, 0x1AB8) * amount / 0x400;
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1AA8) + MODEL_VARIANT_WORD(work, 0x1ABC) * amount / 0x400;
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
        if (i == 0) {
            sheet->size = 0x800;
            if (MODEL_VARIANT_WORD(work, 0x1B18) >= 3) {
                sheet->size = 0;
            }
        } else {
            if (MODEL_VARIANT_WORD(work, 0x1B18) == 0) {
                if (sheet->size < 0x1000) {
                    u32 *timing;

                    timing = *(u32 *G32 *)(work + 0x1AF4);
                    sheet->size = ((MODEL_VARIANT_WORD(work, 0x1AE4) - timing[7]) << 12) /
                                  (timing[8] - timing[7]);
                    if (sheet->size >= 0x1000) {
                        sheet->size = 0x1000;
                        MODEL_VARIANT_WORD(work, 0x1B18) = 1;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x1B18) < 2) {
                sheet->size = 0x1000;
            } else if (MODEL_VARIANT_WORD(work, 0x1B18) == 2) {
                if (sheet->size < 0x8000) {
                    sheet->size += MODEL_VARIANT_WORD(work, 0x1AEC) << 12;
                    if (sheet->size >= 0x8000) {
                        sheet->size = 0x8000;
                    }
                }
            } else if (MODEL_VARIANT_WORD(work, 0x1B18) == 3) {
                if (sheet->size > 0) {
                    sheet->size -= MODEL_VARIANT_WORD(work, 0x1AEC) << 8;
                    if (sheet->size <= 0) {
                        sheet->size = 0;
                        MODEL_VARIANT_WORD(work, 0x1B18) = 4;
                    }
                }
            }
        }
    }
}
