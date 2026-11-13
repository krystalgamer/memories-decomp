#include "../../types.h"

#include "../french_model_variant/variant452_entry.h"

/* Header 452's copy of the header-425 rays (variant425_rays.c) over its own
 * entry state: each ray keeps three status words and the tip segment is
 * indexed by k, which is 2 there; the PAL compiler then hoists ray + 8 and
 * ray + 4 out of the loop. */
/* Builds sixteen three-point rays fanned around the variant's axis (even rays
 * reach 0x100, odd ones 0x80, turning against each other with the entry's
 * spin angle and leaning along its offset vector, mirrored unless slot is 0),
 * projects each spine and a copy moved along the view, and draws each segment
 * as two POLY_GT4 halves as wide as the projected offset, the outer one fading
 * to black. The ray scale grows by step * 256 up to 0x1000, which moves phase
 * 2 on to phase 3. */
void ModelVariant425_DrawRays(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long status[16][3];
    long p;
    long flag;
    Variant452EntryState *work;
    ModelVariantSheet *timing;
    GsOT *ot;
    s16 i;
    s32 sign;
    s32 yaw;
    s32 pitch;
    s32 phi;
    s16 r;
    s32 turn;
    s32 spin;
    ModelVariantRay *ray;
    POLY_GT4 *poly;
    s16 k;
    s16 length;
    s16 size;
    s32 base;
    s32 dx;
    s32 dy;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;

    work = (Variant452EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    sign = -1;
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    yaw = ratan2(work->delta.vz, work->delta.vx) + 0x800;
    timing = work->sheets;
    poly = work->packets.textured;
    pitch = ratan2(work->delta.vy, work->delta.vz) + 0x800;
    if (work->slot == 0) {
        sign = 1;
    }
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw += 0x400;
    }
    ray = work->trails;
    for (i = 0; i < 16; i++, ray++) {
        spin = work->streamer_rotation;
        base = i;
        if (!(i & 1)) {
            r = 0x100;
            phi = (base << 8) + spin * 2;
        } else {
            r = 0x80;
            phi = (base << 8) - spin * 2;
        }
        for (k = 0; k < 3; k++) {
            length = (k * 12 + 4) * timing->size / 4096;
            if (k == 0) {
                setVector(&ray->a[k], 0, 0, 0);
            } else {
                setVector(&ray->a[k], (rsin(phi) * (r * k / 2) >> 12) + work->path.vx * sign * k,
                          (rcos(phi) * (r * k / 2) >> 12) + work->path.vy * sign * k,
                          work->path.vz * sign * k);
            }
            setVector(&ray->b[k], ray->a[k].vx + (rcos(turn * sign) * length >> 12), ray->a[k].vy,
                      ray->a[k].vz + (rsin(turn * sign) * length >> 12));
        }
    }
    if (!(work->clock.frame_count & 1)) {
        size = work->field_339C;
    } else {
        size = work->field_339C + work->field_339C / 16;
    }
    rot.vx = -pitch;
    rot.vy = yaw;
    rot.vz = 0;
    m.t[0] = work->position.vx;
    m.t[1] = work->position.vy;
    m.t[2] = work->position.vz;
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
    ray = work->trails;
    for (i = 0; i < 16; i++, ray++) {
        for (k = 0; k < 3; k++) {
            if (k == 2) {
                ray->otz[k] = RotTransPers4(&ray->a[1], &ray->a[2], &ray->a[1], &ray->a[2],
                                            &ray->sa[1], &ray->sa[2], &ray->sa[1], &ray->sa[2],
                                            /* Header 425's own overrun: with two columns the third
                                             * point's flag lands in status[i + 1][0] (past the array
                                             * for the last ray) and is never read. Legal spellings
                                             * change the frame or the address arithmetic. */
                                            &p, &status[i][2]);
                RotTransPers(&ray->b[2], &ray->sb[2], &p, &flag);
                /* Header 435's masked halves keep the status row's address
                 * inside the loop. */
                dx = (s16)(ray->sa[k] & 0xFFFF) - (s16)(ray->sa[k - 1] & 0xFFFF);
                dy = (s16)(ray->sa[k] >> 16) - (s16)(ray->sa[k - 1] >> 16);
                ray->angle[k] = ratan2(dy, dx) + 0xC00;
                ray->width[k] = (s16)(ray->sb[k] & 0xFFFF) - (s16)(ray->sa[k] & 0xFFFF);
                ray->ox[k] = rcos(ray->angle[k]) * ray->width[k] >> 12;
                ray->oy[k] = rsin(ray->angle[k]) * ray->width[k] >> 12;
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
    ray = work->trails;
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
    if (work->field_339C < 0x1000) {
        work->field_339C += work->clock.step * 256;
        if (work->field_339C >= 0x1000) {
            work->field_339C = 0x1000;
            if (work->phase == 2) {
                work->phase = 3;
            }
        }
    }
}
