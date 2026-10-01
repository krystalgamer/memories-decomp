#include "../../types.h"

#include "../model_variant/model_variant.h"

void func_8013BF1C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    GsOT *ot;
    GsGLINE *line;
    ModelVariantWebNarrow *web;
    s16 i;
    s16 j;
    s16 k;
    s32 size;
    s32 level;
    s32 fade;
    s32 done;
    u8 r;
    u8 g;
    u8 b;
    s32 otz;

    work = ctx;
    web = (ModelVariantWebNarrow *)work;
    ot = func_80058F10();
    done = 1;
    i = 0;
    line = (GsGLINE *)(work + 0x1730);
    ratan2(MODEL_VARIANT_WORD(work, 0x188C), MODEL_VARIANT_WORD(work, 0x1884));
    ratan2(MODEL_VARIANT_WORD(work, 0x1888), MODEL_VARIANT_WORD(work, 0x1884));
    ratan2(MODEL_VARIANT_WORD(work, 0x1774), MODEL_VARIANT_WORD(work, 0x1770));
    ratan2(MODEL_VARIANT_HALF(work, 0x177E), MODEL_VARIANT_HALF(work, 0x177C));
    do {
        if (web->scale > 0) {
            level = web->scale;
            size = level;
        } else {
            size = 0;
            level = size;
        }
        if (size > 0x1800) {
            fade = 0x2000 - size;
            r = web->color.r * fade / 2048;
            g = web->color.g * fade / 2048;
            b = web->color.b * fade / 2048;
        } else {
            r = web->color.r;
            g = web->color.g;
            b = web->color.b;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (MODEL_VARIANT_WORD(work, 0x18E0) < 2) {
            m.t[0] = MODEL_VARIANT_WORD(work, 0x1758);
            m.t[1] = MODEL_VARIANT_WORD(work, 0x175C);
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1760);
        } else {
            m.t[0] = MODEL_VARIANT_HALF(work, 0x1764);
            m.t[1] = MODEL_VARIANT_HALF(work, 0x1766);
            m.t[2] = MODEL_VARIANT_HALF(work, 0x1768);
        }
        scale.vx = level;
        scale.vy = level;
        scale.vz = level;
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 6; k++) {
                line->attribute = 0x50000000;
                otz = RotTransPers4(&web->near[j][k], &web->far[j][k], &web->near[j][k], &web->far[j][k],
                                    (PSXLONG *)&line->x0, (PSXLONG *)&line->x1, (PSXLONG *)&line->x0,
                                    (PSXLONG *)&line->x1, &p, &flag);
                if (MODEL_VARIANT_WORD(work, 0x18E0) < 2) {
                    line->r0 = 0;
                    line->g0 = 0;
                    line->b0 = 0;
                    line->r1 = r;
                    line->g1 = g;
                    line->b1 = b;
                } else {
                    line->r0 = r;
                    line->g0 = g;
                    line->b0 = b;
                    line->r1 = 0;
                    line->g1 = 0;
                    line->b1 = 0;
                }
                if (otz >= 0 && flag >= 0) {
                    GsSortGLine(line, ot, otz);
                }
            }
        }
        if (web->scale < 0x2000) {
            web->scale += MODEL_VARIANT_WORD(work, 0x18A8) << 8;
            if (web->scale >= 0x2000) {
                if (MODEL_VARIANT_WORD(work, 0x18E0) == 5) {
                    web->scale = 0x2000;
                    web->done = 1;
                    if (i + 1 == 3 && done == 1) {
                        MODEL_VARIANT_WORD(work, 0x18E0) = 6;
                    }
                } else {
                    web->scale -= 0x2000;
                }
            }
        }
        i++;
        web++;
    } while (i < 3);
}
