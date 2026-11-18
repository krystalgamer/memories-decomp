#include "../../types.h"

#include "h425_entry.h"

/* Draws the growing POLY_G4 quad of the variant's single quad record and
 * advances its size and rotation through three phases. */
void ModelVariant425_DrawQuad(Variant425EntryState *work)
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
    s32 bias;
    ModelVariantFan *rec;
    POLY_G4 *poly;
    s32 v;

    poly = &work->packets.quad;
    rec = work->fans;
    ot = Graphics_GetDefaultOrderingTable();
    for (i = 0; i < 1; i++, rec++) {
        bias = 0;
        if (work->clock.frame_count & 1) {
            bias = rec->size / 16;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = work->quad_angle;
        m.t[0] = work->matrix.t[0];
        m.t[1] = work->matrix.t[1];
        m.t[2] = work->matrix.t[2];
        scale.vx = rec->size + bias;
        scale.vy = rec->size + bias;
        scale.vz = rec->size + bias;
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
        v = RotTransPers4(&rec->corner[0], &rec->corner[1],
                          &rec->corner[2], &rec->corner[3],
                          (long *)&poly->x0, (long *)&poly->x1, (long *)&poly->x2,
                          (long *)&poly->x3, &p, &flag);
        poly->r0 = rec->outer.r;
        poly->g0 = rec->outer.g;
        poly->b0 = rec->outer.b;
        poly->r1 = rec->inner.r;
        poly->g1 = rec->inner.g;
        poly->b1 = rec->inner.b;
        poly->r2 = rec->outer.r;
        poly->g2 = rec->outer.g;
        poly->b2 = rec->outer.b;
        poly->r3 = rec->inner.r;
        poly->g3 = rec->inner.g;
        poly->b3 = rec->inner.b;
        if (v > 0) {
            Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)v, 1);
        }
        if (work->phase == 0) {
            if (rec->size < 0x1000) {
                rec->size = (work->clock.frame << 12) / work->config->burst_start;
                if (rec->size >= 0x1000) {
                    rec->size = 0x1000;
                    work->phase = 1;
                }
            }
        } else if (work->phase == 1) {
            if (work->quad_angle < 0x800) {
                work->quad_angle += work->clock.step << 6;
                if (work->quad_angle >= 0x800) {
                    work->quad_angle = 0x800;
                    work->phase = 2;
                }
            }
        } else {
            if (rec->size > 0) {
                rec->size -= work->clock.step << 5;
                if (rec->size <= 0) {
                    rec->size = 0;
                }
            }
        }
    }
}
