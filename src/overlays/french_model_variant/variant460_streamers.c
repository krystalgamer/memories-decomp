#include "../../types.h"
#include "../model_variant/variant443_streamers.h"
#include "variant460_entry.h"

void FrenchModelVariant460_DrawStreamers(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant460EntryState *work;
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
    Variant443Streamer *streamer;
    s16 k;
    s32 bulge;
    s32 length;
    s32 wobble;
    s32 phase;
    s16 reach;
    s32 dx;
    s32 dy;
    long *previous;

    work = (Variant460EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 3072;
    poly = (POLY_G4 *)work->flat_textured;
    reach = 160;
    step = 1024;
    if (!(work->clock.frame_count & 1)) {
        size = 4096;
        rx = 512;
        ry = 0;
    } else {
        size = 4096;
        rx = 1536;
        ry = step;
    }
    if ((work->clock.frame_count & 3) < 2) {
        phase = 0;
    } else {
        phase = 1500;
    }
    streamer = (Variant443Streamer *)work->streamers;
    i = 0;
    base = work->field_2EB4;
    spin = phase;
    radius = reach;
    for (; i < 2; i++, streamer++, base += step) {
        s32 twist;

        twist = base * 2;
        for (k = 0, wave = 0; k < 17;
             k++, twist = base * 2 + k * 1500 / 16, spin += 1500, wave += 512) {
            bulge = rcos(wave) * 16 >> 12;
            wobble = bulge + (rcos(spin) * 8 >> 12);
            length = work->field_2EAC / 64;
            setVector(&streamer->spine.a[k],
                bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                rsin(twist) * (radius + (s16)wobble) >> 12);
            setVector(&streamer->spine.b[k],
                streamer->spine.a[k].vx + (rcos(turn) * (s16)length >> 12), streamer->spine.a[k].vy,
                streamer->spine.a[k].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    rot.vx = (s16)work->field_2EB0 + rx;
    rot.vy = (s16)work->field_2EB4 + ry;
    rot.vz = (s16)work->field_2EB8;
    if (work->clock.frame_count % 10 == 0) {
        work->field_2EB4 += 1300;
    }
    work->field_2EB4 += 32;
    m.t[0] = work->target.vx;
    m.t[1] = work->target.vy;
    m.t[2] = work->target.vz;
    setVector(&scale, size, size, size);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    streamer = (Variant443Streamer *)work->streamers;
    for (i = 0, previous = &streamer->spine.sa[15]; i < 2; i++,
         previous = (long *)((u8 *)previous + sizeof(Variant443Streamer)), streamer++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                streamer->offsets.otz[k] = RotTransPers4(
                    &streamer->spine.a[15], &streamer->spine.a[16], &streamer->spine.a[15], &streamer->spine.a[16],
                    previous, &streamer->spine.sa[16], previous, &streamer->spine.sa[16], &p, &flag);
                RotTransPers(&streamer->spine.b[16], &streamer->spine.sb[16], &p, &flag);
                dx = (s16)streamer->spine.sa[k] - (s16)*previous;
                dy = (streamer->spine.sa[k] >> 16) - (*previous >> 16);
                streamer->spine.angle[k] = ratan2(dy, dx) + 3072;
                if (work->field_2EAC > 512) {
                    streamer->spine.width[k] = 2;
                } else {
                    streamer->spine.width[k] = (s16)streamer->spine.sb[k] - (s16)streamer->spine.sa[k];
                }
                streamer->offsets.ox[k] = rcos(streamer->spine.angle[k]) * streamer->spine.width[k] >> 12;
                streamer->offsets.oy[k] = rsin(streamer->spine.angle[k]) * streamer->spine.width[k] >> 12;
            } else {
                streamer->offsets.otz[k] = RotTransPers4(
                    &streamer->spine.a[k], &streamer->spine.a[k + 1], &streamer->spine.a[k], &streamer->spine.a[k + 1],
                    &streamer->spine.sa[k], &streamer->spine.sa[k + 1], &streamer->spine.sa[k], &streamer->spine.sa[k + 1], &p, &flag);
                RotTransPers(&streamer->spine.b[k], &streamer->spine.sb[k], &p, &flag);
                dx = (s16)streamer->spine.sa[k + 1] - (s16)streamer->spine.sa[k];
                dy = (streamer->spine.sa[k + 1] >> 16) - (streamer->spine.sa[k] >> 16);
                streamer->spine.angle[k] = ratan2(dy, dx) + 3072;
                if (work->field_2EAC > 512) {
                    streamer->spine.width[k] = 2;
                } else {
                    streamer->spine.width[k] = (s16)streamer->spine.sb[k] - (s16)streamer->spine.sa[k];
                }
                streamer->offsets.ox[k] = rcos(streamer->spine.angle[k]) * streamer->spine.width[k] >> 12;
                streamer->offsets.oy[k] = rsin(streamer->spine.angle[k]) * streamer->spine.width[k] >> 12;
            }
        }
    }
    streamer = (Variant443Streamer *)work->streamers;
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
            if (streamer->offsets.otz[k] >= 0) {
                if (streamer->offsets.otz[k] < 2048) {
                    GsSortPoly(poly, ot, (u16)streamer->offsets.otz[k]);
                }
            }
        }
    }
    if (work->tail.phase == 4 && work->field_2EAC > 0) {
        work->field_2EAC -= work->clock.step << 4;
        if (work->field_2EAC <= 0) {
            work->field_2EAC = 0;
            work->tail.phase = 5;
        }
    }
}
