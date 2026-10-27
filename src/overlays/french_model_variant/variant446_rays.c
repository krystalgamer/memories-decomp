#include "../../types.h"

#include "../model_variant/variant425_rays.h"

/* Builds sixteen three-point rays from the work start (even rays reach
 * 0x100, odd ones 0x80, turning against each other with the spin at
 * work + 0x1BC4 and leaning along the direction at work + 0x1B64, mirrored by
 * the flag at work + 0x1BE4), pushes a copy of each spine along the turn by
 * the length at work + 0x1BD8, projects both, and draws each segment as two
 * POLY_GT4 halves as wide as the projected offset, the outer one fading to
 * black. The size at work + 0x1BD4 grows by step * 128 up to 0x1000; once the
 * clock passes the timing table's start the length shrinks to zero over the
 * table's span. */
void func_8013DCAC(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    u8 *timing;
    GsOT *ot;
    s16 i;
    s16 length;
    s32 sign;
    s32 yaw;
    s32 pitch;
    s32 phi;
    s32 turn;
    s32 spin;
    s16 r;
    Variant425Ray *ray;
    POLY_GT4 *poly;
    s16 k;
    s16 size;
    s32 base;
    s32 dx;
    s32 dy;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;

    work = ctx;
    ot = func_80058F10();
    sign = -1;
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x1B80), MODEL_VARIANT_WORD(work, 0x1B78)) + 0xC00;
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x1B6C), MODEL_VARIANT_WORD(work, 0x1B64)) + 0x800;
    poly = (POLY_GT4 *)(work + 0x19B0);
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x1B68), MODEL_VARIANT_WORD(work, 0x1B6C)) + 0x800;
    if (MODEL_VARIANT_HALF(work, 0x1BE4) == 0) {
        sign = 1;
    }
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw += 0x400;
    }
    ray = (Variant425Ray *)work;
    for (i = 0; i < 16; i++, ray++) {
        spin = MODEL_VARIANT_HALF(work, 0x1BC4);
        base = i;
        if (!(i & 1)) {
            r = 0x100;
            phi = (base << 8) + spin * 2;
        } else {
            r = 0x80;
            phi = (base << 8) - spin * 2;
        }
        for (k = 0; k < 3; k++) {
            length = (k * 12 + 8) * MODEL_VARIANT_WORD(work, 0x1BD8) / 1024;
            if (k == 0) {
                setVector(&ray->a[k], 0, 0, 0);
            } else {
                setVector(&ray->a[k], (rsin(phi) * (r * k / 2) >> 12) + MODEL_VARIANT_WORD(work, 0x1B64) * sign / 2 * k / 2,
                          (rcos(phi) * (r * k / 2) >> 12) + MODEL_VARIANT_WORD(work, 0x1B68) * sign / 2 * k / 2,
                          MODEL_VARIANT_WORD(work, 0x1B6C) * sign / 2 * k / 2);
            }
            setVector(&ray->b[k], ray->a[k].vx + (rcos(turn * sign) * length >> 12), ray->a[k].vy,
                      ray->a[k].vz + (rsin(turn * sign) * length >> 12));
        }
    }
    if (!(MODEL_VARIANT_WORD(work, 0x1B90) & 1)) {
        size = MODEL_VARIANT_WORD(work, 0x1BD4);
    } else {
        size = MODEL_VARIANT_WORD(work, 0x1BD4) + MODEL_VARIANT_WORD(work, 0x1BD4) / 16;
    }
    rot.vx = -pitch - sign * 512;
    rot.vy = yaw;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x1B50);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x1B54) - 0x80;
    m.t[2] = MODEL_VARIANT_WORD(work, 0x1B58);
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
    ray = (Variant425Ray *)work;
    for (i = 0; i < 16; i++, ray++) {
        for (k = 0; k < 3; k++) {
            if (k == 2) {
                ray->otz[k] = RotTransPers4(&ray->a[k - 1], &ray->a[k], &ray->a[k - 1], &ray->a[k],
                                            &ray->sa[k - 1], &ray->sa[k], &ray->sa[k - 1], &ray->sa[k],
                                            &p, &flag);
                RotTransPers(&ray->b[k], &ray->sb[k], &p, &flag);
                dx = (s16)ray->sa[k] - (s16)ray->sa[k - 1];
                dy = (ray->sa[k] >> 16) - (ray->sa[k - 1] >> 16);
                ray->angle[k] = ratan2(dy, dx) + 0xC00;
                ray->width[k] = (s16)ray->sb[k] - (s16)ray->sa[k];
                ray->ox[k] = rcos(ray->angle[k]) * ray->width[k] >> 12;
                ray->oy[k] = rsin(ray->angle[k]) * ray->width[k] >> 12;
            } else {
                ray->otz[k] = RotTransPers4(&ray->a[k], &ray->a[k + 1], &ray->a[k], &ray->a[k + 1],
                                            &ray->sa[k], &ray->sa[k + 1], &ray->sa[k], &ray->sa[k + 1],
                                            &p, &flag);
                RotTransPers(&ray->b[k], &ray->sb[k], &p, &flag);
                dx = (s16)ray->sa[k + 1] - (s16)ray->sa[k];
                dy = (ray->sa[k + 1] >> 16) - (ray->sa[k] >> 16);
                ray->angle[k] = ratan2(dy, dx) + 0xC00;
                ray->width[k] = (s16)ray->sb[k] - (s16)ray->sa[k];
                ray->ox[k] = rcos(ray->angle[k]) * ray->width[k] >> 12;
                ray->oy[k] = rsin(ray->angle[k]) * ray->width[k] >> 12;
            }
        }
    }
    ray = (Variant425Ray *)work;
    for (i = 0; i < 16; i++, ray++) {
        c0 = 0x20;
        c1 = 0;
        c2 = 0x80;
        c3 = 0x70;
        for (k = 0; k < 2; k++) {
            poly->x0 = ray->sa[k] + ray->ox[k];
            poly->y0 = (ray->sa[k] >> 16) + ray->oy[k];
            poly->x1 = ray->sa[k + 1] + ray->ox[k + 1];
            poly->y1 = (ray->sa[k + 1] >> 16) + ray->oy[k + 1];
            poly->x2 = ray->sa[k];
            poly->y2 = ray->sa[k] >> 16;
            poly->x3 = ray->sa[k + 1];
            poly->y3 = ray->sa[k + 1] >> 16;
            if (k + 1 == 2) {
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
            if (ray->otz[k] > 0) {
                GsSortPoly(poly, ot, ray->otz[k]);
            }
            poly->x0 = ray->sa[k] - ray->ox[k];
            poly->y0 = (ray->sa[k] >> 16) - ray->oy[k];
            poly->x1 = ray->sa[k + 1] - ray->ox[k + 1];
            poly->y1 = (ray->sa[k + 1] >> 16) - ray->oy[k + 1];
            poly->x2 = ray->sa[k];
            poly->y2 = ray->sa[k] >> 16;
            poly->x3 = ray->sa[k + 1];
            poly->y3 = ray->sa[k + 1] >> 16;
            if (k + 1 == 2) {
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
            if (ray->otz[k] > 0) {
                GsSortPoly(poly, ot, ray->otz[k]);
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x1BD4) < 0x1000) {
        MODEL_VARIANT_WORD(work, 0x1BD4) += MODEL_VARIANT_WORD(work, 0x1B9C) * 128;
        if (MODEL_VARIANT_WORD(work, 0x1BD4) >= 0x1000) {
            MODEL_VARIANT_WORD(work, 0x1BD4) = 0x1000;
        }
    }
    timing = (u8 *)MODEL_VARIANT_WORD(work, 0x1BA4);
    if ((u32)MODEL_VARIANT_WORD(work, 0x1B94) >= (u32)MODEL_VARIANT_WORD(timing, 0x44) && MODEL_VARIANT_WORD(work, 0x1BD8) > 0) {
        MODEL_VARIANT_WORD(work, 0x1BD8) = 0x400 - ((u32)(MODEL_VARIANT_WORD(work, 0x1B94) - MODEL_VARIANT_WORD(timing, 0x44)) << 10) /
                                   (u32)(MODEL_VARIANT_WORD(timing, 0x48) - MODEL_VARIANT_WORD(timing, 0x44));
        if (MODEL_VARIANT_WORD(work, 0x1BD8) <= 0) {
            MODEL_VARIANT_WORD(work, 0x1BD8) = 0;
        }
    }
    MODEL_VARIANT_HALF(work, 0x1BC4) += 0x20;
}
