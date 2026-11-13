#include "../../types.h"

#include "model_variant.h"
#include "../french_model_variant/variant422_entry.h"

/* Draws the six expanding rings of eight GsGLINE spokes each, fading the
 * spoke colour in over the first quarter turn, and advances each ring's
 * angle and cycle count. */
void ModelVariant405_DrawRings(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    Variant422EntryState *work;
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

    line = (GsGLINE *)(ctx + 0x1D4C);
    work = (Variant422EntryState *)ctx;
    ring = (ModelVariantRing *)(ctx + 0x15AC);
    ot = Graphics_GetDefaultOrderingTable();
    for (i = 0; i < 6; i++, ring++) {
        if (ring->count > 0) {
            s = rsin(ring->angle);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            m.t[0] = work->matrix.t[0];
            m.t[1] = work->matrix.t[1];
            m.t[2] = work->matrix.t[2];
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
                    line->r0 = ring->inner_color[0] * ring->angle / 256;
                    line->g0 = ring->inner_color[1] * ring->angle / 256;
                    line->b0 = ring->inner_color[2] * ring->angle / 256;
                } else {
                    line->r0 = ring->inner_color[0];
                    line->g0 = ring->inner_color[1];
                    line->b0 = ring->inner_color[2];
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
            ring->angle += work->clock.step * 16;
            if (ring->angle >= 0x400) {
                if (work->phase < 2) {
                    ring->angle = 0;
                    ring->count++;
                } else {
                    ring->angle = 0x400;
                }
                if (i == 0 && ring->angle == 0x400 && work->phase == 2) {
                    work->phase = 3;
                }
            }
        }
        if (i == 5 && ring->count >= 2 && work->phase == 0) {
            work->phase = 1;
        }
    }
}
