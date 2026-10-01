#include "../../types.h"

#include "model_variant.h"

#ifdef VERSION_FRENCH
#define MODEL_VARIANT416_TAIL_OFFSET 0x94
#else
#define MODEL_VARIANT416_TAIL_OFFSET 0
#endif

/* Header 416's form of the header-397 phased webs: the records start at
 * work + 0xA80, and in phase 0 each web is placed a third of a turn behind
 * the previous one along the progress of the timing record at work + 0x1A60.
 * VERSION_FRENCH selects French418's shifted tail; French433 uses the default. */
void func_8013CC68(u8 *ctx)
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
    web = (ModelVariantWebNarrow *)(work + 0xA80);
    ot = func_80058F10();
    done = 1;
    i = 0;
    line = (GsGLINE *)(work + (0x19E4 + MODEL_VARIANT416_TAIL_OFFSET));
    ratan2(MODEL_VARIANT_WORD(work, (0x1A3C + MODEL_VARIANT416_TAIL_OFFSET)), MODEL_VARIANT_WORD(work, (0x1A34 + MODEL_VARIANT416_TAIL_OFFSET)));
    ratan2(MODEL_VARIANT_WORD(work, (0x1A38 + MODEL_VARIANT416_TAIL_OFFSET)), MODEL_VARIANT_WORD(work, (0x1A34 + MODEL_VARIANT416_TAIL_OFFSET)));
    ratan2(MODEL_VARIANT_WORD(work, (0x1A28 + MODEL_VARIANT416_TAIL_OFFSET)), MODEL_VARIANT_WORD(work, (0x1A24 + MODEL_VARIANT416_TAIL_OFFSET)));
    ratan2(MODEL_VARIANT_HALF(work, (0x1A32 + MODEL_VARIANT416_TAIL_OFFSET)), MODEL_VARIANT_HALF(work, (0x1A30 + MODEL_VARIANT416_TAIL_OFFSET)));
    do {
        if (MODEL_VARIANT_WORD(work, (0x1A84 + MODEL_VARIANT416_TAIL_OFFSET)) == 0) {
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
        } else if (MODEL_VARIANT_WORD(work, (0x1A84 + MODEL_VARIANT416_TAIL_OFFSET)) == 1) {
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
        if (MODEL_VARIANT_WORD(work, (0x1A84 + MODEL_VARIANT416_TAIL_OFFSET)) < 2) {
            m.t[0] = MODEL_VARIANT_WORD(work, (0x1A0C + MODEL_VARIANT416_TAIL_OFFSET));
            m.t[1] = MODEL_VARIANT_WORD(work, (0x1A10 + MODEL_VARIANT416_TAIL_OFFSET));
            m.t[2] = MODEL_VARIANT_WORD(work, (0x1A14 + MODEL_VARIANT416_TAIL_OFFSET));
        } else {
            m.t[0] = MODEL_VARIANT_HALF(work, (0x1A18 + MODEL_VARIANT416_TAIL_OFFSET));
            m.t[1] = MODEL_VARIANT_HALF(work, (0x1A1A + MODEL_VARIANT416_TAIL_OFFSET));
            m.t[2] = MODEL_VARIANT_HALF(work, (0x1A1C + MODEL_VARIANT416_TAIL_OFFSET));
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
                if (MODEL_VARIANT_WORD(work, (0x1A84 + MODEL_VARIANT416_TAIL_OFFSET)) < 2) {
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
        if (MODEL_VARIANT_WORD(work, (0x1A84 + MODEL_VARIANT416_TAIL_OFFSET)) == 0) {
            if (web->scale > 0) {
                timing = (u8 *)MODEL_VARIANT_WORD(work, (0x1A60 + MODEL_VARIANT416_TAIL_OFFSET));
                web->scale = (u32)((MODEL_VARIANT_WORD(work, (0x1A50 + MODEL_VARIANT416_TAIL_OFFSET)) - MODEL_VARIANT_WORD(timing, 0x1C)) * 3 << 12) /
                                 (MODEL_VARIANT_WORD(timing, 0x20) - MODEL_VARIANT_WORD(timing, 0x1C)) -
                             0x1000;
                web->scale = (i << 12) / 3 - web->scale;
                if (web->scale <= 0) {
                    web->scale += 0x1000;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, (0x1A84 + MODEL_VARIANT416_TAIL_OFFSET)) == 1) {
            web->scale = -((i << 13) / 3);
        } else if (web->scale < 0x2000) {
            web->scale += MODEL_VARIANT_WORD(work, (0x1A58 + MODEL_VARIANT416_TAIL_OFFSET)) << 8;
            if (web->scale >= 0x2000) {
                if (MODEL_VARIANT_WORD(work, (0x1A84 + MODEL_VARIANT416_TAIL_OFFSET)) >= 4) {
                    web->scale = 0x2000;
                    if (i + 1 == 3 && done == 1 && MODEL_VARIANT_WORD(work, (0x1A84 + MODEL_VARIANT416_TAIL_OFFSET)) == 4) {
                        MODEL_VARIANT_WORD(work, (0x1A84 + MODEL_VARIANT416_TAIL_OFFSET)) = 5;
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
