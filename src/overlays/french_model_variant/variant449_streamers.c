#include "../../types.h"
#include "variant449_streamers.h"

void func_8013CB9C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[3][13];
    PSXLONG p;
    PSXLONG flag;
    Variant449StreamerView *work;
    POLY_FT4 *poly;
    GsOT *ot;
    s16 i;
    s16 rx;
    s16 ry;
    s32 base;
    s32 spin;
    s32 wave;
    s32 turn;
    s16 j;
    s32 radius;
    Variant449Streamer *streamer;
    s16 k;
    s32 twist;
    s32 bulge;
    s32 length;
    s32 wobble;
    s32 phase;
    s16 reach;
    s32 dx;
    s32 dy;

    work = (Variant449StreamerView *)ctx;
    ot = func_80058F10();
    poly = work->quads;
    turn = ratan2(work->axis_z, work->axis_x) + 3072;
    reach = 144;
    if (!(work->flags & 1)) {
        rx = 512;
        ry = 0;
    } else {
        rx = 1536;
        ry = 1024;
    }
    if (!(work->flags & 3) || work->flags % 4 == 1) {
        phase = 0;
    } else {
        phase = 1500;
    }
    streamer = work->streamers;
    i = 0;
    base = work->rotation_y;
    spin = phase;
    radius = reach;
    for (; i < 3; i++, streamer++, base += 1024) {
        for (k = 0, wave = 0, twist = base * 2; k < 13;
             k++, twist = base * 2 + k * 125, spin += 1500, wave += 512) {
            bulge = rcos(wave) * 16 >> 12;
            wobble = bulge + (rcos(spin) * 8 >> 12);
            length = work->length / 64;
            setVector(&streamer->a[k],
                      bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                      bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                      rsin(twist) * (radius + (s16)wobble) >> 12);
            setVector(&streamer->b[k],
                      streamer->a[k].vx + (rcos(turn) * (s16)length >> 12),
                      streamer->a[k].vy,
                      streamer->a[k].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    rot.vx = (s16)work->rotation_x + rx;
    rot.vy = (s16)work->rotation_y + ry;
    rot.vz = (s16)work->rotation_z;
    if (work->flags % 10 == 0) {
        work->rotation_y += 1300;
    }
    work->rotation_y += 32;
    for (j = 0; j < 1; j++) {
        m.t[0] = work->origin.t[0] + work->delta.vx * work->factor / 1024;
        m.t[1] = work->origin.t[1] + work->delta.vy * work->factor / 1024;
        m.t[2] = work->origin.t[2] + work->delta.vz * work->factor / 1024;
        setVector(&scale, work->scale, work->scale, work->scale);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        streamer = work->streamers;
        for (i = 0; i < 3; i++, streamer++) {
            for (k = 0; k < 13; k++) {
                if (k == 12) {
                    streamer->depth[k] = RotTransPers4(
                        &streamer->a[11], &streamer->a[k], &streamer->a[11], &streamer->a[k],
                        &streamer->sa[11], &streamer->sa[k], &streamer->sa[11], &streamer->sa[k],
                        &p, &flags[i][k]);
                    RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                    dx = (s16)streamer->sa[k] - (s16)streamer->sa[11];
                    dy = (streamer->sa[k] >> 16) - (streamer->sa[11] >> 16);
                    streamer->angle[k] = ratan2(dy, dx) + 3072;
                    if (work->length > 512) {
                        streamer->width[k] = 2;
                    } else {
                        streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                    }
                    streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                    streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
                } else {
                    streamer->depth[k] = RotTransPers4(
                        &streamer->a[k], &streamer->a[k + 1], &streamer->a[k], &streamer->a[k + 1],
                        &streamer->sa[k], &streamer->sa[k + 1], &streamer->sa[k], &streamer->sa[k + 1],
                        &p, &flags[i][k]);
                    RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                    dx = (s16)streamer->sa[k + 1] - (s16)streamer->sa[k];
                    dy = (streamer->sa[k + 1] >> 16) - (streamer->sa[k] >> 16);
                    streamer->angle[k] = ratan2(dy, dx) + 3072;
                    if (work->length > 512) {
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
        for (i = 0; i < 3; i++, streamer++) {
            for (k = 0; k < 12; k++) {
                poly->x0 = streamer->sa[k] + streamer->ox[k];
                poly->y0 = (streamer->sa[k] >> 16) + streamer->oy[k];
                poly->x1 = streamer->sa[k + 1] + streamer->ox[k + 1];
                poly->y1 = (streamer->sa[k + 1] >> 16) + streamer->oy[k + 1];
                poly->x2 = streamer->sa[k] - streamer->ox[k];
                poly->y2 = (streamer->sa[k] >> 16) - streamer->oy[k];
                poly->x3 = streamer->sa[k + 1] - streamer->ox[k + 1];
                poly->y3 = (streamer->sa[k + 1] >> 16) - streamer->oy[k + 1];
                poly->r0 = streamer->color[k].r;
                poly->g0 = streamer->color[k].g;
                poly->b0 = streamer->color[k].b;
                if (streamer->depth[k] >= 0 && flags[i][k] >= 0) {
                    GsSortPoly(poly, ot, streamer->depth[k]);
                }
            }
        }
    }
}
