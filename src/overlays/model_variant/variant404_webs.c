#include "../../types.h"

#include "model_variant.h"

/* Header 404's copy of the header-397 phased webs, at its own work-area
 * offsets. */
void func_8013E18C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *timing;
    ModelVariantWebNarrow *web;
    GsOT *ot;
    GsGLINE *line;
    u8 *work;
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
    timing = work + 0xFD8;
    web = (ModelVariantWebNarrow *)work;
    ot = func_80058F10();
    done = 1;
    i = 0;
    line = (GsGLINE *)(work + 0x1874);
    ratan2(MODEL_VARIANT_WORD(work, 0x190C), MODEL_VARIANT_WORD(work, 0x1904));
    ratan2(MODEL_VARIANT_WORD(work, 0x1908), MODEL_VARIANT_WORD(work, 0x1904));
    ratan2(MODEL_VARIANT_WORD(work, 0x18F8), MODEL_VARIANT_WORD(work, 0x18F4));
    ratan2(MODEL_VARIANT_HALF(work, 0x1902), MODEL_VARIANT_HALF(work, 0x1900));
    do {
        if (MODEL_VARIANT_WORD(work, 0x1960) == 0) {
            if (web->scale < 0x1000) {
                level = web->scale;
                size = level;
            } else {
                size = 0x1000;
                level = 0;
            }
            if (size > 0x800) {
                fade = 0x1000 - size;
                r = web->color.r * fade / 2048;
                g = web->color.g * fade / 2048;
                b = web->color.b * fade / 2048;
            } else {
                r = web->color.r;
                g = web->color.g;
                b = web->color.b;
            }
        } else if (MODEL_VARIANT_WORD(work, 0x1960) == 1) {
            level = 0x1000;
            b = g = r = 0;
        } else {
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
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        if (MODEL_VARIANT_WORD(work, 0x1960) < 2) {
            m.t[0] = MODEL_VARIANT_WORD(work, 0x189C);
            m.t[1] = MODEL_VARIANT_WORD(work, 0x18A0);
            m.t[2] = MODEL_VARIANT_WORD(work, 0x18A4);
        } else {
            m.t[0] = MODEL_VARIANT_HALF(work, 0x18E8);
            m.t[1] = MODEL_VARIANT_HALF(work, 0x18EA);
            m.t[2] = MODEL_VARIANT_HALF(work, 0x18EC);
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
                if (MODEL_VARIANT_WORD(work, 0x1960) < 2) {
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
        if (MODEL_VARIANT_WORD(work, 0x1960) == 0) {
            if (web->scale > 0) {
                web->scale -= MODEL_VARIANT_WORD(work, 0x1928) * 0xC0;
                if (web->scale <= 0) {
                    if (MODEL_VARIANT_WORD(timing, 0x88) > 0x800) {
                        web->scale = 0;
                    } else {
                        web->scale += 0x1000;
                    }
                }
            }
            if (MODEL_VARIANT_WORD(timing, 0x88) > 0x800 && web->scale > 0x1000) {
                web->scale = 0;
            }
        } else if (MODEL_VARIANT_WORD(work, 0x1960) == 1) {
            web->scale = -((i << 13) / 3);
        } else if (web->scale < 0x2000) {
            web->scale += MODEL_VARIANT_WORD(work, 0x1928) << 8;
            if (web->scale >= 0x2000) {
                if (MODEL_VARIANT_WORD(work, 0x1960) >= 4) {
                    web->scale = 0x2000;
                    if (i + 1 == 3 && done == 1 && MODEL_VARIANT_WORD(work, 0x1960) == 4) {
                        MODEL_VARIANT_WORD(work, 0x1960) = 5;
                    }
                } else {
                    web->scale -= 0x2000;
                    done = 0;
                }
            }
        }
        i++;
        web++;
    } while (i < 3);
}
