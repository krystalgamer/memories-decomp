#include "../../types.h"
#include "variant324_fan.h"
#include "../french_model_variant/variant341_entry.h"

/* Draws four Gouraud quads around the shared center point. Its scale
 * changes only for the selected sequence entry; rotation advances each call.
 *
 * Named for the US header-324 images, but those build the header-341
 * entry (variant324_entry.c includes the French variant341_entry.c), so
 * the record it draws from is Variant341EntryState. */
void ModelVariant324_DrawFan(Variant341EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    GsOT *ot;
    Variant341EntryState *work;
    Variant324Fan *rec;
    POLY_G4 *poly;
    s32 i;
    s32 j;
    s32 bias;
    s32 otz;

    work = context;
    rec = context->fans;
    poly = &context->packets.quad;
    ot = Graphics_GetDefaultOrderingTable();
    for (i = 0; i < 1; i++, rec++) {
        bias = 0;
        if (work->frame_count & 1) {
            bias = rec->size;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = work->spin;
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
        for (j = 0; j < 4; j += 2) {
            otz = RotTransPers4(&rec->point[j + 1], &rec->point[0],
                                &rec->point[j + 2], &rec->point[j + 3],
                                (long *)&poly->x0, (long *)&poly->x1,
                                (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
            poly->r0 = rec->outer.r;
            poly->g0 = rec->outer.g;
            poly->b0 = rec->outer.b;
            poly->r1 = rec->inner.r;
            poly->g1 = rec->inner.g;
            poly->b1 = rec->inner.b;
            poly->r2 = rec->outer.r;
            poly->g2 = rec->outer.g;
            poly->b2 = rec->outer.b;
            poly->r3 = rec->outer.r;
            poly->g3 = rec->outer.g;
            poly->b3 = rec->outer.b;
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
            }
            otz = RotTransPers4(&rec->point[j + 6], &rec->point[0],
                                &rec->point[j + 7], &rec->point[j + 8],
                                (long *)&poly->x0, (long *)&poly->x1,
                                (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
            poly->r0 = rec->outer.r;
            poly->g0 = rec->outer.g;
            poly->b0 = rec->outer.b;
            poly->r1 = rec->inner.r;
            poly->g1 = rec->inner.g;
            poly->b1 = rec->inner.b;
            poly->r2 = rec->outer.r;
            poly->g2 = rec->outer.g;
            poly->b2 = rec->outer.b;
            poly->r3 = rec->outer.r;
            poly->g3 = rec->outer.g;
            poly->b3 = rec->outer.b;
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
            }
        }
        if (work->substep + 1 == work->config->substeps) {
            if (work->tail.phase == 0) {
                if (rec->size < 0x1000) {
                    rec->size = (u32)(work->frame << 12) / work->config->duration;
                    if (rec->size >= 0x1000) {
                        rec->size = 0x1000;
                        work->tail.phase = 1;
                    }
                }
            } else {
                if (rec->size > 0) {
                    rec->size -= work->step << 5;
                    if (rec->size <= 0) {
                        rec->size = 0;
                    }
                }
            }
        }
    }
    work->spin += work->step << 5;
}
