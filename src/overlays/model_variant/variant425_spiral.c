#include "../../types.h"

#include "variant425_spiral.h"

/* Header 425's form of the header-418 spiral: sixteen two-point arms (records
 * from work + 0x600) fanned on a sphere of radius 0x200 at angles i/16 of a
 * turn plus the sweep at work + 0x2708, each with a copy moved along the view
 * by (0x400 - work[0x2710]) / 128 or / 32, projected and drawn as two POLY_GT4
 * halves in fixed yellow shades, only where the depth and the projection
 * flag are not negative. The scale comes from the timing record's size,
 * shrunk by work[0x2738]; from phase 2 on that grows by step * 128 up to
 * 0x800. The sweep advances by 16 a frame. The two products tested before the
 * arms are template code with dead arms. */
void func_8013C568(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG status[16][2];
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    u8 *timing;
    GsOT *ot;
    s16 i;
    s32 phi;
    s32 eighth;
    s32 r;
    s32 turn;
    Variant425SpiralArm *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 spread;
    s32 theta;
    s16 length;
    s16 size;
    s32 dx;
    s32 dy;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;

    arm = (Variant425SpiralArm *)(ctx + 0x600);
    work = ctx;
    ot = func_80058F10();
    spread = 0x400 - MODEL_VARIANT_WORD(work, 0x2710);
#if defined(VERSION_FRENCH)
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x26E4), MODEL_VARIANT_WORD(work, 0x26DC));
    timing = work + 0x1E68;
    turn += 0xC00;
#else
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x26E4), MODEL_VARIANT_WORD(work, 0x26DC)) + 0xC00;
    timing = work + 0x1E68;
#endif
    poly = (POLY_GT4 *)(work + 0x2570);
    r = 0x200;
    eighth = MODEL_VARIANT_HALF(work, 0x270C) / 8;
    if (r * MODEL_VARIANT_HALF(work, 0x270A) < 0) {
        length = 0;
    }
    if (eighth * MODEL_VARIANT_HALF(work, 0x270A) < 0) {
        length = 0;
    }
    for (i = 0; i < 16; i++, arm++) {
        phi = (i << 8) + MODEL_VARIANT_HALF(work, 0x2708) * 2;
        theta = (i << 9) + MODEL_VARIANT_HALF(work, 0x2708);
        for (k = 0; k < 2; k++) {
            if (k == 0) {
                length = spread / 128;
            } else {
                length = spread / 32;
            }
            if (k == 0) {
                setVector(&arm->a[k], 0, 0, 0);
            } else {
                setVector(&arm->a[k], rcos(phi) * (rsin(theta) * (r * k) >> 12) >> 12,
                          rcos(theta) * (r * k) >> 12,
                          rsin(phi) * (rsin(theta) * (r * k) >> 12) >> 12);
            }
            setVector(&arm->b[k], arm->a[k].vx + (rcos(turn) * length >> 12), arm->a[k].vy,
                      arm->a[k].vz + (rsin(turn) * length >> 12));
        }
    }
    if (!(MODEL_VARIANT_WORD(work, 0x26F4) & 1)) {
        size = MODEL_VARIANT_WORD(timing, 0x88) * (0x1000 - MODEL_VARIANT_WORD(work, 0x2738)) / 4096;
    } else {
        size = MODEL_VARIANT_WORD(timing, 0x88) * (0x1000 - MODEL_VARIANT_WORD(work, 0x2738)) / 4096 +
               MODEL_VARIANT_WORD(timing, 0x88) / 8;
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x26A8);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x26AC);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x26B0);
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
    arm = (Variant425SpiralArm *)(work + 0x600);
    for (i = 0; i < 16; i++, arm++) {
        for (k = 0; k < 2; k++) {
            if (k == 1) {
                arm->otz[1] = RotTransPers4(&arm->a[0], &arm->a[1], &arm->a[0], &arm->a[1],
                                            &arm->sa[0], &arm->sa[1], &arm->sa[0], &arm->sa[1],
                                            &p, &status[i][1]);
                RotTransPers(&arm->b[1], &arm->sb[1], &p, &flag);
                dx = (s16)arm->sa[1] - (s16)arm->sa[0];
                dy = (arm->sa[1] >> 16) - (arm->sa[0] >> 16);
                arm->angle[1] = ratan2(dy, dx) + 0xC00;
                arm->width[1] = (s16)arm->sb[1] - (s16)arm->sa[1];
                arm->ox[1] = rcos(arm->angle[1]) * arm->width[1] >> 12;
                arm->oy[1] = rsin(arm->angle[1]) * arm->width[1] >> 12;
            } else {
                arm->otz[k] = RotTransPers4(&arm->a[k], &arm->a[k + 1], &arm->a[k], &arm->a[k + 1],
                                            &arm->sa[k], &arm->sa[k + 1], &arm->sa[k], &arm->sa[k + 1],
                                            &p, &status[i][k]);
                RotTransPers(&arm->b[k], &arm->sb[k], &p, &flag);
                dx = (s16)arm->sa[k + 1] - (s16)arm->sa[k];
                dy = (arm->sa[k + 1] >> 16) - (arm->sa[k] >> 16);
                arm->angle[k] = ratan2(dy, dx) + 0xC00;
                arm->width[k] = (s16)arm->sb[k] - (s16)arm->sa[k];
                arm->ox[k] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                arm->oy[k] = rsin(arm->angle[k]) * arm->width[k] >> 12;
            }
        }
    }
    c0 = 0x40;
    c1 = 0;
    c2 = 0xA0;
    c3 = 0x80;
    arm = (Variant425SpiralArm *)(work + 0x600);
    for (i = 0; i < 16; i++, arm++) {
        for (k = 0; k < 1; k++) {
            poly->x0 = arm->sa[k] + arm->ox[k];
            poly->y0 = (arm->sa[k] >> 16) + arm->oy[k];
            poly->x1 = arm->sa[k + 1] + arm->ox[k + 1];
            poly->y1 = (arm->sa[k + 1] >> 16) + arm->oy[k + 1];
            poly->x2 = arm->sa[k];
            poly->y2 = arm->sa[k] >> 16;
            poly->x3 = arm->sa[k + 1];
            poly->y3 = arm->sa[k + 1] >> 16;
            if (k + 1 == 1) {
                poly->r0 = c0;
                poly->g0 = c0;
                poly->b0 = c1;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = c2;
                poly->g2 = c2;
                poly->b2 = c3;
                poly->r3 = 0;
                poly->g3 = 0;
                poly->b3 = 0;
            } else {
                poly->r0 = c0;
                poly->g0 = c0;
                poly->b0 = c1;
                poly->r1 = c0;
                poly->g1 = c0;
                poly->b1 = c1;
                poly->r2 = c2;
                poly->g2 = c2;
                poly->b2 = c3;
                poly->r3 = c2;
                poly->g3 = c2;
                poly->b3 = c3;
            }
            if (arm->otz[k] >= 0 && status[i][k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k]);
            }
            poly->x0 = arm->sa[k] - arm->ox[k];
            poly->y0 = (arm->sa[k] >> 16) - arm->oy[k];
            poly->x1 = arm->sa[k + 1] - arm->ox[k + 1];
            poly->y1 = (arm->sa[k + 1] >> 16) - arm->oy[k + 1];
            poly->x2 = arm->sa[k];
            poly->y2 = arm->sa[k] >> 16;
            poly->x3 = arm->sa[k + 1];
            poly->y3 = arm->sa[k + 1] >> 16;
            if (k + 1 == 1) {
                poly->r0 = c0;
                poly->g0 = c0;
                poly->b0 = c1;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = c2;
                poly->g2 = c2;
                poly->b2 = c3;
                poly->r3 = 0;
                poly->g3 = 0;
                poly->b3 = 0;
            } else {
                poly->r0 = c0;
                poly->g0 = c0;
                poly->b0 = c1;
                poly->r1 = c0;
                poly->g1 = c0;
                poly->b1 = c1;
                poly->r2 = c2;
                poly->g2 = c2;
                poly->b2 = c3;
                poly->r3 = c2;
                poly->g3 = c2;
                poly->b3 = c3;
            }
            if (arm->otz[k] >= 0 && status[i][k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k]);
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x2738) < 0x800 && MODEL_VARIANT_WORD(work, 0x2748) >= 2) {
        MODEL_VARIANT_WORD(work, 0x2738) += MODEL_VARIANT_WORD(work, 0x2700) * 128;
        if (MODEL_VARIANT_WORD(work, 0x2738) >= 0x800) {
            MODEL_VARIANT_WORD(work, 0x2738) = 0x800;
        }
    }
    MODEL_VARIANT_HALF(work, 0x2708) += 16;
}
