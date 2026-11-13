#include "../../types.h"

#include "../model_variant/variant472_spiral.h"
#include "../model_variant/variant455_entry.h"

/* The North American header-455 build (CDK compiler) indexes the closing
 * segment's previous point directly instead of walking a pointer to it. */
#ifdef MODEL_VARIANT472_SPIRAL_DIRECT_PREVIOUS
#define SPIRAL472_PREVIOUS (&arm->sa[2])
#else
#define SPIRAL472_PREVIOUS previous
#endif

/* Twelve-arm spiral of four-point spines: the arms are built on a spiral of
 * radius and spread chosen by phase, leaned along the direction, moved along
 * the view, and drawn as two POLY_GT4 halves with per-point inner and outer
 * colours where the depth and the arm's projection flag are not negative. The
 * sweep advances by 16; the size grows over the timing record and fades out
 * after phase 0. */
void SpanishModelVariant472_DrawSpiral(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
#ifndef MODEL_VARIANT472_SPIRAL_DIRECT_PREVIOUS
    long *previous;
#endif
    EntryState455 *work;
    GsOT *ot;
    s16 i;
    s16 length;
    s32 phi;
    s32 theta;
    s32 r;
    s32 ux;
    s32 uy;
    s32 uz;
    s32 spread;
    s32 turn;
    s32 eighth;
    Variant472SpiralArm *arm;
    POLY_GT4 *poly;
    s16 k;
    s16 size;
    s32 dx;
    s32 dy;

    work = (EntryState455 *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    poly = work->textured;
    turn = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 0xC00;
    if (work->phase < 3) {
        spread = 0x400;
        r = work->spiral_size / 2;
    } else {
        r = 0x200;
        spread = work->spiral_size;
    }
    ux = work->aim.direction.vx * work->spiral_size / 1024;
    uy = work->aim.direction.vy * work->spiral_size / 1024;
    uz = work->aim.direction.vz * work->spiral_size / 1024;
    eighth = work->spiral_size / 8;
    if (r * work->field_2F28 < 0) {
        length = 0;
    }
    if (eighth * work->field_2F28 < 0) {
        length = 0;
    }
    arm = (Variant472SpiralArm *)work->fans;
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + work->spiral_angle * 2;
        theta = (i << 13) / 12 + work->spiral_angle;
        for (k = 0; k < 4; k++) {
            length = (k * 32 / 3 + 8) * spread / 1024;
            if (k == 0) {
                setVector(&arm->a[0], 0, 0, 0);
            } else {
                setVector(&arm->a[k], (rcos(phi) * (rsin(theta) * (r * k / 3) >> 12) >> 12) + ux * k / 3,
                          (rcos(theta) * (r * k / 3) >> 12) + uy * k / 3,
                          (rsin(phi) * (rsin(theta) * (r * k / 3) >> 12) >> 12) + uz * k / 3);
            }
            setVector(&arm->b[k], arm->a[k].vx + (rcos(turn) * length >> 12), arm->a[k].vy,
                      arm->a[k].vz + (rsin(turn) * length >> 12));
        }
    }
    if (!(work->clock.frame_count & 1)) {
        size = 0x1000;
    } else {
        size = 0x1200;
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = work->matrix.t[0];
    m.t[1] = work->matrix.t[1];
    m.t[2] = work->matrix.t[2];
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
    arm = (Variant472SpiralArm *)work->fans;
#ifdef MODEL_VARIANT472_SPIRAL_DIRECT_PREVIOUS
    for (i = 0; i < 12; i++, arm++) {
#else
    for (i = 0, previous = &arm->sa[2]; i < 12; i++,
         previous = (long *)((u8 *)previous + sizeof(Variant472SpiralArm)), arm++) {
#endif
        for (k = 0; k < 4; k++) {
            if (k == 3) {
                arm->otz[k] = RotTransPers4(&arm->a[2], &arm->a[3], &arm->a[2], &arm->a[3],
                                            SPIRAL472_PREVIOUS, &arm->sa[3], SPIRAL472_PREVIOUS, &arm->sa[3], &p, &arm->flag[3]);
                RotTransPers(&arm->b[3], &arm->sb[3], &p, &flag);
                dx = (s16)arm->sa[k] - (s16)*SPIRAL472_PREVIOUS;
                dy = (arm->sa[k] >> 16) - (*SPIRAL472_PREVIOUS >> 16);
                arm->angle[k] = ratan2(dy, dx) + 0xC00;
                arm->width[k] = (s16)arm->sb[k] - (s16)arm->sa[k];
                arm->ox[k] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                arm->oy[k] = rsin(arm->angle[k]) * arm->width[k] >> 12;
            } else {
                arm->otz[k] = RotTransPers4(&arm->a[k], &arm->a[k + 1], &arm->a[k], &arm->a[k + 1],
                                            &arm->sa[k], &arm->sa[k + 1], &arm->sa[k], &arm->sa[k + 1],
                                            &p, &arm->flag[k]);
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
    arm = (Variant472SpiralArm *)work->fans;
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 3; k++) {
            poly->x0 = arm->sa[k] + arm->ox[k];
            poly->y0 = (arm->sa[k] >> 16) + arm->oy[k];
            poly->x1 = arm->sa[k + 1] + arm->ox[k + 1];
            poly->y1 = (arm->sa[k + 1] >> 16) + arm->oy[k + 1];
            poly->x2 = arm->sa[k];
            poly->y2 = arm->sa[k] >> 16;
            poly->x3 = arm->sa[k + 1];
            poly->y3 = arm->sa[k + 1] >> 16;
            poly->r0 = arm->ca[k][0];
            poly->g0 = arm->ca[k][1];
            poly->b0 = arm->ca[k][2];
            poly->r1 = arm->ca[k + 1][0];
            poly->g1 = arm->ca[k + 1][1];
            poly->b1 = arm->ca[k + 1][2];
            poly->r2 = arm->cb[k][0];
            poly->g2 = arm->cb[k][1];
            poly->b2 = arm->cb[k][2];
            poly->r3 = arm->cb[k + 1][0];
            poly->g3 = arm->cb[k + 1][1];
            poly->b3 = arm->cb[k + 1][2];
            if (arm->otz[k] >= 0 && arm->flag[k] >= 0) {
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
            poly->r0 = arm->ca[k][0];
            poly->g0 = arm->ca[k][1];
            poly->b0 = arm->ca[k][2];
            poly->r1 = arm->ca[k + 1][0];
            poly->g1 = arm->ca[k + 1][1];
            poly->b1 = arm->ca[k + 1][2];
            poly->r2 = arm->cb[k][0];
            poly->g2 = arm->cb[k][1];
            poly->b2 = arm->cb[k][2];
            poly->r3 = arm->cb[k + 1][0];
            poly->g3 = arm->cb[k + 1][1];
            poly->b3 = arm->cb[k + 1][2];
            if (arm->otz[k] >= 0 && arm->flag[k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k]);
            }
        }
    }
    work->spiral_angle += 16;
    if (work->phase == 0) {
        if (work->spiral_size < 0x400) {
            work->spiral_size = ((work->clock.frame - work->config->fan_start) << 10) /
                                               (work->config->mesh_start - work->config->fan_start);
            if (work->spiral_size >= 0x400) {
                work->spiral_size = 0x400;
            }
        }
    } else if (work->clock.frame >= (u32)MODEL_VARIANT_WORD(work->config, 0x28) &&
               work->spiral_size > 0) {
        work->spiral_size = 0x400 - ((work->clock.frame - MODEL_VARIANT_WORD(work->config, 0x28)) << 10) /
                                           (u32)(MODEL_VARIANT_WORD(work->config, 0x2C) - MODEL_VARIANT_WORD(work->config, 0x28));
        if (work->spiral_size <= 0) {
            work->spiral_size = 0;
        }
    }
}
