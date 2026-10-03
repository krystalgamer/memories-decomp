#include "../../types.h"

#include "model_variant.h"

/* Draws the growing POLY_G4 quad of the variant's single quad record and
 * advances its size and rotation through three phases. */
void func_8013DDBC(u8 *ctx)
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
    s32 bias;
    ModelVariantQuad *rec;
    POLY_G4 *poly;
    s32 v;

    poly = (POLY_G4 *)(ctx + 0x1958);
    rec = (ModelVariantQuad *)(ctx + 0x18AC);
    ot = func_80058F10();
    for (i = 0; i < 1; i++, rec++) {
        bias = 0;
        if (MODEL_VARIANT_WORD(ctx, 0x1AE0) & 1) {
            bias = rec->size / 16;
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = *(u16 *)(ctx + 0x1B14);
        m.t[0] = MODEL_VARIANT_WORD(ctx, 0x1AA0);
        m.t[1] = MODEL_VARIANT_WORD(ctx, 0x1AA4);
        m.t[2] = MODEL_VARIANT_WORD(ctx, 0x1AA8);
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
                          (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2,
                          (PSXLONG *)&poly->x3, &p, &flag);
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
            func_8005B260((u32 *)poly, ot, (u16)v, 1);
        }
        if (MODEL_VARIANT_WORD(ctx, 0x1B18) == 0) {
            if (rec->size < 0x1000) {
                rec->size = (u32)(MODEL_VARIANT_WORD(ctx, 0x1AE4) << 12) / *(u32 *G32)(MODEL_VARIANT_WORD(ctx, 0x1AF4) + 0x20);
                if (rec->size >= 0x1000) {
                    rec->size = 0x1000;
                    MODEL_VARIANT_WORD(ctx, 0x1B18) = 1;
                }
            }
        } else if (MODEL_VARIANT_WORD(ctx, 0x1B18) == 1) {
            if (MODEL_VARIANT_WORD(ctx, 0x1B14) < 0x800) {
                MODEL_VARIANT_WORD(ctx, 0x1B14) += MODEL_VARIANT_WORD(ctx, 0x1AEC) << 6;
                if (MODEL_VARIANT_WORD(ctx, 0x1B14) >= 0x800) {
                    MODEL_VARIANT_WORD(ctx, 0x1B14) = 0x800;
                    MODEL_VARIANT_WORD(ctx, 0x1B18) = 2;
                }
            }
        } else {
            if (rec->size > 0) {
                rec->size -= MODEL_VARIANT_WORD(ctx, 0x1AEC) << 5;
                if (rec->size <= 0) {
                    rec->size = 0;
                }
            }
        }
    }
}
