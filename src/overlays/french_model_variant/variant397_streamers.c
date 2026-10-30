#include "../../types.h"
#include "variant397_streamers.h"

/* Header 397's streamers: for every configured record, two seventeen-point
 * streamers coil around the record's position with its own rotation, scale
 * and length, are projected with a view-shifted copy, and are drawn as
 * POLY_G4 strips. As in header 320's French streamers, each branch of the
 * last-point test computes the width offset itself; the copies merge after
 * register allocation. After phase 6 a record's length shrinks with the
 * frame step, and the last record finishing moves the phase on to 7. */

void func_8013D06C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    Variant397StreamerState *work;
    POLY_G4 *poly;
    GsOT *ot;
    s16 j;
    s16 i;
    s16 size;
    s16 rx;
    s16 ry;
    s16 phase;
    s32 spin;
    s32 wave;
    s32 turn;
    s32 radius;
    s32 base;
    s32 offset;
    Variant397Streamer *streamer;
    s16 k;
    s32 twist;
    s32 bulge;
    s32 length;
    s32 wobble;
    s32 dx;
    s32 dy;

    work = (Variant397StreamerState *)ctx;
    ot = func_80058F10();
    phase = 0x5DC;
    radius = 0x80;
    poly = (POLY_G4 *)((u8 *)work + 0x18C8);
    turn = ratan2(work->view_z, work->view_x) + 0xC00;
    if ((work->frame & 3) < 2) {
        phase = 0;
    }
    for (i = 0; i < work->timing->count; i++) {
        offset = (i << 12) / work->timing->count;
        if (!(work->frame & 1)) {
            size = offset + 0x1000;
            rx = offset + 0x200;
            ry = 0;
        } else {
            size = offset + 0x1000;
            rx = offset + 0x600;
            ry = 0x400;
        }
        j = 0;
        streamer = (Variant397Streamer *)((u8 *)work + 0x1130);
        base = work->ry[i];
        spin = phase;
        for (; j < 2; j++, streamer++, base += 0x400) {
            for (k = 0, twist = base * 2, wave = 0; k < 17;
                 k++, twist = base * 2 + k * 1500 / 16, spin += 0x5DC, wave += 0x200) {
                bulge = rcos(wave) * 16 >> 12;
                wobble = bulge + (rcos(spin) * 8 >> 12);
                length = work->length[i] / 128;
                setVector(&streamer->a[k], bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                          bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                          rsin(twist) * (radius + (s16)wobble) >> 12);
                setVector(&streamer->b[k], streamer->a[k].vx + (rcos(turn) * (s16)length >> 12), streamer->a[k].vy,
                          streamer->a[k].vz + (rsin(turn) * (s16)length >> 12));
            }
        }
        rot.vx = (s16)work->rx[i] + rx;
        rot.vy = (s16)work->ry[i] + ry;
        rot.vz = (s16)work->rz[i];
        if ((work->frame + i) % 8 == 0) {
            work->ry[i] += 0x514;
        }
        work->ry[i] += 0x20;
        work->rz[i] = work->rz[i];
        m.t[0] = work->positions[i].vx;
        m.t[1] = work->positions[i].vy;
        m.t[2] = work->positions[i].vz;
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
        streamer = (Variant397Streamer *)((u8 *)work + 0x1130);
        for (j = 0; j < 2; j++, streamer++) {
            for (k = 0; k < 17; k++) {
                if (k == 16) {
                    streamer->otz[k] = RotTransPers4(&streamer->a[15], &streamer->a[k], &streamer->a[15], &streamer->a[k],
                                                     &streamer->sa[15], &streamer->sa[k],
                                                     &streamer->sa[15], &streamer->sa[k], &p, &streamer->flag[k]);
                    RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                    dx = (s16)streamer->sa[k] - (s16)streamer->sa[15];
                    dy = (streamer->sa[k] >> 16) - (streamer->sa[15] >> 16);
                    streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                    if (work->length[i] > 0x200) {
                        streamer->width[k] = 2;
                    } else {
                        streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                    }
                    streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                    streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
                } else {
                    streamer->otz[k] = RotTransPers4(&streamer->a[k], &streamer->a[k + 1], &streamer->a[k], &streamer->a[k + 1],
                                                     &streamer->sa[k], &streamer->sa[k + 1],
                                                     &streamer->sa[k], &streamer->sa[k + 1], &p, &streamer->flag[k]);
                    RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                    dx = (s16)streamer->sa[k + 1] - (s16)streamer->sa[k];
                    dy = (streamer->sa[k + 1] >> 16) - (streamer->sa[k] >> 16);
                    streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                    if (work->length[i] > 0x200) {
                        streamer->width[k] = 3;
                    } else {
                        streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                    }
                    streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                    streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
                }
            }
        }
        streamer = (Variant397Streamer *)((u8 *)work + 0x1130);
        for (j = 0; j < 2; j++, streamer++) {
            for (k = 0; k < 16; k++) {
                poly->x0 = streamer->sa[k] + streamer->ox[k];
                poly->y0 = (streamer->sa[k] >> 16) + streamer->oy[k];
                poly->x1 = streamer->sa[k + 1] + streamer->ox[k + 1];
                poly->y1 = (streamer->sa[k + 1] >> 16) + streamer->oy[k + 1];
                poly->x2 = streamer->sa[k];
                poly->y2 = streamer->sa[k] >> 16;
                poly->x3 = streamer->sa[k + 1];
                poly->y3 = streamer->sa[k + 1] >> 16;
                poly->r0 = streamer->color[k][0];
                poly->g0 = streamer->color[k][1];
                poly->b0 = streamer->color[k][2];
                if (streamer->otz[k] >= 0 && streamer->flag[k] >= 0) {
                    GsSortPoly(poly, ot, streamer->otz[k]);
                }
            }
        }
        if (work->phase >= 6 && work->length[i] > 0) {
            work->length[i] -= work->step * 64;
            if (work->length[i] <= 0) {
                work->length[i] = 0;
                if (i + 1 == work->timing->count) {
                    work->phase = 7;
                }
            }
        }
    }
}
