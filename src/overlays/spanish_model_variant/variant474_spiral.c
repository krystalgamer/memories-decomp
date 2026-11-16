#include "../../types.h"

#include "../french_model_variant/variant474_entry.h"

/* Header 474's phase-guarded spiral: while phase is not
 * negative, twelve two-point arms (records from spirals) on a spiral of
 * half spiral.size are moved along the view by the spread and
 * drawn as two POLY_GT4 halves where the depth is positive. The size grows
 * to 0x400; only then does the sweep spiral.angle advance by 32. */
void SpanishModelVariant474_DrawSpiral(Variant474EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant474EntryState *work;
    ModelVariantSheet *timing;
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
    u8 c0;
    u8 c1;
    u8 c3;

    work = context;
    ot = Graphics_GetDefaultOrderingTable();
    poly = work->packets.textured;
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    if (work->phase >= 0) {
    r = work->spiral.size / 2;
    eighth = work->spiral.size / 8;
    if (r * work->spiral.field_02 < 0) {
        length = 0;
    }
    if (eighth * work->spiral.field_02 < 0) {
        length = 0;
    }
    if (work->phase >= 3) {
        timing = &work->sheets[1];
        spread = timing->size / 8;
    } else {
        spread = 0x400 - work->spiral.shrink;
    }
    arm = work->spirals;
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + work->spiral.angle * 2;
        theta = (i << 13) / 12 + work->spiral.angle;
        for (k = 0; k < 2; k++) {
            if (k == 0) {
                length = spread / 128;
            } else {
                length = spread / 32;
            }
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
    if (!(work->clock.frame_count & 1)) {
        size = 0x1000;
    } else {
        size = 0x1200;
    }
    arm = work->spirals;
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
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 2; k++) {
            if (k == 1) {
                arm->depth[k] = RotTransPers4(&arm->points[0], &arm->points[1], &arm->points[0], &arm->points[1],
                                            &arm->projected[0], &arm->projected[1], &arm->projected[0], &arm->projected[1], &p, &flag);
                RotTransPers(&arm->edges[1], &arm->edge_projected[1], &p, &flag);
                dx = (s16)arm->projected[k] - (s16)arm->projected[0];
                dy = (arm->projected[k] >> 16) - (arm->projected[0] >> 16);
                arm->angle[k] = ratan2(dy, dx) + 0xC00;
                arm->width[k] = (s16)arm->edge_projected[k] - (s16)arm->projected[k];
                arm->x_offset[1] = rcos(arm->angle[k]) * arm->width[k] >> 12;
                arm->y_offset[1] = rsin(arm->angle[k]) * arm->width[k] >> 12;
            } else {
                arm->depth[k] = RotTransPers4(&arm->points[k], &arm->points[k + 1], &arm->points[k], &arm->points[k + 1],
                                            &arm->projected[k], &arm->projected[k + 1], &arm->projected[k], &arm->projected[k + 1],
                                            &p, &flag);
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
    c0 = 0;
    c1 = 0x80;
    c3 = 0xC0;
    arm = work->spirals;
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
            if (k + 1 == 1) {
                poly->r0 = c0;
                poly->g0 = c0;
                poly->b0 = c1;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = c1;
                poly->g2 = c1;
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
                poly->r2 = c1;
                poly->g2 = c1;
                poly->b2 = c3;
                poly->r3 = c1;
                poly->g3 = c1;
                poly->b3 = c3;
            }
            if (arm->depth[k] > 0) {
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
            if (k + 1 == 1) {
                poly->r0 = c0;
                poly->g0 = c0;
                poly->b0 = c1;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = c1;
                poly->g2 = c1;
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
                poly->r2 = c1;
                poly->g2 = c1;
                poly->b2 = c3;
                poly->r3 = c1;
                poly->g3 = c1;
                poly->b3 = c3;
            }
            if (arm->depth[k] > 0) {
                GsSortPoly(poly, ot, arm->depth[k]);
            }
        }
    }
    }
    if (work->spiral.size < 0x400) {
        work->spiral.size += work->clock.step * 64;
        if (work->spiral.size < 0x400) {
            return;
        }
        work->spiral.size = 0x400;
    }
    work->spiral.angle += 32;
}
