#include "../../types.h"

#include "model_variant.h"

/* Draws the six expanding rings of eight GsGLINE spokes each, fading the
 * spoke colour in over the first quarter turn, and advances each ring's
 * angle and cycle count. */
void func_8013D508(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    u8 *work;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    GsOT *ot;
    GsGLINE *line;
    ModelVariantRing *ring;
    s32 i;
    s32 k;
    SVECTOR *v;
    s32 s;
    s32 otz;
    s32 c;

    line = (GsGLINE *)(ctx + 0x1514);
    work = ctx;
    ring = (ModelVariantRing *)(ctx + 0xDA8);
    ot = func_80058F10();
    for (i = 0; i < 6; i++, ring++) {
        if (ring->count > 0) {
            s = rsin(ring->angle);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            m.t[0] = MODEL_VARIANT_WORD(work, 0x153C);
            m.t[1] = MODEL_VARIANT_WORD(work, 0x1540);
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1544);
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
                                    &ring->outer[k], (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                                    (PSXLONG *)&line->x0, (PSXLONG *)&line->x1, &p, &flag);
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
            ring->angle += MODEL_VARIANT_WORD(work, 0x1588) * 16;
            if (ring->angle >= 0x400) {
                if (MODEL_VARIANT_WORD(work, 0x15BC) < 2) {
                    ring->angle = 0;
                    ring->count++;
                } else {
                    ring->angle = 0x400;
                }
                if (i == 0 && ring->angle == 0x400 && MODEL_VARIANT_WORD(work, 0x15BC) == 2) {
                    MODEL_VARIANT_WORD(work, 0x15BC) = 3;
                }
            }
        }
        if (i == 5 && ring->count >= 2 && MODEL_VARIANT_WORD(work, 0x15BC) == 0) {
            MODEL_VARIANT_WORD(work, 0x15BC) = 1;
        }
    }
}
