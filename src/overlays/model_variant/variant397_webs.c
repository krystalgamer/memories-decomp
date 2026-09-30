#include "../../types.h"

#include "model_variant.h"

/* Draws three webs of 4x6 GsGLINE segments between their near and far grids.
 * In phase 0 each web shrinks from 0x1000 and fades in, in phase 1 the webs
 * are staggered behind the origin, and from phase 2 they grow to 0x2000 and
 * fade out, wrapping until the fourth phase. */
void func_8013CE7C(u8 *ctx)
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
    timing = work + 0x5FC;
    web = (ModelVariantWebNarrow *)work;
    ot = func_80058F10();
    done = 1;
    i = 0;
    line = (GsGLINE *)(work + 0xE98);
    ratan2(MODEL_VARIANT_WORD(work, 0xF30), MODEL_VARIANT_WORD(work, 0xF28));
    ratan2(MODEL_VARIANT_WORD(work, 0xF2C), MODEL_VARIANT_WORD(work, 0xF28));
    ratan2(MODEL_VARIANT_WORD(work, 0xF1C), MODEL_VARIANT_WORD(work, 0xF18));
    ratan2(MODEL_VARIANT_HALF(work, 0xF26), MODEL_VARIANT_HALF(work, 0xF24));
    do {
        if (MODEL_VARIANT_WORD(work, 0xF78) == 0) {
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
        } else if (MODEL_VARIANT_WORD(work, 0xF78) == 1) {
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
        if (MODEL_VARIANT_WORD(work, 0xF78) < 2) {
            m.t[0] = MODEL_VARIANT_WORD(work, 0xEC0);
            m.t[1] = MODEL_VARIANT_WORD(work, 0xEC4);
            m.t[2] = MODEL_VARIANT_WORD(work, 0xEC8);
        } else {
            m.t[0] = MODEL_VARIANT_HALF(work, 0xF0C);
            m.t[1] = MODEL_VARIANT_HALF(work, 0xF0E);
            m.t[2] = MODEL_VARIANT_HALF(work, 0xF10);
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
                if (MODEL_VARIANT_WORD(work, 0xF78) < 2) {
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
        if (MODEL_VARIANT_WORD(work, 0xF78) == 0) {
            if (web->scale > 0) {
                web->scale -= MODEL_VARIANT_WORD(work, 0xF4C) * 0xC0;
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
        } else if (MODEL_VARIANT_WORD(work, 0xF78) == 1) {
            web->scale = -((i << 13) / 3);
        } else if (web->scale < 0x2000) {
            web->scale += MODEL_VARIANT_WORD(work, 0xF4C) << 8;
            if (web->scale >= 0x2000) {
                if (MODEL_VARIANT_WORD(work, 0xF78) >= 4) {
                    web->scale = 0x2000;
                    if (i + 1 == 3 && done == 1 && MODEL_VARIANT_WORD(work, 0xF78) == 4) {
                        MODEL_VARIANT_WORD(work, 0xF78) = 5;
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
