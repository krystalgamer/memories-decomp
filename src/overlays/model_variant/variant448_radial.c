#include "../../types.h"
#include "variant448_radial.h"

/* Draws sixteen triangles and sixteen outer quads with three color bands,
 * then advances the timed growth and step-based shrink. */
void func_8013C254(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    GsOT *ot;
    s32 i;
    u8 *work;
    Variant448Radial *rec;
    POLY_G3 *tri;
    POLY_G4 *quad;
    s32 j;
    s32 otz;

    work = ctx;
    rec = (Variant448Radial *)ctx;
    tri = (POLY_G3 *)(ctx + 0x1948);
    ot = func_80058F10();
    quad = (POLY_G4 *)(ctx + 0x1964);
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x1AB8);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x1ABC);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x1AC0);
    scale.vx = rec->size;
    scale.vy = rec->size;
    scale.vz = rec->size;
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    for (i = 0; i < 1; i++, rec++) {
        for (j = 0; j < 16; j++) {
            otz = RotTransPers3(&rec->point[0], &rec->point[j + 2], &rec->point[j + 1],
                                (PSXLONG *)&tri->x0, (PSXLONG *)&tri->x1,
                                (PSXLONG *)&tri->x2, &p, &flag);
            tri->r0 = rec->center.r;
            tri->g0 = rec->center.g;
            tri->b0 = rec->center.b;
            tri->r1 = rec->middle.r;
            tri->g1 = rec->middle.g;
            tri->b1 = rec->middle.b;
            tri->r2 = rec->middle.r;
            tri->g2 = rec->middle.g;
            tri->b2 = rec->middle.b;
            if (otz >= 0 && flag >= 0) {
                func_8005B260((u32 *)tri, ot, (u16)otz, 1);
            }
            otz = RotTransPers4(&rec->point[j + 1], &rec->point[j + 2],
                                &rec->point[j + 18], &rec->point[j + 19],
                                (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3, &p, &flag);
            quad->r0 = rec->middle.r;
            quad->g0 = rec->middle.g;
            quad->b0 = rec->middle.b;
            quad->r1 = rec->middle.r;
            quad->g1 = rec->middle.g;
            quad->b1 = rec->middle.b;
            quad->r2 = rec->edge.r;
            quad->g2 = rec->edge.g;
            quad->b2 = rec->edge.b;
            quad->r3 = rec->edge.r;
            quad->g3 = rec->edge.g;
            quad->b3 = rec->edge.b;
            if (otz >= 0 && flag >= 0) {
                func_8005B260((u32 *)quad, ot, (u16)otz, 1);
            }
        }
        if (MODEL_VARIANT_WORD(work, 0x1B3C) == 0) {
            if (rec->size < 0x1000) {
                rec->size = (u32)((MODEL_VARIANT_WORD(work, 0x1B00) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B10), 0x1C)) << 12) /
                    (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B10), 0x20) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B10), 0x1C));
                if (rec->size >= 0x1000) {
                    rec->size = 0x1000;
                    if (i + 1 == 1) {
                        MODEL_VARIANT_WORD(work, 0x1B3C) = i + 1;
                    }
                }
            }
        } else if ((u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1B10), 0x24) <= (u32)MODEL_VARIANT_WORD(work, 0x1B00)) {
            if (rec->size > 0) {
                rec->size -= MODEL_VARIANT_WORD(work, 0x1B08) << 7;
                if (rec->size <= 0) {
                    rec->size = 0;
                }
            } else if (MODEL_VARIANT_WORD(work, 0x1B3C) == 2) {
                MODEL_VARIANT_WORD(work, 0x1B3C) = 3;
            }
        }
    }
}
