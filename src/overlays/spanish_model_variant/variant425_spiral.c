#include "../../types.h"

#include "../model_variant/variant448_spiral.h"

/* Header 425's phase-guarded spiral: while the phase at work + 0x180C is not
 * negative, twelve two-point arms (records from work + 0x84C) on a spiral of
 * three quarters of the size at work + 0x17DC are moved along the view by the
 * spread and drawn as two POLY_GT4 halves in each arm's own colours where the
 * depth and projection flag are not negative. Reaching size 0x400 moves phase
 * 1 to 2; from phase 2 on the sweep at work + 0x17D8 advances by 48. */
void SpanishModelVariant425_DrawSpiral(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long status[12][2];
    long p;
    long flag;
    u8 *work;
    u8 *timing;
    GsOT *ot;
    s16 i;
    s32 phi;
    s32 eighth;
    s32 r;
    ModelVariantShadedRay *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 spread;
    s32 turn;
    s32 theta;
    s16 length;
    s16 size;
    s32 dx;
    s32 dy;

    work = ctx;
    ot = Graphics_GetDefaultOrderingTable();
    poly = (POLY_GT4 *)(work + 0x166C);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x1778), MODEL_VARIANT_WORD(work, 0x1770)) + 0xC00;
    if (MODEL_VARIANT_WORD(work, 0x180C) >= 0) {
    r = MODEL_VARIANT_HALF(work, 0x17DC) * 768 / 1024;
    eighth = MODEL_VARIANT_HALF(work, 0x17DC) / 8;
    if (r * MODEL_VARIANT_HALF(work, 0x17DA) < 0) {
        length = 0;
    }
    if (eighth * MODEL_VARIANT_HALF(work, 0x17DA) < 0) {
        length = 0;
    }
    if (MODEL_VARIANT_WORD(work, 0x180C) >= 3) {
        timing = work + 0xEB4;
        spread = MODEL_VARIANT_WORD(timing, 0x88) / 8;
    } else {
        spread = 0x400 - MODEL_VARIANT_HALF(work, 0x17DE);
    }
    arm = (ModelVariantShadedRay *)(work + 0x84C);
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + MODEL_VARIANT_HALF(work, 0x17D8) * 2;
        theta = (i << 13) / 12 + MODEL_VARIANT_HALF(work, 0x17D8);
        for (k = 0; k < 2; k++) {
            length = (k * 16 + 4) * spread / 1024;
            if (k == 0) {
                setVector(&arm->points[k], 0, 0, 0);
            } else {
                setVector(&arm->points[k], rcos(phi) * (rsin(theta) * (r * k) >> 12) >> 12,
                          rcos(theta) * (r * k) >> 12,
                          rsin(phi) * (rsin(theta) * (r * k) >> 12) >> 12);
            }
            setVector(&arm->edges[k], arm->points[k].vx + (rcos(turn) * length >> 12), arm->points[k].vy,
                      arm->points[k].vz + (rsin(turn) * length >> 12));
        }
    }
    if (!(MODEL_VARIANT_WORD(work, 0x1788) & 1)) {
        size = 0x1000;
    } else {
        size = 0x1200;
    }
    arm = (ModelVariantShadedRay *)(work + 0x84C);
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = MODEL_VARIANT_WORD(work, 0x1748);
    m.t[1] = MODEL_VARIANT_WORD(work, 0x174C);
    m.t[2] = MODEL_VARIANT_WORD(work, 0x1750);
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
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 2; k++) {
            if (k == 1) {
                arm->depth[1] = RotTransPers4(&arm->points[0], &arm->points[1], &arm->points[0], &arm->points[1],
                                            &arm->projected[0], &arm->projected[1], &arm->projected[0], &arm->projected[1],
                                            &p, &status[i][1]);
                RotTransPers(&arm->edges[1], &arm->edge_projected[1], &p, &flag);
                dx = (s16)arm->projected[1] - (s16)arm->projected[0];
                dy = (arm->projected[1] >> 16) - (arm->projected[0] >> 16);
                arm->angle[1] = ratan2(dy, dx) + 0xC00;
                arm->width[1] = (s16)arm->edge_projected[1] - (s16)arm->projected[1];
                arm->x_offset[1] = rcos(arm->angle[1]) * arm->width[1] >> 12;
                arm->y_offset[1] = rsin(arm->angle[1]) * arm->width[1] >> 12;
            } else {
                arm->depth[k] = RotTransPers4(&arm->points[k], &arm->points[k + 1], &arm->points[k], &arm->points[k + 1],
                                            &arm->projected[k], &arm->projected[k + 1], &arm->projected[k], &arm->projected[k + 1],
                                            &p, &status[i][k]);
                RotTransPers(&arm->edges[k], &arm->edge_projected[k], &p, &flag);
                dx = (s16)arm->projected[k + 1] - (s16)arm->projected[k];
                dy = (arm->projected[k + 1] >> 16) - (arm->projected[k] >> 16);
                arm->angle[k] = ratan2(dy, dx) + 0xC00;
                arm->width[k] = (s16)arm->edge_projected[k] - (s16)arm->projected[k];
                arm->x_offset[k] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                arm->y_offset[k] = rsin(arm->angle[k]) * arm->width[k] >> 12;
            }
        }
    }
    arm = (ModelVariantShadedRay *)(work + 0x84C);
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 1; k++) {
            poly->x0 = arm->projected[k] + arm->x_offset[k];
            poly->y0 = (arm->projected[k] >> 16) + arm->y_offset[k];
            poly->x1 = arm->projected[k + 1] + arm->x_offset[k + 1];
            poly->y1 = (arm->projected[k + 1] >> 16) + arm->y_offset[k + 1];
            poly->x2 = arm->projected[k];
            poly->y2 = arm->projected[k] >> 16;
            poly->x3 = arm->projected[k + 1];
            poly->y3 = arm->projected[k + 1] >> 16;
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
            if (arm->depth[k] >= 0 && status[i][k] >= 0) {
                GsSortPoly(poly, ot, arm->depth[k]);
            }
            poly->x0 = arm->projected[k] - arm->x_offset[k];
            poly->y0 = (arm->projected[k] >> 16) - arm->y_offset[k];
            poly->x1 = arm->projected[k + 1] - arm->x_offset[k + 1];
            poly->y1 = (arm->projected[k + 1] >> 16) - arm->y_offset[k + 1];
            poly->x2 = arm->projected[k];
            poly->y2 = arm->projected[k] >> 16;
            poly->x3 = arm->projected[k + 1];
            poly->y3 = arm->projected[k + 1] >> 16;
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
            if (arm->depth[k] >= 0 && status[i][k] >= 0) {
                GsSortPoly(poly, ot, arm->depth[k]);
            }
        }
    }
    }
    if (MODEL_VARIANT_HALF(work, 0x17DC) < 0x400) {
        MODEL_VARIANT_HALF(work, 0x17DC) += MODEL_VARIANT_WORD(work, 0x1794) * 64;
        if (MODEL_VARIANT_HALF(work, 0x17DC) >= 0x400) {
            MODEL_VARIANT_HALF(work, 0x17DC) = 0x400;
            if (MODEL_VARIANT_WORD(work, 0x180C) == 1) {
                MODEL_VARIANT_WORD(work, 0x180C) = 2;
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x180C) >= 2) {
        MODEL_VARIANT_HALF(work, 0x17D8) += 48;
    }
}
