#include "../../types.h"
#include "../model_variant/e575_fan.h"
#include "e575_entry_pal.h"

void FrenchModelVariant431_DrawFans(Variant431EntryState *context)
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

    work = context;
    rec = context->fans;
    poly = &context->packets.quad;
    ot = Graphics_GetDefaultOrderingTable();
    for (i = 0; i < 5; i++, rec++) {
        bias = 0;
        if (work->frame_count & 1) {
            bias = rec->size;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = work->spin;
        m.t[0] = work->matrices[i].t[0];
        m.t[1] = work->matrices[i].t[1];
        m.t[2] = work->matrices[i].t[2];
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
        if (work->frame >=
            (work->config->timings[work->start_index] - 8)) {
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
