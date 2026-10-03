#include "../../types.h"
#include "variant324_fan.h"

/* Draws four Gouraud quads around the shared center point. Its scale
 * changes only for the selected sequence entry; rotation advances each call. */
void func_8013C364(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    GsOT *ot;
    u8 *work;
    Variant324Fan *rec;
    POLY_G4 *poly;
    s32 i;
    s32 j;
    s32 bias;
    s32 otz;

    work = ctx;
    rec = (Variant324Fan *)(ctx + 0xD04);
    poly = (POLY_G4 *)(ctx + 0xD90);
    ot = func_80058F10();
    for (i = 0; i < 1; i++, rec++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(work, 0xF18) & 1) {
            bias = rec->size;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = *(u16 *)(work + 0xF4C);
        m.t[0] = MODEL_VARIANT_WORD(work, 0xED8);
        m.t[1] = MODEL_VARIANT_WORD(work, 0xEDC);
        m.t[2] = MODEL_VARIANT_WORD(work, 0xEE0);
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
                                (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
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
                func_8005B260((u32 *)poly, ot, (u16)otz, 1);
            }
            otz = RotTransPers4(&rec->point[j + 6], &rec->point[0],
                                &rec->point[j + 7], &rec->point[j + 8],
                                (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
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
                func_8005B260((u32 *)poly, ot, (u16)otz, 1);
            }
        }
        if (MODEL_VARIANT_HALF(work, 0xF40) + 1 == *(u16 *G32)(MODEL_VARIANT_WORD(work, 0xF2C) + 0xC)) {
            if (MODEL_VARIANT_WORD(work, 0xF50) == 0) {
                if (rec->size < 0x1000) {
                    rec->size = (u32)(MODEL_VARIANT_WORD(work, 0xF1C) << 12) / *(u32 *G32)(MODEL_VARIANT_WORD(work, 0xF2C) + 0x10);
                    if (rec->size >= 0x1000) {
                        rec->size = 0x1000;
                        MODEL_VARIANT_WORD(work, 0xF50) = 1;
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
    MODEL_VARIANT_WORD(work, 0xF4C) += MODEL_VARIANT_WORD(work, 0xF24) << 5;
}
