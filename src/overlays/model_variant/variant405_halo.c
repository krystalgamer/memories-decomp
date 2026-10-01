#include "../../types.h"

#include "variant405_halo.h"

/* Draws five rings of sixteen textured POLY_GT4 strips between a top and a
 * bottom row of seventeen points; each row hangs below the ring by its own
 * height, which grows by step << 5 up to 0x400 and wraps with a count until
 * the timing record's deadline. The strips fade from grey as the heights pass
 * 0x300. */
void func_8013C12C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    u8 r1[5];
    u8 g1[5];
    u8 b1[5];
    u8 r2[5];
    u8 g2[5];
    u8 b2[5];
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    Variant405Halo *halo;
    GsOT *ot;
    POLY_GT4 *poly;
    s32 i;
    s32 j;
    s32 rise;
    s32 fall;
    s32 angle;
    s32 c;
    s32 s;
    s32 x;
    s32 y;
    s32 gap;
    s32 otz;

    work = ctx;
    poly = (POLY_GT4 *)(work + 0x1BDC);
    ot = func_80058F10();
    halo = (Variant405Halo *)(work + 0x820);
    ratan2(MODEL_VARIANT_WORD(work, 0x1D90), MODEL_VARIANT_WORD(work, 0x1D88));
    ratan2(MODEL_VARIANT_WORD(work, 0x1D8C), MODEL_VARIANT_WORD(work, 0x1D90));
    for (i = 0; i < 5; i++) {
        if (halo->rise[i] <= 0) {
            rise = 0;
            fall = 0;
        } else if (halo->fall[i] <= 0) {
            rise = halo->rise[i];
            fall = 0;
        } else {
            rise = halo->rise[i];
            fall = halo->fall[i];
        }
        for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
            c = rcos(angle) * 256 >> 12;
            s = rsin(angle) * 256 >> 12;
            setVector(&halo->top[i][j], c, -(rise * 768 / 1024), s);
            setVector(&halo->bottom[i][j], c, -(fall * 768 / 1024), s);
        }
        if (halo->rise[i] > 0x300) {
            x = 0;
            if (rise < 0x400) {
                x = 0x400 - rise;
            }
            r1[i] = x / 2;
            g1[i] = x / 2;
            b1[i] = x / 2;
            y = 0x400 - fall;
            r2[i] = y / 2;
            g2[i] = y / 2;
            b2[i] = y / 2;
        } else {
            r1[i] = 0x80;
            g1[i] = 0x80;
            b1[i] = 0x80;
            r2[i] = 0x80;
            g2[i] = 0x80;
            b2[i] = 0x80;
        }
        if (halo->fall[i] < 0x400) {
            halo->rise[i] += MODEL_VARIANT_WORD(work, 0x1DC0) << 5;
            halo->fall[i] += MODEL_VARIANT_WORD(work, 0x1DC0) << 5;
            if (halo->fall[i] >= 0x400) {
                gap = halo->rise[i] - halo->fall[i];
                if (*(u32 *)(*(u8 *G32 *)(work + 0x1DC8) + 0x28) <= (u32)MODEL_VARIANT_WORD(work, 0x1DB8)) {
                    halo->rise[i] = 0x400;
                    halo->fall[i] = 0x400;
                } else {
                    halo->rise[i] = halo->fall[i] - 0x400;
                    halo->fall[i] = halo->rise[i] - gap;
                }
                halo->count[i]++;
            }
        }
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_HALF(work, 0x1D80);
    m.t[1] = 0;
    m.t[2] = MODEL_VARIANT_HALF(work, 0x1D84);
    scale.vx = 0x1000;
    scale.vy = 0x1000;
    scale.vz = 0x1000;
    RotMatrix(&rot, &m);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    for (i = 0; i < 5; i++) {
        poly->r0 = r1[i];
        poly->g0 = g1[i];
        poly->b0 = b1[i];
        poly->r1 = r1[i];
        poly->g1 = g1[i];
        poly->b1 = b1[i];
        poly->r2 = r2[i];
        poly->g2 = g2[i];
        poly->b2 = b2[i];
        poly->r3 = r2[i];
        poly->g3 = g2[i];
        poly->b3 = b2[i];
        for (j = 0; j < 16; j++) {
            poly->u0 = j * 16;
            poly->v0 = 0x7F;
            poly->u1 = j * 16 + 16;
            poly->v1 = 0x7F;
            poly->u2 = j * 16;
            poly->v2 = 0x60;
            poly->u3 = j * 16 + 16;
            poly->v3 = 0x60;
            otz = RotTransPers4(&halo->top[i][j], &halo->top[i][j + 1], &halo->bottom[i][j], &halo->bottom[i][j + 1],
                                (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3,
                                &p, &flag);
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
    }
}
