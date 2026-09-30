#include "../../types.h"

#include "model_variant.h"

/* Draws three webs of 6x6 GsGLINE segments between their near and far grids,
 * fading each web's colour once its scale passes 0x1000, and grows each scale
 * to 0x2000, wrapping it until the fourth phase. */
void func_8013CE50(u8 *ctx)
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
    ModelVariantWeb *web;
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
    web = (ModelVariantWeb *)work;
    ot = func_80058F10();
    done = 1;
    i = 0;
    line = (GsGLINE *)(work + 0x1AE0);
    ratan2(MODEL_VARIANT_WORD(work, 0x1B48), MODEL_VARIANT_WORD(work, 0x1B40));
    ratan2(MODEL_VARIANT_WORD(work, 0x1B44), MODEL_VARIANT_WORD(work, 0x1B40));
    ratan2(MODEL_VARIANT_WORD(work, 0x1B34), MODEL_VARIANT_WORD(work, 0x1B30));
    ratan2(MODEL_VARIANT_HALF(work, 0x1B3E), MODEL_VARIANT_HALF(work, 0x1B3C));
    do {
        if (web->scale > 0) {
            level = web->scale;
            size = level;
        } else {
            size = 0;
            level = size;
        }
        if (size > 0x1000) {
            fade = 0x2000 - size;
            r = web->color.r * fade / 4096;
            g = web->color.g * fade / 4096;
            b = web->color.b * fade / 4096;
        } else {
            r = web->color.r;
            g = web->color.g;
            b = web->color.b;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work, 0x1B08);
        m.t[1] = MODEL_VARIANT_WORD(work, 0x1B0C);
        m.t[2] = MODEL_VARIANT_WORD(work, 0x1B10);
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
        for (j = 0; j < 6; j++) {
            for (k = 0; k < 6; k++) {
                line->attribute = 0x50000000;
                otz = RotTransPers4(&web->near[j][k], &web->far[j][k], &web->near[j][k], &web->far[j][k],
                                    (PSXLONG *)&line->x0, (PSXLONG *)&line->x1, (PSXLONG *)&line->x0,
                                    (PSXLONG *)&line->x1, &p, &flag);
                line->r0 = r;
                line->g0 = g;
                line->b0 = b;
                line->r1 = 0;
                line->g1 = 0;
                line->b1 = 0;
                if (otz >= 0 && flag >= 0) {
                    GsSortGLine(line, ot, otz);
                }
            }
        }
        if (web->scale < 0x2000) {
            web->scale += MODEL_VARIANT_WORD(work, 0x1B64) * 0xC0;
            if (web->scale >= 0x2000) {
                if (MODEL_VARIANT_WORD(work, 0x1B9C) < 4) {
                    web->scale -= 0x2000;
                } else {
                    web->scale = 0x2000;
                }
            } else {
                done = 0;
            }
        }
        if (i + 1 == 3 && done == 1 && MODEL_VARIANT_WORD(work, 0x1B9C) == 4) {
            MODEL_VARIANT_WORD(work, 0x1B9C) = 5;
        }
        i++;
        web++;
    } while (i < 3);
}
