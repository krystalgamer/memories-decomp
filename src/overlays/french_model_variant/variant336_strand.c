#include "../../types.h"
#include "../model_variant/model_variant.h"

void func_8013C860(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    GsLINE *line;
    ModelVariantStrandWide *strand;
    s32 i;
    s32 j;
    s32 a;
    s32 b;
    s32 r;

    work = ctx;
    strand = (ModelVariantStrandWide *)(work + 0x1AB0);
    line = (GsLINE *)(work + 0x1F10);
    if (MODEL_VARIANT_WORD(work, 0x2000) >= 2) {
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_HALF(work, 0x1F90);
        m.t[1] = MODEL_VARIANT_HALF(work, 0x1F92);
        m.t[2] = MODEL_VARIANT_HALF(work, 0x1F94);
        scale.vx = 0x1000;
        scale.vy = 0x1000;
        scale.vz = 0x1000;
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        i = 0;
        a = i;
        do {
            for (j = 0, b = 0; j < 13; j++, b += 0x708) {
                r = (j << 10) / 12;
                strand->point[j].vx = rcos(a) * r >> 12;
                strand->point[j].vy = (u32)rsin(b) >> 9;
                strand->point[j].vz = rsin(a) * r >> 12;
            }
            i++;
            a = (i << 12) / 6;
            strand++;
        } while (i < 6);
        strand = (ModelVariantStrandWide *)(work + 0x1AB0);
        for (i = 0; i < 6; i++, strand++) {
            for (j = MODEL_VARIANT_HALF(work, 0x1FF0);
                 j < MODEL_VARIANT_HALF(work, 0x1FF2); j++) {
                line->attribute = 0x50000000;
                RotTransPers4(&strand->point[j], &strand->point[j + 1],
                              &strand->point[j], &strand->point[j + 1],
                              (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                              (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                              &p, &flag);
                line->g = 0x40;
                line->r = 0;
                line->b = 0x80;
            }
        }
        if (MODEL_VARIANT_HALF(work, 0x1FF2) < 13) {
            MODEL_VARIANT_HALF(work, 0x1FF2) += (u32)MODEL_VARIANT_WORD(work, 0x1FD0) >> 1;
            if (MODEL_VARIANT_HALF(work, 0x1FF2) >= 12) {
                MODEL_VARIANT_HALF(work, 0x1FF2) = 12;
                if (MODEL_VARIANT_HALF(work, 0x1FF0) < 13) {
                    MODEL_VARIANT_HALF(work, 0x1FF0) += (u32)MODEL_VARIANT_WORD(work, 0x1FD0) >> 1;
                    if (MODEL_VARIANT_HALF(work, 0x1FF0) >= 12) {
                        MODEL_VARIANT_HALF(work, 0x1FF2) = 0;
                        MODEL_VARIANT_HALF(work, 0x1FF0) = 0;
                    }
                }
            }
        }
    }
}
