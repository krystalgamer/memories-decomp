#include "../../types.h"

#include "model_variant.h"

/* Header 423's form of the header-416 webs: the records start at work, and
 * the lines are projected with RotTransPers3. */
void func_8013CBDC(u8 *ctx)
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
    web = (ModelVariantWebNarrow *)work;
    ot = func_80058F10();
    done = 1;
    i = 0;
    line = (GsGLINE *)(work + 0xE84);
    ratan2(MODEL_VARIANT_WORD(work, 0xEDC), MODEL_VARIANT_WORD(work, 0xED4));
    ratan2(MODEL_VARIANT_WORD(work, 0xED8), MODEL_VARIANT_WORD(work, 0xED4));
    ratan2(MODEL_VARIANT_WORD(work, 0xEC8), MODEL_VARIANT_WORD(work, 0xEC4));
    ratan2(MODEL_VARIANT_HALF(work, 0xED2), MODEL_VARIANT_HALF(work, 0xED0));
    do {
        if (MODEL_VARIANT_WORD(work, 0xF24) == 0) {
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
        } else if (MODEL_VARIANT_WORD(work, 0xF24) == 1) {
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
        if (MODEL_VARIANT_WORD(work, 0xF24) < 2) {
            m.t[0] = MODEL_VARIANT_WORD(work, 0xEAC);
            m.t[1] = MODEL_VARIANT_WORD(work, 0xEB0);
            m.t[2] = MODEL_VARIANT_WORD(work, 0xEB4);
        } else {
            m.t[0] = MODEL_VARIANT_HALF(work, 0xEB8);
            m.t[1] = MODEL_VARIANT_HALF(work, 0xEBA);
            m.t[2] = MODEL_VARIANT_HALF(work, 0xEBC);
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
                otz = RotTransPers3(&web->near[j][k], &web->far[j][k], &web->near[j][k], (PSXLONG *)&line->x0,
                                    (PSXLONG *)&line->x1, (PSXLONG *)&line->x0, &p, &flag);
                if (MODEL_VARIANT_WORD(work, 0xF24) < 2) {
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
        if (MODEL_VARIANT_WORD(work, 0xF24) == 0) {
            if (web->scale > 0) {
                timing = (u8 *G32)MODEL_VARIANT_WORD(work, 0xF00);
                web->scale = (u32)((MODEL_VARIANT_WORD(work, 0xEF0) - MODEL_VARIANT_WORD(timing, 0x1C)) * 3 << 12) /
                                 (MODEL_VARIANT_WORD(timing, 0x20) - MODEL_VARIANT_WORD(timing, 0x1C)) -
                             0x1000;
                web->scale = (i << 12) / 3 - web->scale;
                if (web->scale <= 0) {
                    web->scale += 0x1000;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0xF24) == 1) {
            web->scale = -((i << 13) / 3);
        } else if (web->scale < 0x2000) {
            web->scale += MODEL_VARIANT_WORD(work, 0xEF8) * 0x180;
            if (web->scale >= 0x2000) {
                if (MODEL_VARIANT_WORD(work, 0xF24) >= 4) {
                    web->scale = 0x2000;
                    if (i + 1 == 3 && done == 1 && MODEL_VARIANT_WORD(work, 0xF24) == 4) {
                        MODEL_VARIANT_WORD(work, 0xF24) = 5;
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
