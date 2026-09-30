#include "../../types.h"

#include "model_variant.h"

/* Draws the six expanding rings of eight GsGLINE spokes each, fading the
 * spoke colour in over the first quarter turn, and advances each ring's
 * angle and cycle count. */
void func_8013E284(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    u8 *work;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    GsOT *ot;
    GsGLINE *line;
    ModelVariantRing *ring;
    s32 i;
    s32 k;
    SVECTOR *v;
    s32 s;
    s32 otz;
    s32 c;

    line = (GsGLINE *)(ctx + 0x1AE0);
    work = ctx;
    ring = (ModelVariantRing *)(ctx + 0x1374);
    ot = func_80058F10();
    for (i = 0; i < 6; i++, ring++) {
        if (ring->count > 0) {
            s = rsin(ring->angle);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            m.t[0] = MODEL_VARIANT_WORD(work, 0x1B08);
            m.t[1] = MODEL_VARIANT_WORD(work, 0x1B0C);
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1B10);
            s = 0x1000 - (s * 0x1000 >> 12);
            scale.vx = s;
            scale.vy = s;
            scale.vz = s;
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
            for (k = 0, v = ring->inner; k < 8; k++, v++) {
                line->attribute = 0x50000000;
                otz = RotTransPers4(v, &ring->outer[k], v,
                                    &ring->outer[k], (long *)&line->x0, (long *)&line->x1,
                                    (long *)&line->x0, (long *)&line->x1, &p, &flag);
                if (ring->angle < 0x100) {
                    line->r0 = ring->color[0] * ring->angle / 256;
                    line->g0 = ring->color[1] * ring->angle / 256;
                    line->b0 = ring->color[2] * ring->angle / 256;
                } else {
                    line->r0 = ring->color[0];
                    line->g0 = ring->color[1];
                    line->b0 = ring->color[2];
                }
                line->r1 = 0;
                line->g1 = 0;
                line->b1 = 0;
                if (otz > 0) {
                    if (otz < 0x800) {
                        GsSortGLine(line, ot, otz);
                    }
                }
            }
        }
        if (ring->angle < 0x400) {
            ring->angle += MODEL_VARIANT_WORD(work, 0x1B64) * 16;
            if (ring->angle >= 0x400) {
                if (MODEL_VARIANT_WORD(work, 0x1B9C) < 2) {
                    ring->angle = 0;
                    ring->count++;
                } else {
                    ring->angle = 0x400;
                }
                if (i == 0 && ring->angle == 0x400 && MODEL_VARIANT_WORD(work, 0x1B9C) == 2) {
                    MODEL_VARIANT_WORD(work, 0x1B9C) = 3;
                }
            }
        }
        if (i == 5 && ring->count >= 2 && MODEL_VARIANT_WORD(work, 0x1B9C) == 0) {
            MODEL_VARIANT_WORD(work, 0x1B9C) = 1;
        }
    }
}
