#include "../../types.h"
#include "../../overlays/model_variant/variant398_screen_rings.h"

/* CANDIDATE, NOT MATCHING: USA header-398 framebuffer-ring renderer.
 * gcc_2_7_2_cdk_g0 emits the retail 0x5F4 bytes (381 instructions).
 * Three words differ at image offsets 0x1368, 0x136C and 0x13F0:
 * the initialization/copy order of the point index and angle registers.
 * All other masked instruction words match; retail assembly stays linked.
 *
 * Three 0x1E4-byte records start at context + 0xE08. Each begins with
 * three rows of seventeen vertices. Above scale 2048 the first row expands
 * by (scale - 2048) * 192 / 2048 and bends backwards by that amount;
 * otherwise it has radius 256 and zero Z. Geometry is rebuilt even when
 * scale is nonpositive, but projection/drawing only runs for positive scale.
 * The direction at 0x19C0 determines the two rotation angles. The third
 * row samples the framebuffer and the second supplies the drawn quad.
 * The active framebuffer and screen X select a page with blend mode 1.
 * Frame bits select eight outer colors; the inner edge remains white.
 * Nonnegative depth and GTE flag are required before the u16 sort-depth cast.
 *
 * Scale grows by step * 96 below 4096, then wraps/increments count before
 * phase 3 or clamps thereafter. The record layout is an observed access
 * span, not evidence of allocation capacity. Five models use this family
 * in both slots: 102, 282, 288, 642 and 645.
 *
 * Separate geometry-radius and matrix-scale locals retain the target's
 * live ranges. Widening GetTPage's u16 result into s32 retains both retail
 * zero extensions. Hoisting angle initialization outside the two geometry
 * loops gives this three-word remainder; independent angle locals leave
 * four differences, while one shared in-loop initialization leaves thirteen.
 * No fixed registers, inline assembly or replacement opcodes are used.
 */
void func_8013C2B8(u8 *ctx)
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
    s32 angle_y;
    s32 angle_x;
    s32 buff;
    Variant398ScreenRing *ring;
    POLY_GT4 *poly;
    s32 j;
    s32 angle;
    s32 scale_size;
    s32 size;
    s32 otz;
    s32 tpage;
    s32 red;
    s32 green;
    s32 blue;

    work = ctx;
    ring = (Variant398ScreenRing *)(work + 0xE08);
    ot = func_80058F10();
    angle_y = ratan2(MODEL_VARIANT_WORD(work, 0x19C8), MODEL_VARIANT_WORD(work, 0x19C0)) + 0x800;
    angle_x = ratan2(MODEL_VARIANT_WORD(work, 0x19C4), MODEL_VARIANT_WORD(work, 0x19C8)) + 0x800;
    buff = GsGetActiveBuff();
    poly = (POLY_GT4 *)(work + 0x18BC);
    if (angle_y < 0x800) {
        angle_y = -angle_y + 0x400;
    } else {
        angle_y += 0x400;
    }
    for (i = 0; i < 3; i++, ring++) {
        angle = 0;
        if (ring->scale > 2048) {
            for (j = 0; j < 17; j++, angle = j << 8) {
                size = (ring->scale - 2048) * 192 / 2048;
                setVector(&ring->a[j], rcos(angle) * (size + 256) >> 12,
                          rsin(angle) * (size + 256) >> 12, -size);
            }
        } else {
            for (j = 0; j < 17; j++, angle = j << 8) {
                setVector(&ring->a[j], rcos(angle) * 256 >> 12,
                          rsin(angle) * 256 >> 12, 0);
            }
        }
        if (ring->scale > 0) {
            scale_size = ring->scale;
            rot.vx = -angle_x;
            rot.vy = angle_y;
            rot.vz = 0;
            m.t[0] = MODEL_VARIANT_WORD(work, 0x19AC);
            m.t[1] = MODEL_VARIANT_WORD(work, 0x19B0);
            m.t[2] = MODEL_VARIANT_WORD(work, 0x19B4);
            scale.vx = scale_size;
            scale.vy = scale_size;
            scale.vz = scale_size;
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            for (j = 0; j < 16; j++) {
                RotTransPers4(&ring->a[j], &ring->a[j + 1], &ring->c[j], &ring->c[j + 1],
                              (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                              (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
                if (poly->x0 < 160) {
                    if (buff == 0) {
                        tpage = GetTPage(2, 1, 320, 0);
                    } else {
                        tpage = GetTPage(2, 1, 0, 0);
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
                        tpage = GetTPage(2, 1, 448, 0);
                    } else {
                        tpage = GetTPage(2, 1, 128, 0);
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
                SetSemiTrans(poly, 0);
                SetShadeTex(poly, 0);
                if ((MODEL_VARIANT_WORD(work, 0x19EC) & 7) == 0) { red = 255; green = 0; blue = 0; }
                else if ((MODEL_VARIANT_WORD(work, 0x19EC) & 7) == 1) { red = 255; green = 128; blue = 0; }
                else if ((MODEL_VARIANT_WORD(work, 0x19EC) & 7) == 2) { red = 255; green = 255; blue = 0; }
                else if ((MODEL_VARIANT_WORD(work, 0x19EC) & 7) == 3) { red = 0; green = 255; blue = 0; }
                else if ((MODEL_VARIANT_WORD(work, 0x19EC) & 7) == 4) { red = 0; green = 255; blue = 255; }
                else if ((MODEL_VARIANT_WORD(work, 0x19EC) & 7) == 5) { red = 0; green = 0; blue = 255; }
                else if ((MODEL_VARIANT_WORD(work, 0x19EC) & 7) == 6) { red = 255; green = 0; blue = 255; }
                else { red = 255; green = 0; blue = 128; }
                setRGB0(poly, 255, 255, 255);
                setRGB1(poly, 255, 255, 255);
                setRGB2(poly, red, green, blue);
                setRGB3(poly, red, green, blue);
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, (u16)otz);
                }
            }
        }
        if (ring->scale < 4096) {
            ring->scale += MODEL_VARIANT_WORD(work, 0x19F8) * 96;
            if (ring->scale >= 4096) {
                if (MODEL_VARIANT_WORD(work, 0x1A30) < 3) {
                    ring->scale -= 4096;
                    ring->count++;
                } else {
                    ring->scale = 4096;
                }
            }
        }
    }
}
