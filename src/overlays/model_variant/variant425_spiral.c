#include "../../types.h"

#include "variant425_entry.h"

/* Header 425's form of the header-418 spiral: the sixteen two-point arms in
 * arms, fanned on a sphere of radius 0x200 at angles i/16 of a turn plus
 * the sweep at spiral_angle, each with a copy moved along the view
 * by (0x400 - field_2710) / 128 or / 32, projected and drawn as two POLY_GT4
 * halves in fixed yellow shades, only where the depth and the projection
 * flag are not negative. The scale comes from the timing record's size,
 * shrunk by spiral_shrink; from phase 2 on that grows by step * 128 up to
 * 0x800. The sweep advances by 16 a frame. The two products tested before the
 * arms are template code with dead arms. */
void ModelVariant425_DrawSpiral(Variant425EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long status[16][2];
    long p;
    long flag;
    Variant425EntryState *work;
    ModelVariantSheet *timing;
    GsOT *ot;
    s16 i;
    s32 phi;
    s32 eighth;
    s32 r;
    s32 turn;
    ModelVariantShadedRay *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 spread;
    s32 theta;
    s16 length;
    s16 size;
    s32 dx;
    s32 dy;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;

    arm = context->arms;
    work = context;
    ot = Graphics_GetDefaultOrderingTable();
    spread = 0x400 - work->field_2710;
#if defined(VERSION_FRENCH)
    turn = ratan2(work->view_delta.vz, work->view_delta.vx);
    timing = work->sheets;
    turn += 0xC00;
#else
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    timing = work->sheets;
#endif
    poly = work->packets.textured;
    r = 0x200;
    eighth = work->field_270C / 8;
    if (r * work->field_270A < 0) {
        length = 0;
    }
    if (eighth * work->field_270A < 0) {
        length = 0;
    }
    for (i = 0; i < 16; i++, arm++) {
        phi = (i << 8) + work->spiral_angle * 2;
        theta = (i << 9) + work->spiral_angle;
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
        size = timing->size * (0x1000 - work->spiral_shrink) / 4096;
    } else {
        size = timing->size * (0x1000 - work->spiral_shrink) / 4096 +
               timing->size / 8;
    }
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = work->orbit.vx;
    m.t[1] = work->orbit.vy;
    m.t[2] = work->orbit.vz;
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
    for (i = 0; i < 16; i++, arm++) {
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
    c0 = 0x40;
    c1 = 0;
    c2 = 0xA0;
    c3 = 0x80;
    arm = work->arms;
    for (i = 0; i < 16; i++, arm++) {
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
            if (k + 1 == 1) {
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
            if (arm->depth[k] >= 0 && status[i][k] >= 0) {
                GsSortPoly(poly, ot, arm->depth[k]);
            }
        }
    }
    if (work->spiral_shrink < 0x800 && work->phase >= 2) {
        work->spiral_shrink += work->clock.step * 128;
        if (work->spiral_shrink >= 0x800) {
            work->spiral_shrink = 0x800;
        }
    }
    work->spiral_angle += 16;
}
