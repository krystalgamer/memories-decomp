#include "../../types.h"

#include "model_variant.h"
#include "variant428_entry.h"

/* Draws the growing POLY_G4 quad of the variant's single quad record and
 * advances its size and rotation through three phases. */
void ModelVariant428_DrawQuad(Variant428EntryState *work)
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
    ModelVariantQuad *rec;
    POLY_G4 *poly;
    s32 v;

    poly = &work->packets.quad;
    rec = (ModelVariantQuad *)work->fans;
    ot = Graphics_GetDefaultOrderingTable();
    for (i = 0; i < 1; i++, rec++) {
        bias = 0;
        if (work->clock.frame_count & 1) {
            bias = rec->size / 16;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = *(u16 *)((u8 *)work + 0x15B8);
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
        poly->r0 = rec->outer[0];
        poly->g0 = rec->outer[1];
        poly->b0 = rec->outer[2];
        poly->r1 = rec->inner[0];
        poly->g1 = rec->inner[1];
        poly->b1 = rec->inner[2];
        poly->r2 = rec->outer[0];
        poly->g2 = rec->outer[1];
        poly->b2 = rec->outer[2];
        poly->r3 = rec->inner[0];
        poly->g3 = rec->inner[1];
        poly->b3 = rec->inner[2];
        if (v > 0) {
            Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)v, 1);
        }
        if (work->tail.phase == 0) {
            if (rec->size < 0x1000) {
                rec->size = (u32)(work->clock.frame << 12) / *(u32 *)((u8 *)work->config + 0x20);
                if (rec->size >= 0x1000) {
                    rec->size = 0x1000;
                    work->tail.phase = 1;
                }
            }
        } else if (work->tail.phase == 1) {
            if (work->field_15B8 < 0x800) {
                work->field_15B8 += work->clock.step << 6;
                if (work->field_15B8 >= 0x800) {
                    work->field_15B8 = 0x800;
                    work->tail.phase = 2;
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
