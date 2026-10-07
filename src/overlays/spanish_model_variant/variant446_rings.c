#include "../../types.h"

#include "../model_variant/model_variant.h"

/* The MODEL385 rings with this family's work block: up to four seventeen-point
 * POLY_GT4 rings around the variant's direction, ring 3 timed by the record and
 * the others grown by step * 128 until phase 5. Quads are sorted whenever the
 * depth is positive. */
void func_8013D4BC(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    u8 *pulse;
    POLY_GT4 *poly;
    ModelVariantCurtain *ring;
    GsOT *ot;
    s32 i;
    s32 j;
    s32 k;
    s32 a;
    s32 size;
    s32 depth;
    s32 amp;
    s32 first;
    s32 yaw;
    s32 heading;
    s32 pitch;
    s16 fade;
    u8 ir, ig, ib;
    u8 or, og, ob;

    work = ctx;
    ring = (ModelVariantCurtain *)(work + 0x1510);
    ot = func_80058F10();
    first = 1;
    pulse = work + 0x1130;
    poly = (POLY_GT4 *)(work + 0x1A4C);
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x1B6C), MODEL_VARIANT_WORD(work, 0x1B64));
    heading = yaw + 0x800;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x1B68), MODEL_VARIANT_WORD(work, 0x1B6C)) + 0x800;
    if (heading < 0x800) {
        heading = -heading + 0x400;
    } else {
        heading = yaw + 0xC00;
    }
    amp = ((MODEL_VARIANT_WORD(work, 0x1B90) & 1) << 5) * MODEL_VARIANT_WORD(pulse, 0x80) / 4096;
    if (MODEL_VARIANT_WORD(work, 0x1BDC) < 2) {
        k = 3;
        ring += 3;
    } else {
        k = 0;
        ring = (ModelVariantCurtain *)(work + 0x1510);
    }
    for (i = k; i < 4; i++, ring++) {
        if (ring->scale > 0) {
            size = ring->scale;
            for (j = 0, a = MODEL_VARIANT_HALF(work, 0x1BD2); j < 17; j++, a = MODEL_VARIANT_HALF(work, 0x1BD2) + (j << 8)) {
                if (i == 3) {
                    setVector(&ring->a[j], rcos(a) * MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x14) >> 12,
                              rsin(a) * MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x14) >> 12, 0);
                    setVector(&ring->b[j], rcos(a) * (MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x18) + amp) >> 12,
                              rsin(a) * (MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x18) + amp) >> 12, -(amp + 64));
                } else {
                    setVector(&ring->a[j], rcos(a) * MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x1C) >> 12,
                              rsin(a) * MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x1C) >> 12, 0);
                    setVector(&ring->b[j], rcos(a) * (MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x20) + amp) >> 12,
                              rsin(a) * (MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x20) + amp) >> 12, amp + 128);
                }
            }
            rot.vx = -pitch;
            rot.vy = heading;
            rot.vz = 0;
            if (i == 3) {
                m.t[0] = MODEL_VARIANT_WORD(work, 0x1B30);
                m.t[1] = MODEL_VARIANT_WORD(work, 0x1B34);
                m.t[2] = MODEL_VARIANT_WORD(work, 0x1B38);
            } else {
                m.t[0] = MODEL_VARIANT_HALF(work, 0x1B5C);
                m.t[1] = MODEL_VARIANT_HALF(work, 0x1B5E);
                m.t[2] = MODEL_VARIANT_HALF(work, 0x1B60);
            }
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            if (i == 3) {
                ir = work[0x1BBC];
                ig = work[0x1BBD];
                ib = work[0x1BBE];
                or = work[0x1BC0];
                og = work[0x1BC1];
                ob = work[0x1BC2];
            } else if (ring->scale < 0x800) {
                ir = work[0x1BBC];
                ig = work[0x1BBD];
                ib = work[0x1BBE];
                or = work[0x1BC0];
                og = work[0x1BC1];
                ob = work[0x1BC2];
            } else {
                fade = 0x1000 - ring->scale;
                ir = work[0x1BBC] * fade / 2048;
                ig = work[0x1BBD] * fade / 2048;
                ib = work[0x1BBE] * fade / 2048;
                or = work[0x1BC0] * fade / 2048;
                og = work[0x1BC1] * fade / 2048;
                ob = work[0x1BC2] * fade / 2048;
            }
            for (j = 0; j < 16; j++) {
                depth = RotTransPers4(&ring->a[j], &ring->a[j + 1],
                                      &ring->b[j], &ring->b[j + 1],
                                      (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                      (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                poly->r0 = ir;
                poly->g0 = ig;
                poly->b0 = ib;
                poly->r1 = ir;
                poly->g1 = ig;
                poly->b1 = ib;
                poly->r2 = or;
                poly->g2 = og;
                poly->b2 = ob;
                poly->r3 = or;
                poly->g3 = og;
                poly->b3 = ob;
                if (depth > 0) {
                    GsSortPoly(poly, ot, depth);
                }
            }
        }
        if (i == 3) {
            if (ring->scale < 0x1000 && MODEL_VARIANT_WORD(work, 0x1BDC) < 3) {
                ring->scale = (u32)(MODEL_VARIANT_WORD(work, 0x1B94) << 12) /
                             (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x30);
                if (ring->scale >= 0x1000) {
                    ring->scale = 0x1000;
                    ring->count++;
                }
            } else if ((u32)MODEL_VARIANT_WORD(work, 0x1B94) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x38)) {
                ring->scale = 0x1000 - (u32)((MODEL_VARIANT_WORD(work, 0x1B94) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x38)) << 12) /
                             (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x3C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1BA4), 0x38));
            }
        } else if (ring->scale < 0x1000) {
            ring->scale += MODEL_VARIANT_WORD(work, 0x1B9C) << 7;
            if (ring->scale >= 0x1000) {
                if (MODEL_VARIANT_WORD(work, 0x1BDC) >= 5) {
                    ring->scale = 0x1000;
                    if (i + 1 == 3 && first == 1) {
                        MODEL_VARIANT_WORD(work, 0x1BDC) = 6;
                    }
                } else {
                    first = 0;
                    ring->scale -= 0x1000;
                    ring->count++;
                }
            }
        }
    }
    MODEL_VARIANT_HALF(work, 0x1BD2) += MODEL_VARIANT_WORD(work, 0x1B9C) * 80;
}
