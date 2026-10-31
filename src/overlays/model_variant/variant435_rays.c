#include "../../types.h"

#include "variant425_rays.h"

/* The header-435 rays helper: the header-425 rays (variant425_rays.c) on a
 * work block 0xC60 bytes further on, so every work offset here is the header-425
 * one plus 0xC60. The status table has a slot for each of the three points,
 * and the last segment reads its previous point through a pointer whose index
 * is set in the otherwise dead length. */
void func_8013D3AC(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG status[16][3];
    PSXLONG p;
    PSXLONG flag;
    u8 *work;
    u8 *timing;
    GsOT *ot;
    s16 i;
    s32 sign;
    s32 yaw;
    s32 pitch;
    s32 phi;
    s16 r;
    s32 turn;
    s32 spin;
    Variant425Ray *ray;
    POLY_GT4 *poly;
    s16 k;
    s16 length;
    s16 size;
    s32 base;
    s32 dx;
    s32 dy;
    PSXLONG *closing;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;

    work = ctx;
    ot = func_80058F10();
    sign = -1;
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x3344), MODEL_VARIANT_WORD(work, 0x333C)) + 0xC00;
    yaw = ratan2(MODEL_VARIANT_WORD(work, 0x3320), MODEL_VARIANT_WORD(work, 0x3318)) + 0x800;
    timing = work + 0x2AC8;
    poly = (POLY_GT4 *)(work + 0x31D0);
    pitch = ratan2(MODEL_VARIANT_WORD(work, 0x331C), MODEL_VARIANT_WORD(work, 0x3320)) + 0x800;
    if (MODEL_VARIANT_HALF(work, 0x33BC) == 0) {
        sign = 1;
    }
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw += 0x400;
    }
    ray = (Variant425Ray *)(work + 0x1A20);
    for (i = 0; i < 16; i++, ray++) {
        spin = MODEL_VARIANT_HALF(work, 0x3368);
        base = i;
        if (!(i & 1)) {
            r = 0x100;
            phi = (base << 8) + spin * 2;
        } else {
            r = 0x80;
            phi = (base << 8) - spin * 2;
        }
        for (k = 0; k < 3; k++) {
            length = (k * 12 + 4) * MODEL_VARIANT_WORD(timing, 0x88) / 4096;
            if (k == 0) {
                setVector(&ray->a[k], 0, 0, 0);
            } else {
                setVector(&ray->a[k], (rsin(phi) * (r * k / 2) >> 12) + MODEL_VARIANT_WORD(work, 0x3328) * sign * k,
                          (rcos(phi) * (r * k / 2) >> 12) + MODEL_VARIANT_WORD(work, 0x332C) * sign * k,
                          MODEL_VARIANT_WORD(work, 0x3330) * sign * k);
            }
            setVector(&ray->b[k], ray->a[k].vx + (rcos(turn * sign) * length >> 12), ray->a[k].vy,
                      ray->a[k].vz + (rsin(turn * sign) * length >> 12));
        }
    }
    if (!(MODEL_VARIANT_WORD(work, 0x3354) & 1)) {
        size = MODEL_VARIANT_WORD(work, 0x339C);
    } else {
        size = MODEL_VARIANT_WORD(work, 0x339C) + MODEL_VARIANT_WORD(work, 0x339C) / 16;
    }
    rot.vx = -pitch;
    rot.vy = yaw;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x3308);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x330C);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x3310);
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
    ray = (Variant425Ray *)(work + 0x1A20);
    for (i = 0; i < 16; i++, ray++) {
        for (k = 0; k < 3; k++) {
            length = 1;
            if (k == 2) {
                closing = &ray->sa[length];
                ray->otz[2] = RotTransPers4(&ray->a[1], &ray->a[2], &ray->a[1], &ray->a[2],
                                            &ray->sa[1], &ray->sa[2], &ray->sa[1], &ray->sa[2],
                                            &p, &status[i][2]);
                RotTransPers(&ray->b[2], &ray->sb[2], &p, &flag);
                dx = (s16)ray->sa[2] - (s16)*closing;
                dy = (ray->sa[2] >> 16) - (*closing >> 16);
                ray->angle[2] = ratan2(dy, dx) + 0xC00;
                ray->width[2] = (s16)ray->sb[2] - (s16)ray->sa[2];
                ray->ox[2] = rcos(ray->angle[2]) * ray->width[2] >> 12;
                ray->oy[2] = rsin(ray->angle[2]) * ray->width[2] >> 12;
            } else {
                ray->otz[k] = RotTransPers4(&ray->a[k], &ray->a[k + 1], &ray->a[k], &ray->a[k + 1],
                                            &ray->sa[k], &ray->sa[k + 1], &ray->sa[k], &ray->sa[k + 1],
                                            &p, &status[i][k]);
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
    ray = (Variant425Ray *)(work + 0x1A20);
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
            if (ray->otz[k] >= 0 && status[i][k] >= 0) {
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
            if (ray->otz[k] >= 0 && status[i][k] >= 0) {
                GsSortPoly(poly, ot, ray->otz[k]);
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x339C) < 0x1000) {
        MODEL_VARIANT_WORD(work, 0x339C) += MODEL_VARIANT_WORD(work, 0x3360) * 256;
        if (MODEL_VARIANT_WORD(work, 0x339C) >= 0x1000) {
            MODEL_VARIANT_WORD(work, 0x339C) = 0x1000;
            if (MODEL_VARIANT_WORD(work, 0x33A8) == 2) {
                MODEL_VARIANT_WORD(work, 0x33A8) = 3;
            }
        }
    }
}
