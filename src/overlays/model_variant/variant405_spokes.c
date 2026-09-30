#include "../../types.h"

#include "model_variant.h"

/* Draws four rings of eight GsGLINE spokes, each scaled by its angle against
 * the sheet size at work + 0x684 and faded out over the second half turn,
 * and advances each ring's angle and cycle count. */
void func_8013DE54(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    u8 *work;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    GsGLINE *line;
    ModelVariantSpokeRing *ring;
    u8 *obj;
    GsOT *ot;
    s32 i;
    s32 k;
    SVECTOR *v;
    s32 s;
    s32 otz;
    s32 f;

    line = (GsGLINE *)(ctx + 0x1D4C);
    work = ctx;
    obj = ctx + 0x147C;
    ring = (ModelVariantSpokeRing *)(ctx + 0x190C);
    ot = func_80058F10();
    for (i = 0; i < 4; i++, ring++) {
        if (ring->count >= 0) {
            s = MODEL_VARIANT_WORD(obj, 0x88) * ring->angle / 1024;
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            m.t[0] = MODEL_VARIANT_HALF(work, 0x1D80);
            m.t[1] = MODEL_VARIANT_HALF(work, 0x1D82);
            m.t[2] = MODEL_VARIANT_HALF(work, 0x1D84);
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
                otz = RotTransPers4(v, &ring->outer[k], v, &ring->outer[k],
                                    (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                                    (PSXLONG *)&line->x0, (PSXLONG *)&line->x1, &p, &flag);
                line->r0 = 0;
                line->g0 = 0;
                line->b0 = 0;
                if (ring->angle >= 0x200) {
                    f = 0x400 - ring->angle;
                    line->r1 = ring->color[0] * f / 512;
                    line->g1 = ring->color[1] * f / 512;
                    line->b1 = ring->color[2] * f / 512;
                } else {
                    line->r1 = ring->color[0];
                    line->g1 = ring->color[1];
                    line->b1 = ring->color[2];
                }
                if (otz > 0) {
                    if (otz < 0x800) {
                        GsSortGLine(line, ot, otz);
                    }
                }
            }
        }
        if (ring->angle < 0x401) {
            ring->angle += MODEL_VARIANT_WORD(work, 0x1DC0) * 0x30;
            if (ring->angle >= 0x400) {
                ring->angle = 0;
                ring->count++;
            }
        }
    }
}
