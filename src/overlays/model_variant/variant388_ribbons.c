#include "../../types.h"

#include "variant388_entry_pal.h"

/* Builds eight two-point ribbons fanned 0x200 apart from
 * ribbon_angle, projects each spine and its sideways copy, and draws the
 * first point of each as a POLY_G3 as wide as the projected offset when
 * both its depth and its projection flag are non-negative. The fan turns
 * by step * 32 on the last frame of the timing record. */
void SpanishModelVariant388_DrawRibbonFan(Variant388EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant388EntryState *work;
    ModelVariantSheet *timing;
    GsOT *ot;
    s16 i;
    s32 turn;
    s32 tilt;
    Ribbon388 *ribbon;
    POLY_G3 *prim;
    s16 k;
    s16 angle;
    s16 length;
    s32 flags;
    s16 radius;
    s32 dx;
    s32 dy;

    work = context;
    ot = Graphics_GetDefaultOrderingTable();
    turn = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 0xC00;
    ratan2(work->aim.view_delta.vy, work->aim.view_delta.vx);
    ribbon = work->ribbons;
    tilt = ratan2(work->aim.direction.vz, work->aim.direction.vy) + 0x400;
    ratan2(work->aim.direction.vz, work->aim.direction.vx);
    timing = work->sheets;
    prim = &work->packets.triangle;
    if (work->slot == 1) {
        turn = -turn;
    }
    length = 16;
    flags = work->clock.frame_count;
    for (i = 0, angle = work->ribbon_angle; i < 8;
         i++, angle = work->ribbon_angle + i * 0x200, ribbon++) {
        for (k = 0; k < 2; k++) {
            radius = 0x96;
            if (k == 0) {
                radius = 0x28;
            }
            setVector(&ribbon->points[k], rcos(angle) * radius >> 12, rsin(angle) * radius >> 12, k * ((flags & 1) * 32 + 0xA0));
            setVector(&ribbon->edges[k], ribbon->points[k].vx + (rcos(turn) * length >> 12), ribbon->points[k].vy,
                      ribbon->points[k].vz + (rsin(turn) * length >> 12));
        }
    }
    rot.vx = tilt;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = work->matrix.t[0] + work->aim.direction.vx * work->progress / 1024;
    m.t[1] = work->matrix.t[1] + work->aim.direction.vy * work->progress / 1024;
    m.t[2] = work->matrix.t[2] + work->aim.direction.vz * work->progress / 1024;
    scale.vx = timing->size;
    scale.vy = timing->size;
    scale.vz = timing->size;
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    ribbon = work->ribbons;
    for (i = 0; i < 8; i++, ribbon++) {
        for (k = 0; k < 2; k++) {
            if (k == 0) {
                ribbon->otz[0] = RotTransPers4(&ribbon->points[0], &ribbon->points[1], &ribbon->points[0], &ribbon->points[1],
                                               &ribbon->screen[0], &ribbon->screen[1], &ribbon->screen[0], &ribbon->screen[1], &p,
                                               &ribbon->flag[0]);
                dx = (s16)ribbon->screen[k + 1] - (s16)ribbon->screen[k];
                dy = (ribbon->screen[k + 1] >> 16) - (ribbon->screen[k] >> 16);
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->points[k - 1], &ribbon->points[k], &ribbon->points[k - 1], &ribbon->points[k],
                                               &ribbon->screen[k - 1], &ribbon->screen[k], &ribbon->screen[k - 1], &ribbon->screen[k], &p,
                                               &ribbon->flag[k]);
                dx = (s16)ribbon->screen[k] - (s16)ribbon->screen[k - 1];
                dy = (ribbon->screen[k] >> 16) - (ribbon->screen[k - 1] >> 16);
            }
            RotTransPers(&ribbon->edges[k], &ribbon->edge_screen[k], &p, &flag);
            ribbon->angle[k] = ratan2(dy, dx) + 0xC00;
            ribbon->width[k] = (s16)ribbon->edge_screen[k] - (s16)ribbon->screen[k];
            ribbon->x_offset[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
            ribbon->y_offset[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
        }
    }
    ribbon = work->ribbons;
    for (i = 0; i < 8; i++, ribbon++) {
        for (k = 0; k < 1; k++) {
            prim->x0 = ribbon->screen[k] + ribbon->x_offset[k];
            prim->y0 = (ribbon->screen[k] >> 16) + ribbon->y_offset[k];
            prim->x1 = ribbon->screen[k + 1];
            prim->y1 = ribbon->screen[k + 1] >> 16;
            prim->x2 = ribbon->screen[k] - ribbon->x_offset[k];
            prim->y2 = (ribbon->screen[k] >> 16) - ribbon->y_offset[k];
            prim->r0 = 0x80;
            prim->g0 = 0x80;
            prim->b0 = 0x80;
            prim->r1 = 0xFF;
            prim->g1 = 0;
            prim->b1 = 0xFF;
            prim->r2 = 0x80;
            prim->g2 = 0x80;
            prim->b2 = 0x80;
            if (ribbon->otz[k] >= 0 && ribbon->flag[k] >= 0) {
                Graphics_SubmitDrawModePacket((u32 *)prim, ot, (u16)ribbon->otz[k], 1);
            }
        }
    }
    if (work->index + 1 == work->config->count) {
        work->ribbon_angle += work->clock.step << 5;
    }
}
