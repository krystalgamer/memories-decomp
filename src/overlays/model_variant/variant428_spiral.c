#include "../../types.h"

#include "variant428_spiral.h"
#include "variant428_entry.h"

/* Header 428's form of the header-418 spiral: twelve arms on a spiral whose
 * radius is half the size at (u8 *)work + 0x15A8, drawn only where both the depth
 * and the projection flag are not negative; the size grows to 0x400.
 * The two products tested before the arms are template code with dead arms. */
void ModelVariant428_DrawSpiral(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant428EntryState *work;
    GsOT *ot;
    s16 i;
    s32 phi;
    s32 r;
    s32 eighth;
    s32 unused0;
    s32 unused1;
    s32 turn;
    ModelVariantShadedRay *arm;
    POLY_GT4 *poly;
    s16 k;
    s32 theta;
    s16 length;
    s16 size;
    long flags[12][2];
    s32 dx;
    s32 dy;

    work = (Variant428EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    turn = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 0xC00;
    poly = work->packets.textured;
    r = work->spiral_size / 2;
    eighth = work->spiral_size / 8;
    if (r * work->field_15A6 < 0) {
        length = 0;
    }
    if (eighth * work->field_15A6 < 0) {
        length = 0;
    }
    arm = (ModelVariantShadedRay *)work;
    for (i = 0; i < 12; i++, arm++) {
        phi = (i << 12) / 12 + work->field_15A4 * 2;
        theta = (i << 13) / 12 + work->field_15A4;
        for (k = 0; k < 2; k++) {
            if (k == 0) {
                length = work->scale / 512;
            } else {
                length = work->scale * 24 / 4096;
            }
            if (k == 0) {
                setVector(&arm->points[0], 0, 0, 0);
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
    arm = (ModelVariantShadedRay *)work;
    for (i = 0; i < 12; i++, arm++) {
        for (k = 0; k < 2; k++) {
            if (k == 1) {
                arm->depth[1] = RotTransPers4(&arm->points[0], &arm->points[1], &arm->points[0], &arm->points[1],
                                            &arm->projected[0], &arm->projected[1], &arm->projected[0], &arm->projected[1], &p, &flags[i][1]);
                RotTransPers(&arm->edges[1], &arm->edge_projected[1], &p, &flag);
                dx = (s16)arm->projected[1] - (s16)arm->projected[0];
                dy = (arm->projected[1] >> 16) - (arm->projected[0] >> 16);
                arm->angle[1] = ratan2(dy, dx) + 0xC00;
                arm->width[1] = (s16)arm->edge_projected[1] - (s16)arm->projected[1];
                arm->x_offset[1] = rcos(arm->angle[1]) * arm->width[1] >> 12;
                arm->y_offset[1] = rsin(arm->angle[1]) * arm->width[1] >> 12;
            } else {
                arm->depth[k] = RotTransPers4(&arm->points[k], &arm->points[k + 1], &arm->points[k], &arm->points[k + 1],
                                            &arm->projected[k], &arm->projected[k + 1], &arm->projected[k], &arm->projected[k + 1], &p, &flags[i][k]);
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
    arm = (ModelVariantShadedRay *)work;
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
            if (arm->depth[k] >= 0 && flags[i][k] >= 0) {
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
            if (arm->depth[k] >= 0 && flags[i][k] >= 0) {
                GsSortPoly(poly, ot, arm->depth[k]);
            }
        }
    }
    if (work->spiral_size < 0x400) {
        work->spiral_size += work->clock.step * 64;
        if (work->spiral_size >= 0x400) {
            work->spiral_size = 0x400;
        }
    }
}
