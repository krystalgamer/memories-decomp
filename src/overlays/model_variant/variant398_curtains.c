#include "../../types.h"

#include "model_variant.h"

/* Header 398's form of the header-422 curtains: each frame the two rows of
 * every shown curtain are rebuilt around the rotating angle at work + 0x1A2C,
 * the outer row lifted by the first sheet's size when flag bit 0 is set. */
void func_8013DA44(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    ModelVariantCurtain *curtain;
    GsOT *ot;
    s32 i;
    s32 lift;
    s32 yaw;
    s32 pitch;
    s32 radius;
    POLY_GT4 *poly;
    s32 j;
    s32 otz;
    s32 size;
    s32 done;
    s32 angle;
    ModelVariantSheet *sheet;
    s16 fade;
    u8 r0;
    u8 g0;
    u8 b0;
    u8 r1;
    u8 g1;
    u8 b1;

    work = ctx;
    curtain = (ModelVariantCurtain *)(work + 0x13B4);
    ot = func_80058F10();
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x19C8), MODEL_VARIANT_WORD(work, 0x19C0)) + 0x800;
    sheet = (ModelVariantSheet *)(work + 0x6A8);
    poly = (POLY_GT4 *)(work + 0x18F0);
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x19C4), MODEL_VARIANT_WORD(work, 0x19C8)) + 0x800;
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw += 0x400;
    }
    lift = ((MODEL_VARIANT_WORD(work, 0x19EC) & 1) << 5) * sheet->size / 4096;
    done = 1;
    i = 0;
    radius = lift + 0x180;
    for (; i < 3; i++, curtain++) {
        if (curtain->scale > 0) {
            size = curtain->scale;
            for (j = 0, angle = MODEL_VARIANT_WORD(work, 0x1A2C); j < 17;
                 j++, angle = MODEL_VARIANT_WORD(work, 0x1A2C) + j * 256) {
                setVector(&curtain->a[j], (u32)rcos(angle) >> 4, (u32)rsin(angle) >> 4, 0);
                setVector(&curtain->b[j], rcos(angle) * radius >> 12, rsin(angle) * radius >> 12, lift + 0x80);
            }
            rot.vx = -pitch;
            rot.vy = yaw;
            rot.vz = 0;
            m.t[0] = MODEL_VARIANT_HALF(work, 0x19B8);
            m.t[1] = MODEL_VARIANT_HALF(work, 0x19BA);
            m.t[2] = MODEL_VARIANT_HALF(work, 0x19BC);
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
            if (curtain->scale < 0x800) {
                r0 = work[0x1A24];
                g0 = work[0x1A25];
                b0 = work[0x1A26];
                r1 = work[0x1A28];
                g1 = work[0x1A29];
                b1 = work[0x1A2A];
            } else {
                fade = 0x1000 - curtain->scale;
                r0 = work[0x1A24] * fade / 2048;
                g0 = work[0x1A25] * fade / 2048;
                b0 = work[0x1A26] * fade / 2048;
                r1 = work[0x1A28] * fade / 2048;
                g1 = work[0x1A29] * fade / 2048;
                b1 = work[0x1A2A] * fade / 2048;
            }
            for (j = 0; j < 16; j++) {
                otz = RotTransPers4(&curtain->a[j], &curtain->a[j + 1], &curtain->b[j], &curtain->b[j + 1],
                                    (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2,
                                    (PSXLONG *)&poly->x3, &p, &flag);
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = r0;
                poly->g1 = g0;
                poly->b1 = b0;
                poly->r2 = r1;
                poly->g2 = g1;
                poly->b2 = b1;
                poly->r3 = r1;
                poly->g3 = g1;
                poly->b3 = b1;
                if (otz >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, otz);
                }
            }
        }
        if (curtain->scale < 0x1000) {
            curtain->scale += MODEL_VARIANT_WORD(work, 0x19F8) << 7;
            if (curtain->scale >= 0x1000) {
                if (MODEL_VARIANT_WORD(work, 0x1A30) >= 4) {
                    curtain->scale = 0x1000;
                } else {
                    curtain->scale -= 0x1000;
                    curtain->count++;
                }
                if (i + 1 == 3 && done == 1 && MODEL_VARIANT_WORD(work, 0x1A30) == 4) {
                    MODEL_VARIANT_WORD(work, 0x1A30) = 5;
                }
            }
        }
    }
    MODEL_VARIANT_WORD(work, 0x1A2C) += MODEL_VARIANT_WORD(work, 0x19F8) * 80;
}
