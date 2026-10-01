#include "../../types.h"

#include "variant405_veils.h"

/* Draws five veils of sixteen textured POLY_GT4 strips that open and close
 * with their scale, white at the inner row and blue at the outer one; each
 * strip is textured from the screen half it lands on. Each scale grows to
 * 0x800 and wraps with a count until the timing record's deadline, and the
 * phase moves on once every veil has wrapped. */
void func_8013C620(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    /* Retail reserves 24 bytes before the two color locals. */
    u8 unknown_stack[24];
    CVECTOR dark;
    CVECTOR light;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    GsOT *ot;
    s32 i;
    s32 all;
    s32 buff;
    Variant405Veil *veil;
    POLY_GT4 *poly;
    s32 j;
    s32 angle;
    s32 otz;
    s32 tpage;

    veil = (Variant405Veil *)ctx;
    work = ctx;
    ot = func_80058F10();
    all = 1;
    buff = GsGetActiveBuff();
    poly = (POLY_GT4 *)(work + 0x1CB8);
    ratan2(MODEL_VARIANT_WORD(work, 0x1D90), MODEL_VARIANT_WORD(work, 0x1D88));
    ratan2(MODEL_VARIANT_WORD(work, 0x1D8C), MODEL_VARIANT_WORD(work, 0x1D90));
    for (i = 0; i < 5; i++, veil++) {
        if (veil->scale > 0) {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                setVector(&veil->a[j], rcos(angle) * (rsin(veil->scale) * 224 >> 12) >> 12, 0,
                          rsin(angle) * (rsin(veil->scale) * 224 >> 12) >> 12);
                setVector(&veil->b[j], rcos(angle) * (rsin(veil->scale) * 320 >> 12) >> 12,
                          (u32)(rsin(veil->scale) * 3) >> 8,
                          rsin(angle) * (rsin(veil->scale) * 320 >> 12) >> 12);
                setVector(&veil->c[j], rcos(angle) * (rsin(veil->scale) * 416 >> 12) >> 12,
                          (u32)(rsin(veil->scale) * 3) >> 8,
                          rsin(angle) * (rsin(veil->scale) * 416 >> 12) >> 12);
            }
            light.r = 0xFF;
            light.g = 0xFF;
            light.b = 0xFF;
            dark.r = 0;
            dark.g = 0x80;
            dark.b = 0xFF;
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            m.t[0] = MODEL_VARIANT_WORD(work, 0x1D74);
            m.t[1] = MODEL_VARIANT_WORD(work, 0x1D78) - (rcos(veil->scale) * 224 >> 12);
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1D7C);
            RotMatrix(&rot, &m);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            for (j = 0; j < 16; j++) {
                RotTransPers4(&veil->a[j], &veil->a[j + 1], &veil->c[j], &veil->c[j + 1],
                              (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2,
                              (PSXLONG *)&poly->x3, &p, &flag);
                if (poly->x0 < 0xA0) {
                    if (buff == 0) {
                        tpage = GetTPage(2, 1, 0x140, 0);
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
                        tpage = GetTPage(2, 1, 0x1C0, 0);
                    } else {
                        tpage = GetTPage(2, 1, 0x80, 0);
                    }
                    SetPolyGT4(poly);
                    poly->u0 = poly->x0 - 0x80;
                    poly->v0 = poly->y0;
                    poly->u1 = poly->x1 - 0x80;
                    poly->v1 = poly->y1;
                    poly->u2 = poly->x2 - 0x80;
                    poly->v2 = poly->y2;
                    poly->u3 = poly->x3 - 0x80;
                    poly->v3 = poly->y3;
                    poly->tpage = tpage;
                }
                otz = RotTransPers4(&veil->a[j], &veil->a[j + 1], &veil->b[j], &veil->b[j + 1],
                                    (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2,
                                    (PSXLONG *)&poly->x3, &p, &flag);
                SetSemiTrans(poly, 1);
                SetShadeTex(poly, 0);
                poly->r0 = light.r;
                poly->g0 = light.g;
                poly->b0 = light.b;
                poly->r1 = light.r;
                poly->g1 = light.g;
                poly->b1 = light.b;
                poly->r2 = dark.r;
                poly->g2 = dark.g;
                poly->b2 = dark.b;
                poly->r3 = dark.r;
                poly->g3 = dark.g;
                poly->b3 = dark.b;
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, otz);
                }
            }
        }
        if (veil->scale < 0x800) {
            veil->scale += MODEL_VARIANT_WORD(work, 0x1DC0) * 40;
            if (veil->scale >= 0x800) {
                if (*(u32 *)(*(u8 *G32 *)(work + 0x1DC8) + 0x24) <= (u32)MODEL_VARIANT_WORD(work, 0x1DB8)) {
                    veil->scale = 0x800;
                    veil->count = 1;
                } else {
                    veil->scale -= 0x800;
                }
                if (MODEL_VARIANT_WORD(work, 0x1E1C) == 0 &&
                    *(u32 *)(*(u8 *G32 *)(work + 0x1DC8) + 0x20) <= (u32)MODEL_VARIANT_WORD(work, 0x1DB8)) {
                    MODEL_VARIANT_WORD(work, 0x1E1C) = 2;
                }
            }
        }
        all *= veil->count;
        if (i + 1 == 5 && all == 1 && MODEL_VARIANT_WORD(work, 0x1E1C) == 2) {
            MODEL_VARIANT_WORD(work, 0x1E1C) = i + 1;
        }
    }
}
