#include "../../types.h"

#include "petals.h"
#include "entry_second.h"

void ModelExodia_DrawPetals(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    ExodiaSecondEntryState *work;
    GsOT *ot;
    s16 i;
    s32 phi;
    s32 radius;
    s32 eighth;
    s32 scaled_width;
    s32 distance;
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

    work = (ExodiaSecondEntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    arm = work->petals;
    poly = work->packets.textured;
    radius = work->petal_radius * 4;
    eighth = work->petal_radius / 8;
    /* The template tests retain both products, but their assignments are dead. */
    if (radius * work->field_C8E < 0) {
        length = 0;
    }
    if (eighth * work->field_C8E < 0) {
        length = 0;
    }
    spread = 0x400 - work->field_C92;
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + work->petal_angle * 2;
        theta = (i << 13) / 12 + work->petal_angle;
        for (k = 0; k < 2; k++) {
            if (!(work->frame_count & 1)) {
                length = (k * 128 + 0x30) * spread / 1024;
            } else {
                length = (k * 64 + 0x18) * spread / 1024;
            }
            if (k == 0) {
                setVector(&arm->a[0], 0, 0, 0);
            } else {
                distance = radius * k;
                setVector(&arm->a[k], rcos(phi) * (rsin(theta) * distance >> 12) >> 12,
                          rcos(theta) * distance >> 12,
                          rsin(phi) * (rsin(theta) * distance >> 12) >> 12);
            }
            setVector(&arm->b[k], arm->a[k].vx + (rcos(turn) * length >> 12), arm->a[k].vy,
                      arm->a[k].vz + (rsin(turn) * length >> 12));
        }
    }
    if (!(work->frame_count & 1)) {
        size = 0x1000;
    } else {
        size = 0x1200;
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = work->origin.vx;
    m.t[1] = work->origin.vy;
    m.t[2] = work->origin.vz;
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
    arm = work->petals;
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 2; k++) {
            if (k == 1) {
                arm->otz[k] = RotTransPers4(&arm->a[0], &arm->a[k], &arm->a[0], &arm->a[k],
                                            &arm->sa[0], &arm->sa[k], &arm->sa[0], &arm->sa[k], &p, &arm->flag[k]);
                RotTransPers(&arm->b[k], &arm->sb[k], &p, &flag);
                dx = (s16)arm->sa[k] - (s16)arm->sa[0];
                dy = (arm->sa[k] >> 16) - (arm->sa[0] >> 16);
                arm->angle[k] = ratan2(dy, dx) + 0xC00;
                arm->width[k] = (s16)arm->sb[k] - (s16)arm->sa[k];
                arm->ox[k] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                arm->oy[k] = rsin(arm->angle[k]) * arm->width[k] >> 12;
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
    arm = work->petals;
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
            poly->r0 = arm->outer[k].r;
            poly->g0 = arm->outer[k].g;
            poly->b0 = arm->outer[k].b;
            poly->r1 = arm->outer[k + 1].r;
            poly->g1 = arm->outer[k + 1].g;
            poly->b1 = arm->outer[k + 1].b;
            poly->r2 = arm->inner[k].r;
            poly->g2 = arm->inner[k].g;
            poly->b2 = arm->inner[k].b;
            poly->r3 = arm->inner[k + 1].r;
            poly->g3 = arm->inner[k + 1].g;
            poly->b3 = arm->inner[k + 1].b;
            arm->otz[k] = 0;
            arm->flag[k] = 0;
            if (arm->otz[k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k] & 0xFFFF);
            }
            poly->x0 = arm->sa[k] - arm->ox[k];
            poly->y0 = (arm->sa[k] >> 16) - arm->oy[k];
            poly->x1 = arm->sa[k + 1] - arm->ox[k + 1];
            poly->y1 = (arm->sa[k + 1] >> 16) - arm->oy[k + 1];
            poly->x2 = arm->sa[k];
            poly->y2 = arm->sa[k] >> 16;
            poly->x3 = arm->sa[k + 1];
            poly->y3 = arm->sa[k + 1] >> 16;
            poly->r0 = arm->outer[k].r;
            poly->g0 = arm->outer[k].g;
            poly->b0 = arm->outer[k].b;
            poly->r1 = arm->outer[k + 1].r;
            poly->g1 = arm->outer[k + 1].g;
            poly->b1 = arm->outer[k + 1].b;
            poly->r2 = arm->inner[k].r;
            poly->g2 = arm->inner[k].g;
            poly->b2 = arm->inner[k].b;
            poly->r3 = arm->inner[k + 1].r;
            poly->g3 = arm->inner[k + 1].g;
            poly->b3 = arm->inner[k + 1].b;
            arm->otz[k] = 0;
            arm->flag[k] = 0;
            if (arm->otz[k] >= 0) {
                GsSortPoly(poly, ot, arm->otz[k] & 0xFFFF);
            }
        }
    }
    work->petal_angle += work->step * 16;
    if (work->petal_radius < 0x400) {
        work->petal_radius += work->step * 64;
        if (work->petal_radius >= 0x400) {
            work->petal_radius = 0x400;
        }
    }
}
