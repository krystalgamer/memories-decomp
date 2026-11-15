#include "../../types.h"

#include "variant438_entry.h"

/* Phase-guarded twelve-arm spiral with an eight-entry palette: while the phase
 * is not negative, the arms are built on a spiral of half the size, moved along
 * the view by the spread, and drawn as two POLY_GT4 halves whose inner and outer
 * colours follow (flags + arm) & 7. The sweep advances by 16 a frame and the
 * size grows to 0x400 once the timing record's bound is reached. */
void SpanishModelVariant438_DrawSpiral(Variant438EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long status[12][2];
    long p;
    long flag;
    Variant438EntryState *work;
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

    work = context;
    ot = Graphics_GetDefaultOrderingTable();
    poly = work->packets.textured;
    turn = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 0xC00;
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
    arm = work->glows;
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
    arm = work->glows;
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = work->aim.target.vx;
    m.t[1] = work->aim.target.vy;
    m.t[2] = work->aim.target.vz;
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
    arm = work->glows;
    for (i = 0; i < 12; i++, arm++) {
        u8 r0;
        u8 g0;
        u8 b0;
        u8 r2;
        u8 g2;
        u8 b2;

        if (((work->clock.frame_count + i) & 7) == 0) {
            r0 = 128;
            g0 = 0;
            b0 = 0;
            r2 = 192;
            g2 = 128;
            b2 = 128;
        } else if (((work->clock.frame_count + i) & 7) == 1) {
            r0 = 128;
            g0 = 64;
            b0 = 0;
            r2 = 192;
            g2 = 160;
            b2 = 128;
        } else if (((work->clock.frame_count + i) & 7) == 2) {
            r0 = 128;
            g0 = 128;
            b0 = 0;
            r2 = 192;
            g2 = 192;
            b2 = 128;
        } else if (((work->clock.frame_count + i) & 7) == 3) {
            r0 = 0;
            g0 = 128;
            b0 = 0;
            r2 = 128;
            g2 = 192;
            b2 = 128;
        } else if (((work->clock.frame_count + i) & 7) == 4) {
            r0 = 0;
            g0 = 128;
            b0 = 128;
            r2 = 128;
            g2 = 192;
            b2 = 192;
        } else if (((work->clock.frame_count + i) & 7) == 5) {
            r0 = 0;
            g0 = 0;
            b0 = 128;
            r2 = 128;
            g2 = 128;
            b2 = 192;
        } else if (((work->clock.frame_count + i) & 7) == 6) {
            r0 = 128;
            g0 = 0;
            b0 = 128;
            r2 = 192;
            g2 = 128;
            b2 = 192;
        } else if (((work->clock.frame_count + i) & 7) == 7) {
            r0 = 128;
            g0 = 0;
            b0 = 64;
            r2 = 192;
            g2 = 128;
            b2 = 160;
        }
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
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = 0;
                poly->g3 = 0;
                poly->b3 = 0;
            } else {
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = r0;
                poly->g1 = g0;
                poly->b1 = b0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = r2;
                poly->g3 = g2;
                poly->b3 = b2;
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
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = 0;
                poly->g1 = 0;
                poly->b1 = 0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = 0;
                poly->g3 = 0;
                poly->b3 = 0;
            } else {
                poly->r0 = r0;
                poly->g0 = g0;
                poly->b0 = b0;
                poly->r1 = r0;
                poly->g1 = g0;
                poly->b1 = b0;
                poly->r2 = r2;
                poly->g2 = g2;
                poly->b2 = b2;
                poly->r3 = r2;
                poly->g3 = g2;
                poly->b3 = b2;
            }
            if (arm->depth[k] >= 0 && status[i][k] >= 0) {
                GsSortPoly(poly, ot, arm->depth[k]);
            }
        }
    }
    }
    work->spiral.angle += 16;
    if (work->config->field_20 <= work->clock.frame &&
        work->field_22EC == 0 && work->spiral.size < 0x400) {
        work->spiral.size += work->clock.step * 64;
        if (work->spiral.size >= 0x400) {
            work->spiral.size = 0x400;
            work->field_22EC = 2;
        }
    }
}
