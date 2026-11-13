#include "../../types.h"

#include "variant446_entry.h"

/* Builds sixteen three-point rays from the work start (even rays reach
 * 0x100, odd ones 0x80, turning against each other with the entry's spin and
 * leaning along its aim direction, mirrored unless slot is 0), pushes a copy
 * of each spine along the turn by ray_length, projects both, and draws each
 * segment as two POLY_GT4 halves as wide as the projected offset, the outer
 * one fading to black. ray_size grows by step * 128 up to 0x1000; once the
 * clock passes the config's rays_end the length shrinks to zero over the
 * span to field_48. */
void FrenchModelVariant446_DrawRays(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    EntryState446 *work;
    EntryConfig446 *timing;
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
    ModelVariantRay *ray;
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

    work = (EntryState446 *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    sign = -1;
    turn = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 0xC00;
    yaw = ratan2(work->aim.direction.vz, work->aim.direction.vx) + 0x800;
    poly = work->textured;
    pitch = ratan2(work->aim.direction.vy, work->aim.direction.vz) + 0x800;
    if (work->slot == 0) {
        sign = 1;
    }
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw += 0x400;
    }
    ray = (ModelVariantRay *)work;
    for (i = 0; i < 16; i++, ray++) {
        spin = work->spin;
        base = i;
        if (!(i & 1)) {
            r = 0x100;
            phi = (base << 8) + spin * 2;
        } else {
            r = 0x80;
            phi = (base << 8) - spin * 2;
        }
        for (k = 0; k < 3; k++) {
            length = (k * 12 + 8) * work->ray_length / 1024;
            if (k == 0) {
                setVector(&ray->a[k], 0, 0, 0);
            } else {
                setVector(&ray->a[k], (rsin(phi) * (r * k / 2) >> 12) + work->aim.direction.vx * sign / 2 * k / 2,
                          (rcos(phi) * (r * k / 2) >> 12) + work->aim.direction.vy * sign / 2 * k / 2,
                          work->aim.direction.vz * sign / 2 * k / 2);
            }
            setVector(&ray->b[k], ray->a[k].vx + (rcos(turn * sign) * length >> 12), ray->a[k].vy,
                      ray->a[k].vz + (rsin(turn * sign) * length >> 12));
        }
    }
    if (!(work->frame_count & 1)) {
        size = work->ray_size;
    } else {
        size = work->ray_size + work->ray_size / 16;
    }
    rot.vx = -pitch - sign * 512;
    rot.vy = yaw;
    rot.vz = 0;
    m.t[0] = work->second_matrix.t[0];
    m.t[1] = work->second_matrix.t[1] - 0x80;
    m.t[2] = work->second_matrix.t[2];
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
    ray = (ModelVariantRay *)work;
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
    ray = (ModelVariantRay *)work;
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
    if (work->ray_size < 0x1000) {
        work->ray_size += work->step * 128;
        if (work->ray_size >= 0x1000) {
            work->ray_size = 0x1000;
        }
    }
    timing = work->config;
    if (work->elapsed >= timing->rays_end && work->ray_length > 0) {
        work->ray_length = 0x400 - ((work->elapsed - timing->rays_end) << 10) /
                                   (u32)(timing->field_48 - timing->rays_end);
        if (work->ray_length <= 0) {
            work->ray_length = 0;
        }
    }
    work->spin += 0x20;
}
