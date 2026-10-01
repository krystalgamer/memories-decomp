#include "../../types.h"
#include "variant459_curtains.h"

/* Draws five rotating curtains of sixteen textured strips. Each grows and
 * fades from 0x1400 to 0x1800, wraps with a new rotation until its deadline,
 * then stops at full scale with zero color. */
void func_8013D430(u8 *ctx)
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
    GsOT *ot;
    s32 i;
    u8 *work;
    Variant459Curtain *curtain;
    POLY_GT4 *poly;
    s32 j;
    s32 fade;
    s32 otz;

    work = ctx;
    curtain = (Variant459Curtain *)(work + 0x1A18);
    ot = func_80058F10();
    GsGetActiveBuff();
    poly = (POLY_GT4 *)(work + 0x2704);
    i = 0;
    ratan2(MODEL_VARIANT_WORD(work, 0x2778), MODEL_VARIANT_WORD(work, 0x2770));
    ratan2(MODEL_VARIANT_WORD(work, 0x2774), MODEL_VARIANT_WORD(work, 0x2778));
    do {
        if (curtain->scale > 0) {
            rsin(curtain->scale);
            rot.vx = curtain->rotation.vx;
            rot.vy = curtain->rotation.vy;
            rot.vz = curtain->rotation.vz;
            m.t[0] = MODEL_VARIANT_HALF(work, 0x2758);
            m.t[1] = MODEL_VARIANT_HALF(work, 0x275A);
            m.t[2] = MODEL_VARIANT_HALF(work, 0x275C);
            scale.vx = curtain->scale;
            scale.vy = curtain->scale;
            scale.vz = curtain->scale;
            RotMatrix(&rot, &m);
            ScaleMatrix(&m, &scale);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            if (curtain->scale < 0x1400) {
                light.r = 128;
                light.g = 128;
                light.b = 192;
                dark.r = 0;
                dark.g = 0;
                dark.b = 255;
            } else {
                fade = curtain->scale - 0x1400;
                light.r = 128 - fade / 8;
                light.g = 128 - fade / 8;
                light.b = 192 - fade * 192 / 1024;
                dark.r = 0;
                dark.g = 0;
                dark.b = 255 - fade * 255 / 1024;
            }
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
            for (j = 0; j < 16; j++) {
                otz = RotTransPers4(&curtain->a[j], &curtain->a[j + 1], &curtain->b[j], &curtain->b[j + 1],
                                    (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2,
                                    (PSXLONG *)&poly->x3, &p, &flag);
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, otz);
                }
            }
        }
        if (curtain->scale < 0x1800) {
            curtain->scale += MODEL_VARIANT_WORD(work, 0x27B8) << 7;
            if (curtain->scale >= 0x1800) {
                curtain->scale -= 0x1800;
                curtain->rotation.vx += 0x180;
                curtain->rotation.vz += 0x320;
                if ((u32)MODEL_VARIANT_WORD(work, 0x27B0) > (u32)MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(work, 0x27C0), 0x34)) {
                    curtain->scale = 0x1800;
                }
            }
        }
        i++;
        curtain++;
    } while (i < 5);
}
