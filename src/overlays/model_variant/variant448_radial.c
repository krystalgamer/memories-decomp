#include "../../types.h"
#include "variant448_radial.h"
#include "variant448_entry.h"

/* Draws sixteen triangles and sixteen outer quads with three color bands,
 * then advances the timed growth and step-based shrink. */
void ModelVariant448_DrawRadialDisc(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    GsOT *ot;
    s32 i;
    Variant448EntryState *work;
    Variant448Radial *rec;
    POLY_G3 *tri;
    POLY_G4 *quad;
    s32 j;
    s32 otz;

    work = (Variant448EntryState *)ctx;
    rec = (Variant448Radial *)ctx;
    tri = (POLY_G3 *)(ctx + 0x1948);
    ot = Graphics_GetDefaultOrderingTable();
    quad = (POLY_G4 *)(ctx + 0x1964);
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = work->position.vx;
    m.t[1] = work->position.vy;
    m.t[2] = work->position.vz;
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
                                (long *)&tri->x0, (long *)&tri->x1,
                                (long *)&tri->x2, &p, &flag);
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
                Graphics_SubmitDrawModePacket((u32 *)tri, ot, (u16)otz, 1);
            }
            otz = RotTransPers4(&rec->point[j + 1], &rec->point[j + 2],
                                &rec->point[j + 18], &rec->point[j + 19],
                                (long *)&quad->x0, (long *)&quad->x1,
                                (long *)&quad->x2, (long *)&quad->x3, &p, &flag);
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
                Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)otz, 1);
            }
        }
        if (work->phase == 0) {
            if (rec->size < 0x1000) {
                rec->size = ((work->clock.frame - work->config->start) << 12) /
                    (work->config->field_20 - work->config->start);
                if (rec->size >= 0x1000) {
                    rec->size = 0x1000;
                    if (i + 1 == 1) {
                        work->phase = i + 1;
                    }
                }
            }
        } else if ((u32)work->config->field_24 <= work->clock.frame) {
            if (rec->size > 0) {
                rec->size -= work->clock.step << 7;
                if (rec->size <= 0) {
                    rec->size = 0;
                }
            } else if (work->phase == 2) {
                work->phase = 3;
            }
        }
    }
}
