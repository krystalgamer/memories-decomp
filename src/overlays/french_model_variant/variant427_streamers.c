#include "../../types.h"

#include "variant427_streamers.h"
#include "variant427_entry.h"

/* Builds two seventeen-point streamers that coil around the variant's axis,
 * projects each spine and a copy of it moved along the view, and draws each
 * segment as a POLY_G4 as wide as the projected offset. */
void FrenchModelVariant427_DrawStreamers(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    EntryState427 *work;
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
    Streamer427 *streamer;
    s16 k;
    s32 twist;
    s32 bulge;
    s32 length;
    s32 wobble;
    s32 phase;
    s16 reach;
    s32 dx;
    s32 dy;

    work = (EntryState427 *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    poly = (POLY_G4 *)work->flat;
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    reach = 0xC0;
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
            setVector(&streamer->spine.a[k], bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                      bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                      rsin(twist) * (radius + (s16)wobble) >> 12);
            setVector(&streamer->spine.b[k], streamer->spine.a[k].vx + (rcos(turn) * (s16)length >> 12), streamer->spine.a[k].vy,
                      streamer->spine.a[k].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    rot.vx = (s16)work->streamer.rotation_x + rx;
    rot.vy = (s16)work->streamer.rotation_y + ry;
    rot.vz = (s16)work->streamer.rotation_z;
    if (work->clock.frame_count % 10 == 0) {
        work->streamer.rotation_y += 0x514;
    }
    work->streamer.rotation_y += 0x20;
    /* A zero step: the PAL build drops it, the USA build keeps its load. */
    work->streamer.rotation_z += 0;
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
    for (i = 0; i < 2; i++, streamer++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                streamer->offsets.otz[k] = RotTransPers4(&streamer->spine.a[15], &streamer->spine.a[16], &streamer->spine.a[15], &streamer->spine.a[16],
                                                  &streamer->spine.sa[15], &streamer->spine.sa[16],
                                                  &streamer->spine.sa[15], &streamer->spine.sa[16], &p, &flag);
                RotTransPers(&streamer->spine.b[16], &streamer->spine.sb[16], &p, &flag);
                dx = (s16)streamer->spine.sa[k] - (s16)streamer->spine.sa[15];
                dy = (streamer->spine.sa[k] >> 16) - (streamer->spine.sa[15] >> 16);
                streamer->spine.angle[k] = ratan2(dy, dx) + 0xC00;
                if (work->streamer.length > 0x200) {
                    streamer->spine.width[k] = 2;
                } else {
                    streamer->spine.width[k] = (s16)streamer->spine.sb[k] - (s16)streamer->spine.sa[k];
                }
                streamer->offsets.ox[k] = rcos(streamer->spine.angle[k]) * streamer->spine.width[k] >> 12;
                streamer->offsets.oy[k] = rsin(streamer->spine.angle[k]) * streamer->spine.width[k] >> 12;
            } else {
                streamer->offsets.otz[k] = RotTransPers4(&streamer->spine.a[k], &streamer->spine.a[k + 1], &streamer->spine.a[k], &streamer->spine.a[k + 1],
                                                 &streamer->spine.sa[k], &streamer->spine.sa[k + 1],
                                                 &streamer->spine.sa[k], &streamer->spine.sa[k + 1], &p, &flag);
                RotTransPers(&streamer->spine.b[k], &streamer->spine.sb[k], &p, &flag);
                dx = (s16)streamer->spine.sa[k + 1] - (s16)streamer->spine.sa[k];
                dy = (streamer->spine.sa[k + 1] >> 16) - (streamer->spine.sa[k] >> 16);
                streamer->spine.angle[k] = ratan2(dy, dx) + 0xC00;
                if (work->streamer.length > 0x200) {
                    streamer->spine.width[k] = 2;
                } else {
                    streamer->spine.width[k] = (s16)streamer->spine.sb[k] - (s16)streamer->spine.sa[k];
                }
                streamer->offsets.ox[k] = rcos(streamer->spine.angle[k]) * streamer->spine.width[k] >> 12;
                streamer->offsets.oy[k] = rsin(streamer->spine.angle[k]) * streamer->spine.width[k] >> 12;
            }
        }
    }
    streamer = work->streamers;
    for (i = 0; i < 2; i++, streamer++) {
        for (k = 0; k < 16; k++) {
            poly->x0 = streamer->spine.sa[k] + streamer->offsets.ox[k];
            poly->y0 = (streamer->spine.sa[k] >> 16) + streamer->offsets.oy[k];
            poly->x1 = streamer->spine.sa[k + 1] + streamer->offsets.ox[k + 1];
            poly->y1 = (streamer->spine.sa[k + 1] >> 16) + streamer->offsets.oy[k + 1];
            poly->x2 = streamer->spine.sa[k] - streamer->offsets.ox[k];
            poly->y2 = (streamer->spine.sa[k] >> 16) - streamer->offsets.oy[k];
            poly->x3 = streamer->spine.sa[k + 1] - streamer->offsets.ox[k + 1];
            poly->y3 = (streamer->spine.sa[k + 1] >> 16) - streamer->offsets.oy[k + 1];
            poly->r0 = streamer->color[k][0];
            poly->g0 = streamer->color[k][1];
            poly->b0 = streamer->color[k][2];
            if (streamer->offsets.otz[k] > 0) {
                GsSortPoly(poly, ot, streamer->offsets.otz[k]);
            }
        }
    }
}
