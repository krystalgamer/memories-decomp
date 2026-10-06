#include "../../types.h"

#include "../model_variant/model_variant.h"

/* Draws up to four seventeen-point POLY_GT4 rings around the variant's
 * direction. Ring 3 grows with the timing record and shrinks over its fade
 * window; the other rings grow by step * 128 a frame, cycle until phase 5 and
 * fade out past half size. The ring angle advances by step * 80. */
void func_8013D6B4(u8 *ctx)
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
    ring = (ModelVariantCurtain *)(work + 0x98C);
    ot = func_80058F10();
    first = 1;
    pulse = work + 0x56C;
    poly = (POLY_GT4 *)(work + 0xEC8);
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0xFC8), MODEL_VARIANT_WORD(work, 0xFC0));
    heading = yaw + 0x800;
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0xFC4), MODEL_VARIANT_WORD(work, 0xFC8)) + 0x800;
    if (heading < 0x800) {
        heading = -heading + 0x400;
    } else {
        heading = yaw + 0xC00;
    }
    amp = ((MODEL_VARIANT_WORD(work, 0xFEC) & 1) << 5) * MODEL_VARIANT_WORD(pulse, 0x80) / 4096;
    if (MODEL_VARIANT_WORD(work, 0x1030) < 2) {
        k = 3;
        ring += 3;
    } else {
        k = 0;
        ring = (ModelVariantCurtain *)(work + 0x98C);
    }
    for (i = k; i < 4; i++, ring++) {
        if (ring->scale > 0) {
            size = ring->scale;
            for (j = 0, a = MODEL_VARIANT_HALF(work, 0x102E); j < 17; j++, a = MODEL_VARIANT_HALF(work, 0x102E) + (j << 8)) {
                if (i == 3) {
                    setVector(&ring->a[j], rcos(a) * MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x14) >> 12,
                              rsin(a) * MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x14) >> 12, 0);
                    setVector(&ring->b[j], rcos(a) * (MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x18) + amp) >> 12,
                              rsin(a) * (MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x18) + amp) >> 12, -(amp + 64));
                } else {
                    setVector(&ring->a[j], rcos(a) * MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x1C) >> 12,
                              rsin(a) * MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x1C) >> 12, 0);
                    setVector(&ring->b[j], rcos(a) * (MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x20) + amp) >> 12,
                              rsin(a) * (MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x20) + amp) >> 12, amp + 128);
                }
            }
            rot.vx = -pitch;
            rot.vy = heading;
            rot.vz = 0;
            if (i == 3) {
                m.t[0] = MODEL_VARIANT_WORD(work, 0xFAC);
                m.t[1] = MODEL_VARIANT_WORD(work, 0xFB0);
                m.t[2] = MODEL_VARIANT_WORD(work, 0xFB4);
            } else {
                m.t[0] = MODEL_VARIANT_HALF(work, 0xFB8);
                m.t[1] = MODEL_VARIANT_HALF(work, 0xFBA);
                m.t[2] = MODEL_VARIANT_HALF(work, 0xFBC);
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
                ir = work[0x1018];
                ig = work[0x1019];
                ib = work[0x101A];
                or = work[0x101C];
                og = work[0x101D];
                ob = work[0x101E];
            } else if (ring->scale < 0x800) {
                ir = work[0x1018];
                ig = work[0x1019];
                ib = work[0x101A];
                or = work[0x101C];
                og = work[0x101D];
                ob = work[0x101E];
            } else {
                fade = 0x1000 - ring->scale;
                ir = work[0x1018] * fade / 2048;
                ig = work[0x1019] * fade / 2048;
                ib = work[0x101A] * fade / 2048;
                or = work[0x101C] * fade / 2048;
                og = work[0x101D] * fade / 2048;
                ob = work[0x101E] * fade / 2048;
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
        if (i == 3) {
            if (ring->scale < 0x1000 && MODEL_VARIANT_WORD(work, 0x1030) < 3) {
                ring->scale = (u32)(MODEL_VARIANT_WORD(work, 0xFF0) << 12) /
                             (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x30);
                if (ring->scale >= 0x1000) {
                    ring->scale = 0x1000;
                    ring->count++;
                }
            } else if ((u32)MODEL_VARIANT_WORD(work, 0xFF0) >= (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x38)) {
                ring->scale = 0x1000 - (u32)((MODEL_VARIANT_WORD(work, 0xFF0) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x38)) << 12) /
                             (u32)(MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x3C) - MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x1000), 0x38));
            }
        } else if (ring->scale < 0x1000) {
            ring->scale += MODEL_VARIANT_WORD(work, 0xFF8) << 7;
            if (ring->scale >= 0x1000) {
                if (MODEL_VARIANT_WORD(work, 0x1030) >= 5) {
                    ring->scale = 0x1000;
                    if (i + 1 == 3 && first == 1) {
                        MODEL_VARIANT_WORD(work, 0x1030) = 6;
                    }
                } else {
                    first = 0;
                    ring->scale -= 0x1000;
                    ring->count++;
                }
            }
        }
    }
    MODEL_VARIANT_HALF(work, 0x102E) += MODEL_VARIANT_WORD(work, 0xFF8) * 80;
}
