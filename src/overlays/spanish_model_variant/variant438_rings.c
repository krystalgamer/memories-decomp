#include "../../types.h"

#include "../model_variant/model_variant.h"

#define RING_COUNT 1
#define LAST_RING (RING_COUNT - 1)
#define TIMING(work) MODEL_VARIANT_WORD(work, 0x22B4)

/* The MODEL446 rings with a single ring: one seventeen-point POLY_GT4 ring
 * around the variant's direction, timed by the record, with radii scaled from
 * the timing record. Quads are sorted when depth and flag are non-negative. */
void func_8013CB5C(u8 *ctx)
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
    ring = (ModelVariantCurtain *)(work + 0x848);
    ot = func_80058F10();
    first = 1;
    pulse = work + 0x1938;
    poly = (POLY_GT4 *)(work + 0x2140);
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x2274), MODEL_VARIANT_WORD(work, 0x226C));
    heading = yaw + 0x800;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x2270), MODEL_VARIANT_WORD(work, 0x2274)) + 0x800;
    if (heading < 0x800) {
        heading = -heading + 0x400;
    } else {
        heading = yaw + 0xC00;
    }
    amp = ((MODEL_VARIANT_WORD(work, 0x2298) & 1) << 5) * MODEL_VARIANT_WORD(pulse, 0x88) / 4096;
    if (MODEL_VARIANT_WORD(work, 0x22E8) < 2) {
        k = LAST_RING;
        ring += LAST_RING;
    } else {
        k = 0;
        ring = (ModelVariantCurtain *)(work + 0x848);
    }
    for (i = k; i < RING_COUNT; i++, ring++) {
        if (ring->scale > 0) {
            size = ring->scale;
            for (j = 0, a = MODEL_VARIANT_WORD(work, 0x22D4); j < 17; j++, a = MODEL_VARIANT_WORD(work, 0x22D4) + (j << 8)) {
                if (i == LAST_RING) {
                    setVector(&ring->a[j], rcos(a) * (MODEL_VARIANT_WORD(TIMING(work), 0x44) / 4) >> 12,
                              rsin(a) * (MODEL_VARIANT_WORD(TIMING(work), 0x44) / 4) >> 12, 0);
                    setVector(&ring->b[j], rcos(a) * (MODEL_VARIANT_WORD(TIMING(work), 0x44) * (amp + 128) / 256) >> 12,
                              rsin(a) * (MODEL_VARIANT_WORD(TIMING(work), 0x44) * (amp + 128) / 256) >> 12,
                              -(MODEL_VARIANT_WORD(TIMING(work), 0x44) * (amp + 64) / 256));
                } else {
                    setVector(&ring->a[j], rcos(a) * MODEL_VARIANT_WORD(TIMING(work), 0x44) >> 12,
                              rsin(a) * MODEL_VARIANT_WORD(TIMING(work), 0x44) >> 12, 0);
                    setVector(&ring->b[j], rcos(a) * (MODEL_VARIANT_WORD(TIMING(work), 0x44) * (amp + 384) / 256) >> 12,
                              rsin(a) * (MODEL_VARIANT_WORD(TIMING(work), 0x44) * (amp + 384) / 256) >> 12,
                              MODEL_VARIANT_WORD(TIMING(work), 0x44) * (amp + 128) / 256);
                }
            }
            rot.vx = -pitch;
            rot.vy = heading;
            rot.vz = 0;
            if (i == LAST_RING) {
                m.t[0] = MODEL_VARIANT_WORD(work, 0x2258);
                m.t[1] = MODEL_VARIANT_WORD(work, 0x225C);
                m.t[2] = MODEL_VARIANT_WORD(work, 0x2260);
            } else {
                m.t[0] = MODEL_VARIANT_HALF(work, 0x2264);
                m.t[1] = MODEL_VARIANT_HALF(work, 0x2266);
                m.t[2] = MODEL_VARIANT_HALF(work, 0x2268);
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
            if (i == LAST_RING) {
                ir = work[0x22CC];
                ig = work[0x22CD];
                ib = work[0x22CE];
                or = work[0x22D0];
                og = work[0x22D1];
                ob = work[0x22D2];
            } else if (ring->scale < 0x800) {
                ir = work[0x22CC];
                ig = work[0x22CD];
                ib = work[0x22CE];
                or = work[0x22D0];
                og = work[0x22D1];
                ob = work[0x22D2];
            } else {
                fade = 0x1000 - ring->scale;
                ir = work[0x22CC] * fade / 2048;
                ig = work[0x22CD] * fade / 2048;
                ib = work[0x22CE] * fade / 2048;
                or = work[0x22D0] * fade / 2048;
                og = work[0x22D1] * fade / 2048;
                ob = work[0x22D2] * fade / 2048;
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
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, depth);
                }
            }
        }
        if (i == LAST_RING) {
            if (ring->scale < 0x1000 && MODEL_VARIANT_WORD(work, 0x22E8) < 3) {
                ring->scale = (u32)(MODEL_VARIANT_WORD(work, 0x229C) << 12) /
                             (u32)MODEL_VARIANT_WORD(TIMING(work), 0x20);
                if (ring->scale >= 0x1000) {
                    ring->scale = 0x1000;
                    ring->count++;
                }
            } else if ((u32)MODEL_VARIANT_WORD(work, 0x229C) >= (u32)MODEL_VARIANT_WORD(TIMING(work), 0x28)) {
                ring->scale = 0x1000 - (u32)((MODEL_VARIANT_WORD(work, 0x229C) - MODEL_VARIANT_WORD(TIMING(work), 0x28)) << 12) /
                             (u32)(MODEL_VARIANT_WORD(TIMING(work), 0x2C) - MODEL_VARIANT_WORD(TIMING(work), 0x28));
            }
        } else if (ring->scale < 0x1000) {
            ring->scale += MODEL_VARIANT_WORD(work, 0x22A4) << 7;
            if (ring->scale >= 0x1000) {
                if (MODEL_VARIANT_WORD(work, 0x22E8) >= 5) {
                    ring->scale = 0x1000;
                    if (i + 1 == LAST_RING && first == 1) {
                        MODEL_VARIANT_WORD(work, 0x22E8) = 6;
                    }
                } else {
                    first = 0;
                    ring->scale -= 0x1000;
                    ring->count++;
                }
            }
        }
    }
    MODEL_VARIANT_WORD(work, 0x22D4) += MODEL_VARIANT_WORD(work, 0x22A4) * 80;
}
