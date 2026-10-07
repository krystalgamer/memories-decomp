#include "../../types.h"

#include "variant439_screen_rings.h"

/* Draws three rings of sixteen POLY_GT4 strips textured from the half of the
 * frame buffer each strip lands on. The inner row is rebuilt each frame on a
 * radius that follows the ring's scale; each scale grows by step * 96 and
 * wraps with a count until phase 3, then stays at full size. */
void func_8013C230(u8 *ctx)
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
    Variant439ScreenRing *ring;
    POLY_GT4 *poly;
    s32 i;
    s32 j;
    s32 angle;
    s32 r;
    s32 otz;
    s32 buff;
    s32 tpage;
    s32 size;

    work = ctx;
    ring = (Variant439ScreenRing *)(work + 0xE08);
    poly = (POLY_GT4 *)(work + 0x1808);
    ot = func_80058F10();
    buff = GsGetActiveBuff();
    for (i = 0; i < 3; i++, ring++) {
        if (ring->scale > 0) {
            size = ring->scale;
            r = size * 192 / 4096;
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            m.t[0] = MODEL_VARIANT_WORD(work, 0x18F8);
            m.t[1] = MODEL_VARIANT_WORD(work, 0x18FC);
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1900);
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
                setVector(&ring->a[j], rcos(angle) * (r + 128) >> 12, rsin(angle) * (r + 128) >> 12, 0);
            }
            for (j = 0; j < 16; j++) {
                RotTransPers4(&ring->a[j], &ring->a[j + 1], &ring->c[j], &ring->c[j + 1],
                              (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2,
                              (PSXLONG *)&poly->x3, &p, &flag);
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
                                    (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2,
                                    (PSXLONG *)&poly->x3, &p, &flag);
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
                    GsSortPoly(poly, ot, otz);
                }
            }
        }
        if (ring->scale < 0x1000) {
            ring->scale += MODEL_VARIANT_WORD(work, 0x1944) * 96;
            if (ring->scale >= 0x1000) {
                if (MODEL_VARIANT_WORD(work, 0x197C) >= 3) {
                    ring->scale = 0x1000;
                } else {
                    ring->scale -= 0x1000;
                    ring->count++;
                }
            }
        }
    }
}
