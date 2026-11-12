#include "../../types.h"

#include "variant418_spiral.h"
#include "variant418_entry.h"

/* Builds twelve two-point arms on a spiral around the variant's direction,
 * projects each spine and a copy of it moved along the view, and draws each
 * arm as two POLY_GT4 halves as wide as the projected offset. The spiral turns
 * by 0x10 a frame and grows and widens through the timing record's phases. */
void ModelVariant418_DrawSpiral(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant418EntryState *work;
    GsOT *ot;
    s16 i;
    s32 phi;
    s32 ux;
    s32 uy;
    s32 uz;
    s32 spread;
    s32 turn;
    ModelVariantFlaggedSpiralArm *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 theta;
    s16 length;
    s16 size;
    s32 dx;
    s32 dy;

    work = (Variant418EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    turn = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 0xC00;
    ux = work->aim.direction.vx;
    uy = work->aim.direction.vy;
    uz = work->aim.direction.vz;
    arm = work->arms;
    poly = work->packets.textured;
    spread = 0x400 - work->spiral_shrink;
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + work->spiral_angle * 2;
        theta = (i << 13) / 12 + work->spiral_angle;
        for (k = 0; k < 2; k++) {
            if (!(work->clock.frame_count & 1)) {
                length = (k * 32 + 0x10) * spread / 1024;
            } else {
                length = (k * 24 + 0xC) * spread / 1024;
            }
            if (k == 0) {
                setVector(&arm->a[0], 0, 0, 0);
            } else {
                setVector(&arm->a[k], (rcos(phi) * ((rsin(theta) << 9) * k >> 12) >> 12) + ux * k,
                          ((rcos(theta) << 9) * k >> 12) + uy * k,
                          (rsin(phi) * ((rsin(theta) << 9) * k >> 12) >> 12) + uz * k);
            }
            setVector(&arm->b[k], arm->a[k].vx + (rcos(turn) * length >> 12), arm->a[k].vy,
                      arm->a[k].vz + (rsin(turn) * length >> 12));
        }
    }
    if (!(work->clock.frame_count & 1)) {
        size = work->spiral_size;
    } else {
        size = work->spiral_size + work->spiral_size / 8;
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
    arm = work->arms;
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 2; k++) {
            if (k == 1) {
#ifdef VERSION_FRENCH
                arm->otz[k] = RotTransPers4(&arm->a[0], &arm->a[k], &arm->a[0], &arm->a[k],
                                            &arm->sa[0], &arm->sa[k], &arm->sa[0], &arm->sa[k], &p, &arm->flag[k]);
                RotTransPers(&arm->b[k], &arm->sb[k], &p, &flag);
                dx = (s16)arm->sa[k] - (s16)arm->sa[0];
                dy = (arm->sa[k] >> 16) - (arm->sa[0] >> 16);
                arm->angle[k] = ratan2(dy, dx) + 0xC00;
                arm->width[k] = (s16)arm->sb[k] - (s16)arm->sa[k];
                arm->ox[k] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                arm->oy[k] = rsin(arm->angle[k]) * arm->width[k] >> 12;
#else
                arm->otz[1] = RotTransPers4(&arm->a[0], &arm->a[1], &arm->a[0], &arm->a[1],
                                            &arm->sa[0], &arm->sa[1], &arm->sa[0], &arm->sa[1], &p, &arm->flag[1]);
                RotTransPers(&arm->b[1], &arm->sb[1], &p, &flag);
                dx = (s16)arm->sa[1] - (s16)arm->sa[0];
                dy = (arm->sa[1] >> 16) - (arm->sa[0] >> 16);
                arm->angle[1] = ratan2(dy, dx) + 0xC00;
                arm->width[1] = (s16)arm->sb[1] - (s16)arm->sa[1];
                arm->ox[1] = rcos(arm->angle[1]) * arm->width[1] >> 12;
                arm->oy[1] = rsin(arm->angle[1]) * arm->width[1] >> 12;
#endif
            } else {
                arm->otz[k] = RotTransPers4(&arm->a[k], &arm->a[k + 1], &arm->a[k], &arm->a[k + 1],
                                            &arm->sa[k], &arm->sa[k + 1], &arm->sa[k], &arm->sa[k + 1], &p, &arm->flag[k]);
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
    arm = work->arms;
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 1; k++) {
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
            arm->otz[k] = 0;
            arm->flag[k] = 0;
            if (arm->otz[k] >= 0) {
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
            arm->otz[k] = 0;
            arm->flag[k] = 0;
            if (arm->otz[k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k]);
            }
        }
    }
    work->spiral_angle += 0x10;
    if (work->phase == 1) {
        if (work->spiral_size < 0x1000) {
            work->spiral_size = (u32)((work->clock.frame - MODEL_VARIANT_WORD(work->config, 0x2C)) << 12) /
                                          (u32)(MODEL_VARIANT_WORD(work->config, 0x30) - MODEL_VARIANT_WORD(work->config, 0x2C));
            if (work->spiral_size >= 0x1000) {
                work->spiral_size = 0x1000;
                work->phase = 2;
            }
        }
    } else if ((u32)MODEL_VARIANT_WORD(work->config, 0x34) < (u32)work->clock.frame &&
               work->phase == 2 && work->spiral_shrink < 0x400) {
        work->spiral_shrink = (u32)((work->clock.frame - MODEL_VARIANT_WORD(work->config, 0x34)) << 10) /
                                      (u32)(MODEL_VARIANT_WORD(work->config, 0x38) - MODEL_VARIANT_WORD(work->config, 0x34));
        if (work->spiral_shrink >= 0x400) {
            work->spiral_shrink = 0x400;
            work->phase = 3;
        }
    }
}
