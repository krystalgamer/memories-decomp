#include "../../types.h"

#include "model_variant.h"

#define PETAL_SCALE(w, i) (*(s32 *)((u8 *)(w) + 0x794 + (i) * 4))
#define PETAL_DONE(w, i) (*(s32 *)((u8 *)(w) + 0x854 + (i) * 4))

/* Draws 48 petals: each a POLY_G4 grown from the variant centre along its own
 * velocity and swung around a circle, faded past half size, and cycles each
 * petal's size through the timing record's phases. */
void func_8013C2DC(u8 *ctx)
{
    SVECTOR rot;
    /* The target reserves 16 unused bytes between rot and scale. */
    u8 unknown_stack[16];
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    u8 *base;
    GsOT *ot;
    s16 i;
    s16 sweep;
    s32 done;
    u8 r;
    u8 g;
    u8 b;
    MATRIX *lp;
    POLY_G4 *poly;
    u8 *conf;
    s16 phase;
    s32 size;
    s32 fade;
    s16 radius;
    s16 lift;
    s32 ang;
    s32 otz;

    work = ctx;
    base = ctx;
    poly = (POLY_G4 *)(base + 0x2320);
    ot = func_80058F10();
    done = 1;
    lp = &ls;
    i = 0;
    sweep = 0;
    phase = 0;
    ratan2(MODEL_VARIANT_WORD(base, 0x2398), MODEL_VARIANT_WORD(base, 0x239C));
    ratan2(MODEL_VARIANT_HALF(base, 0x23A6), MODEL_VARIANT_HALF(base, 0x23A4));
    do {
        size = PETAL_SCALE(work, i);
        if (size > 0) {
            if (size > 0x200) {
                fade = 0x400 - size;
                r = work[0x780] * fade / 512;
                g = work[0x781] * fade / 512;
                b = work[0x782] * fade / 512;
            } else {
                r = work[0x780];
                g = work[0x781];
                b = work[0x782];
            }
            radius = *(u16 *)(*(u8 *G32 *)(base + 0x29D4) + 0x1E) -
                     (rcos(PETAL_SCALE(work, i)) * *(u16 *)(*(u8 *G32 *)(base + 0x29D4) + 0x1E) >> 12);
            lift = (rsin(PETAL_SCALE(work, i)) * 0x1000 >> 12) + 0x180;
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = 0;
            fade = 0x400;
            ang = phase + (sweep - fade);
            m.t[0] = MODEL_VARIANT_WORD(base, 0x2380) + *(s32 *)(base - -(i * 16) + 0x26A8) * PETAL_SCALE(work, i) / 512 + (rcos(ang) * radius >> 12);
            m.t[1] = MODEL_VARIANT_WORD(base, 0x2384) + *(s32 *)(base - -(i * 16) + 0x26AC) * PETAL_SCALE(work, i) / 512 + (rsin(ang) * radius >> 12);
            m.t[2] = MODEL_VARIANT_WORD(base, 0x2388) + *(s32 *)(base - -(i * 16) + 0x26B0) * PETAL_SCALE(work, i) / 512;
            scale.vx = lift;
            scale.vy = lift;
            scale.vz = lift;
            RotMatrix(&rot, &m);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, lp);
            GsSetLsMatrix(lp);
            ReadRotMatrix(lp);
            RotMatrix(&rot, lp);
            ScaleMatrix(lp, &scale);
            SetRotMatrix(lp);
            otz = RotTransPers4((SVECTOR *)(work + i * 8), (SVECTOR *)(work + (i * 8 + 0x180)),
                                (SVECTOR *)(work + (i * 8 + 0x300)), (SVECTOR *)(work + (i * 8 + 0x480)),
                                (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
            poly->r0 = r;
            poly->g0 = g;
            poly->b0 = b;
            if (otz >= 0 && flag >= 0 && PETAL_DONE(work, i) == 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
        if (PETAL_SCALE(work, i) < 0x400) {
            conf = *(u8 *G32 *)(base + 0x29D4);
            PETAL_SCALE(work, i) += (u32)(*(u16 *)(conf + 0x20) * MODEL_VARIANT_WORD(base, 0x29CC)) >> 1;
            if (PETAL_SCALE(work, 0) >= 0x200 && MODEL_VARIANT_WORD(base, 0x29FC) == 0) {
                MODEL_VARIANT_WORD(base, 0x29FC) = 2;
            }
            if (PETAL_SCALE(work, i) >= 0x400) {
                if (*(u32 *)(*(u8 *G32 *)(base + 0x29D4) + 0x28) > (u32)MODEL_VARIANT_WORD(base, 0x29C4)) {
                    PETAL_SCALE(work, i) -= 0x400;
                    *(s32 *)(work + 0x9D4 + i * 4) = 0;
                } else {
                    PETAL_SCALE(work, i) = 0x400;
                    PETAL_DONE(work, i) = 1;
                }
            } else if (PETAL_SCALE(work, i) <= 0 &&
                       *(u32 *)(*(u8 *G32 *)(base + 0x29D4) + 0x28) <= (u32)MODEL_VARIANT_WORD(base, 0x29C4)) {
                PETAL_SCALE(work, i) = 0x400;
                PETAL_DONE(work, i) = 1;
            }
        }
        done *= PETAL_DONE(work, i);
        if (i + 1 == 48 && done == 1 && MODEL_VARIANT_WORD(base, 0x29FC) == 2) {
            MODEL_VARIANT_WORD(base, 0x29FC) = 5;
        }
        i++;
        sweep += 0x40;
        phase = (s16)(i % 5) * 0x1000 / 5;
    } while (i < 48);
}
