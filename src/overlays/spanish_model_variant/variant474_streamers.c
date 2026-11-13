#include "../../types.h"

#include "../french_model_variant/variant474_entry.h"

/* The North American header-457 copy, built with the CDK compiler, reloads
 * the word after the advanced rotation into itself like header 458 and indexes
 * the closing segment's previous point directly instead of walking a pointer
 * to it. */
#ifdef MODEL_VARIANT474_STREAMERS_DIRECT_PREVIOUS
#define STREAMERS474_PREVIOUS (&streamer->sa[15])
#else
#define STREAMERS474_PREVIOUS previous
#endif

/* Builds two seventeen-point streamers that coil around the variant's axis,
 * projects each spine and a copy of it moved along the view, and draws each
 * segment as a POLY_G4 as wide as the projected offset. From the fifth phase
 * on, the streamer length is reset from the entry's mesh_intensity. */
void SpanishModelVariant474_DrawCoiledStreamers(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
#ifndef MODEL_VARIANT474_STREAMERS_DIRECT_PREVIOUS
    long *previous;
#endif
    Variant474EntryState *work;
    POLY_G4 *poly;
    GsOT *ot;
    s16 i;
    s16 size;
    s16 rx;
    s16 ry;
    s32 step;
    s32 base;
    s32 spin;
    s32 wave;
    s32 turn;
    s32 radius;
    Variant458Streamer *streamer;
    s16 k;
    s32 twist;
    s32 bulge;
    s32 length;
    s32 wobble;
    s32 phase;
    s16 reach;
    s32 dx;
    s32 dy;

    work = (Variant474EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    poly = &work->flat.g4;
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    reach = 0x140;
    step = 0x400;
    if (!(work->clock.frame_count & 1)) {
        size = 0x1000;
        rx = 0x200;
        ry = 0;
    } else {
        size = 0x1000;
        rx = 0x600;
        ry = step;
    }
    if ((work->clock.frame_count & 3) < 2) {
        phase = 0;
    } else {
        phase = 0x5DC;
    }
    streamer = work->streamers;
    i = 0;
    base = work->streamer.rotation_y;
    spin = phase;
    radius = reach;
    for (; i < 2; i++, streamer++, base += step) {
        twist = base * 2;
        for (k = 0, wave = 0; k < 17;
             k++, twist = base * 2 + k * 1500 / 16, spin += 0x5DC, wave += 0x200) {
            bulge = rcos(wave) * 16 >> 12;
            wobble = bulge + (rcos(spin) * 8 >> 12);
            length = work->streamer.length / 64;
            setVector(&streamer->a[k], bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                      bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                      rsin(twist) * (radius + (s16)wobble) >> 12);
            setVector(&streamer->b[k], streamer->a[k].vx + (rcos(turn) * (s16)length >> 12), streamer->a[k].vy,
                      streamer->a[k].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    rot.vx = work->streamer.rotation_x + rx;
    rot.vy = work->streamer.rotation_y + ry;
    rot.vz = work->streamer.rotation_z;
    if (work->clock.frame_count % 10 == 0) {
        work->streamer.rotation_y += 0x514;
    }
    work->streamer.rotation_y += 0x20;
#ifdef MODEL_VARIANT474_STREAMERS_DIRECT_PREVIOUS
    work->streamer.rotation_z = work->streamer.rotation_z;
#endif
    m.t[0] = work->target.vx;
    m.t[1] = work->target.vy;
    m.t[2] = work->target.vz;
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
    streamer = work->streamers;
#ifdef MODEL_VARIANT474_STREAMERS_DIRECT_PREVIOUS
    for (i = 0; i < 2; i++, streamer++) {
#else
    for (i = 0, previous = &streamer->sa[15]; i < 2; i++,
         previous = (long *)((u8 *)previous + sizeof(Variant458Streamer)), streamer++) {
#endif
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                streamer->otz[k] = RotTransPers4(&streamer->a[15], &streamer->a[16], &streamer->a[15], &streamer->a[16],
                                                  STREAMERS474_PREVIOUS, &streamer->sa[16],
                                                  STREAMERS474_PREVIOUS, &streamer->sa[16], &p, &flag);
                RotTransPers(&streamer->b[16], &streamer->sb[16], &p, &flag);
                dx = (s16)streamer->sa[k] - (s16)*STREAMERS474_PREVIOUS;
                dy = (streamer->sa[k] >> 16) - (*STREAMERS474_PREVIOUS >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                if (work->streamer.length > 0x200) {
                    streamer->width[k] = 2;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            } else {
                streamer->otz[k] = RotTransPers4(&streamer->a[k], &streamer->a[k + 1], &streamer->a[k], &streamer->a[k + 1],
                                                 &streamer->sa[k], &streamer->sa[k + 1],
                                                 &streamer->sa[k], &streamer->sa[k + 1], &p, &flag);
                RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                dx = (s16)streamer->sa[k + 1] - (s16)streamer->sa[k];
                dy = (streamer->sa[k + 1] >> 16) - (streamer->sa[k] >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                if (work->streamer.length > 0x200) {
                    streamer->width[k] = 2;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            }
        }
    }
    streamer = work->streamers;
    for (i = 0; i < 2; i++, streamer++) {
        for (k = 0; k < 16; k++) {
            poly->x0 = streamer->sa[k] + streamer->ox[k];
            poly->y0 = (streamer->sa[k] >> 16) + streamer->oy[k];
            poly->x1 = streamer->sa[k + 1] + streamer->ox[k + 1];
            poly->y1 = (streamer->sa[k + 1] >> 16) + streamer->oy[k + 1];
            poly->x2 = streamer->sa[k] - streamer->ox[k];
            poly->y2 = (streamer->sa[k] >> 16) - streamer->oy[k];
            poly->x3 = streamer->sa[k + 1] - streamer->ox[k + 1];
            poly->y3 = (streamer->sa[k + 1] >> 16) - streamer->oy[k + 1];
            poly->r0 = streamer->color[k][0];
            poly->g0 = streamer->color[k][1];
            poly->b0 = streamer->color[k][2];
            if (streamer->otz[k] > 0) {
                GsSortPoly(poly, ot, streamer->otz[k]);
            }
        }
    }
    if (work->phase >= 5) {
        work->streamer.length = work->mesh_intensity;
    }
}
