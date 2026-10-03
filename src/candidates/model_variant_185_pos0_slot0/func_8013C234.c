#include "../../types.h"
#include "../../overlays/french_model_variant/variant439_entry.h"

/* CANDIDATE, NOT MATCHING: USA header-422 framebuffer-ring renderer.
 * gcc_2_7_2_cdk_g0 emits 0x4D8 bytes (310 instructions), matching the
 * retail function length. Twelve words differ, all at 0x12A0..0x12F0 in
 * the scale/radius and matrix-translation setup. Projection, texture-page
 * selection, UV/color writes, depth truncation, and scale/count updates
 * match. The setup still keeps the signed division numerator and cached
 * translations live in different registers from retail. Retail retains
 * the signed division's negative-value rounding branch.
 *
 * Three 0x1A8-byte rings at context + 0xE08 reuse the recovered header-439
 * layout. Each ring has three rows of seventeen vertices. The first row
 * is rebuilt with radius scale * 192 / 4096 + 128; sixteen quads first
 * sample the framebuffer against the third row, then project against the
 * second row and apply the stored inner/outer colors. X < 160 selects the
 * left texture page; the active framebuffer chooses its VRAM half. Only
 * positive depths sort, with the retail u16 depth conversion preserved.
 *
 * Scale grows by step * 96 even while a staggered ring is nonpositive.
 * Below phase 3 it wraps at 4096 and increments count; from phase 3 it
 * clamps to 4096. No new allocation or lifetime claim is made.
 *
 * Measured alternatives included separate/combined scale assignments,
 * SDK setVector macros, cached versus repeated scale reads, signed
 * quotient forms, and translation sampling order. Grouping the sampled
 * Y/Z translations after the X store yields this closest measured body.
 * Keep the sampled translation order when refining this attempt. The
 * compiler profile and retail assembly target are pinned in candidates.json.
 */
void func_8013C234(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    GsOT *ot;
    s32 i;
    s32 buff;
    Variant439EntryScreenRing *ring;
    POLY_GT4 *poly;
    s32 j;
    s32 angle;
    s32 radius;
    s32 size;
    s32 pos0, pos1, pos2;
    s32 otz;
    s32 tpage;

    work = ctx;
    ring = (Variant439EntryScreenRing *)(work + 0xE08);
    poly = (POLY_GT4 *)(work + 0x1808);
    ot = func_80058F10();
    buff = GsGetActiveBuff();
    for (i = 0; i < 3; i++, ring++) {
        if (ring->scale > 0) {
            size = ring->scale;
            radius = size * 192 / 4096 + 128;
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            pos0 = MODEL_VARIANT_WORD(work, 0x18F8);
            m.t[0] = pos0;
            pos2 = MODEL_VARIANT_WORD(work, 0x1900);
            pos1 = MODEL_VARIANT_WORD(work, 0x18FC);
            m.t[2] = pos2;
            m.t[1] = pos1;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
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
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                setVector(&ring->a[j], rcos(angle) * radius >> 12, rsin(angle) * radius >> 12, 0);
            }
            for (j = 0; j < 16; j++) {
                RotTransPers4(&ring->a[j], &ring->a[j + 1], &ring->c[j], &ring->c[j + 1],
                              (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                              (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                if (poly->x0 < 160) {
                    if (buff == 0) {
                        tpage = GetTPage(2, 2, 320, 0);
                    } else {
                        tpage = GetTPage(2, 2, 0, 0);
                    }
                    SetPolyGT4(poly);
                    poly->u0 = poly->x0;
                    poly->v0 = poly->y0;
                    poly->u1 = poly->x1;
                    poly->v1 = poly->y1;
                    poly->u2 = poly->x2;
                    poly->v2 = poly->y2;
                    poly->u3 = poly->x3;
                    poly->v3 = poly->y3;
                    poly->tpage = tpage;
                } else {
                    if (buff == 0) {
                        tpage = GetTPage(2, 2, 448, 0);
                    } else {
                        tpage = GetTPage(2, 2, 128, 0);
                    }
                    SetPolyGT4(poly);
                    poly->u0 = poly->x0 - 128;
                    poly->v0 = poly->y0;
                    poly->u1 = poly->x1 - 128;
                    poly->v1 = poly->y1;
                    poly->u2 = poly->x2 - 128;
                    poly->v2 = poly->y2;
                    poly->u3 = poly->x3 - 128;
                    poly->v3 = poly->y3;
                    poly->tpage = tpage;
                }
                otz = RotTransPers4(&ring->a[j], &ring->a[j + 1], &ring->b[j], &ring->b[j + 1],
                                    (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                    (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                poly->r0 = ring->inner.r;
                poly->g0 = ring->inner.g;
                poly->b0 = ring->inner.b;
                poly->r1 = ring->inner.r;
                poly->g1 = ring->inner.g;
                poly->b1 = ring->inner.b;
                poly->r2 = ring->outer.r;
                poly->g2 = ring->outer.g;
                poly->b2 = ring->outer.b;
                poly->r3 = ring->outer.r;
                poly->g3 = ring->outer.g;
                poly->b3 = ring->outer.b;
                if (otz > 0) {
                    GsSortPoly(poly, ot, (u16)otz);
                }
            }
        }
        if (ring->scale < 4096) {
            ring->scale += MODEL_VARIANT_WORD(work, 0x1944) * 96;
            if (ring->scale >= 4096) {
                if (MODEL_VARIANT_WORD(work, 0x197C) >= 3) {
                    ring->scale = 4096;
                } else {
                    ring->scale -= 4096;
                    ring->count++;
                }
            }
        }
    }
}
