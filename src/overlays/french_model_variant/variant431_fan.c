#include "../../types.h"
#include "../model_variant/variant414_fan.h"
#include "variant431_entry.h"

void FrenchModelVariant431_DrawFans(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    GsOT *ot;
    Variant431EntryState *work;
    Variant414Fan *rec;
    POLY_G4 *poly;
    s32 i;
    s32 j;
    s32 bias;
    s32 otz;

    work = (Variant431EntryState *)ctx;
    rec = (Variant414Fan *)(ctx + 0x13B0);
    poly = (POLY_G4 *)(ctx + 0x1610);
    ot = Graphics_GetDefaultOrderingTable();
    for (i = 0; i < 5; i++, rec++) {
        bias = 0;
        if (work->frame_count & 1) {
            bias = rec->size;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = *(u16 *)((u8 *)work + 0x18DC);
        m.t[0] = MODEL_VARIANT_WORD((u8 *)work + i * 32, 0x1794);
        m.t[1] = MODEL_VARIANT_WORD((u8 *)work + i * 32, 0x1798);
        m.t[2] = MODEL_VARIANT_WORD((u8 *)work + i * 32, 0x179C);
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
        if (work->frame >=
            (u32)(MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x18B0) -
                                    -(work->start_index * 4), 0x2C) - 8)) {
            if (rec->size < 0x1000 && rec->done == 0) {
                rec->size += work->step << 10;
                if (rec->size >= 0x1000) {
                    rec->size = 0x1000;
                    rec->done = 1;
                }
            } else if (rec->size > 0 && rec->done == 1) {
                rec->size -= work->step << 8;
                if (rec->size <= 0) {
                    rec->size = 0;
                }
            }
        }
    }
    work->spin += work->step * 0xC0;
}
