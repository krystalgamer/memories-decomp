#include "../../types.h"
#include "variant324_fan.h"
#include "../french_model_variant/variant341_entry.h"

/* Draws four Gouraud quads around the shared center point. Its scale
 * changes only for the selected sequence entry; rotation advances each call. */
void ModelVariant324_DrawFan(u8 *ctx)
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

    work = (Variant341EntryState *)ctx;
    rec = (Variant324Fan *)(ctx + 0xD04);
    poly = (POLY_G4 *)(ctx + 0xD90);
    ot = Graphics_GetDefaultOrderingTable();
    for (i = 0; i < 1; i++, rec++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0xF18) & 1) {
            bias = rec->size;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = *(u16 *)((u8 *)work + 0xF4C);
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
            poly->r0 = rec->outer[0];
            poly->g0 = rec->outer[1];
            poly->b0 = rec->outer[2];
            poly->r1 = rec->inner[0];
            poly->g1 = rec->inner[1];
            poly->b1 = rec->inner[2];
            poly->r2 = rec->outer[0];
            poly->g2 = rec->outer[1];
            poly->b2 = rec->outer[2];
            poly->r3 = rec->outer[0];
            poly->g3 = rec->outer[1];
            poly->b3 = rec->outer[2];
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
            }
            otz = RotTransPers4(&rec->point[j + 6], &rec->point[0],
                                &rec->point[j + 7], &rec->point[j + 8],
                                (long *)&poly->x0, (long *)&poly->x1,
                                (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
            poly->r0 = rec->outer[0];
            poly->g0 = rec->outer[1];
            poly->b0 = rec->outer[2];
            poly->r1 = rec->inner[0];
            poly->g1 = rec->inner[1];
            poly->b1 = rec->inner[2];
            poly->r2 = rec->outer[0];
            poly->g2 = rec->outer[1];
            poly->b2 = rec->outer[2];
            poly->r3 = rec->outer[0];
            poly->g3 = rec->outer[1];
            poly->b3 = rec->outer[2];
            if (otz >= 0 && flag >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)poly, ot, (u16)otz, 1);
            }
        }
        if (work->substep + 1 == *(u16 *)(MODEL_VARIANT_WORD(work, 0xF2C) + 0xC)) {
            if (work->tail.phase == 0) {
                if (rec->size < 0x1000) {
                    rec->size = (u32)(MODEL_VARIANT_WORD(work, 0xF1C) << 12) / *(u32 *)(MODEL_VARIANT_WORD(work, 0xF2C) + 0x10);
                    if (rec->size >= 0x1000) {
                        rec->size = 0x1000;
                        work->tail.phase = 1;
                    }
                }
            } else {
                if (rec->size > 0) {
                    rec->size -= MODEL_VARIANT_WORD(work, 0xF24) << 5;
                    if (rec->size <= 0) {
                        rec->size = 0;
                    }
                }
            }
        }
    }
    work->spin += MODEL_VARIANT_WORD(work, 0xF24) << 5;
}
