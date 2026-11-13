#include "../../types.h"

#include "model_variant.h"
#include "variant401_entry.h"

/* Draws four rings of eight GsGLINE spokes, each scaled by its angle against
 * the sheet size at (u8 *)work + 0x684 and faded out over the second half turn,
 * and advances each ring's angle and cycle count. */
void ModelVariant401_DrawSpokes(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    Variant401EntryState *work;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    GsGLINE *line;
    ModelVariantRing *ring;
    ModelVariantSheet *obj;
    GsOT *ot;
    s32 i;
    s32 k;
    SVECTOR *v;
    s32 s;
    s32 otz;
    s32 f;

    line = (GsGLINE *)(ctx + 0x1A78);
    work = (Variant401EntryState *)ctx;
    obj = work->sheets;
    ring = (ModelVariantRing *)(ctx + 0x166C);
    ot = Graphics_GetDefaultOrderingTable();
    for (i = 0; i < 4; i++, ring++) {
        if (ring->count >= 0) {
            s = obj->size * ring->angle / 1024;
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            m.t[0] = work->aim.target.vx;
            m.t[1] = work->aim.target.vy;
            m.t[2] = work->aim.target.vz;
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
                                    (long *)&line->x0, (long *)&line->x1,
                                    (long *)&line->x0, (long *)&line->x1, &p, &flag);
                line->r0 = 0;
                line->g0 = 0;
                line->b0 = 0;
                if (ring->angle >= 0x200) {
                    f = 0x400 - ring->angle;
                    line->r1 = ring->outer_color[0] * f / 512;
                    line->g1 = ring->outer_color[1] * f / 512;
                    line->b1 = ring->outer_color[2] * f / 512;
                } else {
                    line->r1 = ring->outer_color[0];
                    line->g1 = ring->outer_color[1];
                    line->b1 = ring->outer_color[2];
                }
                if (otz > 0) {
                    if (otz < 0x800) {
                        GsSortGLine(line, ot, otz);
                    }
                }
            }
        }
        if (ring->angle < 0x401) {
            ring->angle += work->clock.step * 0x30;
            if (ring->angle >= 0x400) {
                ring->angle = 0;
                ring->count++;
            }
        }
    }
}
