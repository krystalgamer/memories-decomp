#include "../../types.h"

#include "model_variant.h"

/* Header 459's form of the header-418 webs: faded past scale 0x800, grown by
 * step * 128 to 0x1000, and sorted whenever the depth is positive. */
void func_8013D064(u8 *ctx)
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
    line = (GsGLINE *)(work + 0x237C);
    ratan2(MODEL_VARIANT_WORD(work, 0x279C), MODEL_VARIANT_WORD(work, 0x2794));
    ratan2(MODEL_VARIANT_WORD(work, 0x2798), MODEL_VARIANT_WORD(work, 0x2794));
    ratan2(MODEL_VARIANT_WORD(work, 0x2778), MODEL_VARIANT_WORD(work, 0x2774));
    ratan2(MODEL_VARIANT_HALF(work, 0x2792), MODEL_VARIANT_HALF(work, 0x2790));
    do {
        if (web->scale > 0) {
            level = web->scale;
            size = level;
        } else {
            size = 0;
            level = size;
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
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = MODEL_VARIANT_WORD(work, 0x274C);
        m.t[1] = MODEL_VARIANT_WORD(work, 0x2750);
        m.t[2] = MODEL_VARIANT_WORD(work, 0x2754);
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
                if (otz > 0) {
                    GsSortGLine(line, ot, otz);
                }
            }
        }
        if (web->scale < 0x1000) {
            web->scale += MODEL_VARIANT_WORD(work, 0x27B8) * 128;
            if (web->scale >= 0x1000) {
                if (MODEL_VARIANT_WORD(work, 0x284C) < 4) {
                    web->scale -= 0x1000;
                } else {
                    web->scale = 0x1000;
                }
            } else {
                done = 0;
            }
        }
        if (i + 1 == 3 && done == 1 && MODEL_VARIANT_WORD(work, 0x284C) == 4) {
            MODEL_VARIANT_WORD(work, 0x284C) = 5;
        }
        i++;
        web++;
    } while (i < 3);
}
