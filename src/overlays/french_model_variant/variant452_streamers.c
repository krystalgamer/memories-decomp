#include "../../types.h"
#include "variant452_streamers.h"

void func_8013C51C(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[16][5];
    PSXLONG p;
    PSXLONG flag;
    Variant452StreamersView *work;
    ModelVariantSheet *sheet;
    GsOT *ot;
    s16 i;
    s32 longitude;
    s32 radius, length;
    s32 turn;
    Variant452Streamer *streamer;
    Variant452Streamer *point;
    SVECTOR *previous;
    SVECTOR *last;
    POLY_GT4 *poly;
    s16 j;
    s32 latitude;
    s16 width;
    s16 matrix_scale;
    s32 dx, dy;

    streamer = (Variant452Streamer *)(ctx + 0x720);
    work = (Variant452StreamersView *)ctx;
    ot = func_80058F10();
    turn = ratan2(work->delta.vz, work->delta.vx);
    sheet = &work->sheet;
    turn += 3072;
    poly = &work->poly;
    radius = work->radius / 8;
    length = work->field336C / 8;
    if (radius * work->field336A < 0) {
        width = 0;
    }
    if (length * work->field336A < 0) {
        width = 0;
    }
    for (i = 0; i < 16; i++, streamer++) {
        if (!(i & 1)) {
            longitude = i * 256 + work->rotation;
            latitude = i * 512 + work->rotation;
        } else {
            longitude = i * 256 - work->rotation;
            latitude = i * 512 + work->rotation;
        }
        for (j = 0; j < 5; j++) {
            if (!(work->flags & 1)) {
                width = (j + 1) * sheet->size / 4096;
            } else {
                width = (j * 2 + 2) * sheet->size / 4096;
            }
            if (j == 0) {
                setVector(&streamer->a[j], 0, 0, 0);
            } else {
                (j + streamer->a)->vx =
                    rcos(longitude) * (rsin(latitude) * (radius * j / 4) >> 12) >> 12;
                (j + streamer->a)->vy = rcos(latitude) * (radius * j / 4) >> 12;
                (j + streamer->a)->vz =
                    rsin(longitude) * (rsin(latitude) * (radius * j / 4) >> 12) >> 12;
            }
            (j + streamer->b)->vx = streamer->a[j].vx + (rcos(turn) * width >> 12);
            (j + streamer->b)->vy = streamer->a[j].vy;
            (j + streamer->b)->vz = streamer->a[j].vz + (rsin(turn) * width >> 12);
        }
    }
    if (work->phase < 3) {
        if (!(work->flags & 1)) {
            matrix_scale = 4096;
        } else {
            matrix_scale = 4608;
        }
    } else {
        if (!(work->flags & 1)) {
            matrix_scale = 8192;
        } else {
            matrix_scale = 8704;
        }
    }
    setVector(&rot, 0, 0, 0);
    m.t[0] = work->translation[0];
    m.t[1] = work->translation[1];
    m.t[2] = work->translation[2];
    setVector(&scale, matrix_scale, matrix_scale, matrix_scale);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    streamer = work->streamers;
    for (i = 0; i < 16; i++, streamer++) {
        for (j = 0; j < 5; j++) {
            if (j == 4) {
                previous = &streamer->a[3];
                last = &streamer->a[4];
                point = (Variant452Streamer *)((s32 *)streamer + 4);
                point->depth[0] = RotTransPers4(
                    previous, last, previous, last,
                    &streamer->sa[3], &streamer->sa[4], &streamer->sa[3], &streamer->sa[4],
                    &p, &flags[i][4]);
                RotTransPers(&streamer->b[4], &streamer->sb[4], &p, &flag);
                dx = (s16)point->sa[0] - (s16)streamer->sa[3];
                dy = (point->sa[0] >> 16) - (streamer->sa[3] >> 16);
                point->angle[0] = ratan2(dy, dx) + 3072;
                point->width[0] = (s16)point->sb[0] - (s16)point->sa[0];
                streamer->ox[4] = rcos(point->angle[0]) * point->width[0] >> 12;
                streamer->oy[4] = rsin(point->angle[0]) * point->width[0] >> 12;
            } else {
                streamer->depth[j] = RotTransPers4(
                    &streamer->a[j], &streamer->a[j + 1], &streamer->a[j], &streamer->a[j + 1],
                    &streamer->sa[j], &streamer->sa[j + 1], &streamer->sa[j], &streamer->sa[j + 1],
                    &p, &flags[i][j]);
                RotTransPers(&streamer->b[j], &streamer->sb[j], &p, &flag);
                dx = (s16)streamer->sa[j + 1] - (s16)streamer->sa[j];
                dy = (streamer->sa[j + 1] >> 16) - (streamer->sa[j] >> 16);
                streamer->angle[j] = ratan2(dy, dx) + 3072;
                streamer->width[j] = (s16)streamer->sb[j] - (s16)streamer->sa[j];
                streamer->ox[j] = rcos(streamer->angle[j]) * streamer->width[j] >> 12;
                streamer->oy[j] = rsin(streamer->angle[j]) * streamer->width[j] >> 12;
            }
        }
    }
    streamer = work->streamers;
    for (i = 0; i < 16; i++, streamer++) {
        for (j = 0; j < 4; j++) {
            poly->x0 = streamer->sa[j] + streamer->ox[j];
            poly->y0 = (streamer->sa[j] >> 16) + streamer->oy[j];
            poly->x1 = streamer->sa[j + 1] + streamer->ox[j + 1];
            poly->y1 = (streamer->sa[j + 1] >> 16) + streamer->oy[j + 1];
            poly->x2 = streamer->sa[j];
            poly->y2 = streamer->sa[j] >> 16;
            poly->x3 = streamer->sa[j + 1];
            poly->y3 = streamer->sa[j + 1] >> 16;
            poly->r0 = streamer->outer[j].r;
            poly->g0 = streamer->outer[j].g;
            poly->b0 = streamer->outer[j].b;
            poly->r1 = streamer->outer[j + 1].r;
            poly->g1 = streamer->outer[j + 1].g;
            poly->b1 = streamer->outer[j + 1].b;
            poly->r2 = streamer->inner[j].r;
            poly->g2 = streamer->inner[j].g;
            poly->b2 = streamer->inner[j].b;
            poly->r3 = streamer->inner[j + 1].r;
            poly->g3 = streamer->inner[j + 1].g;
            poly->b3 = streamer->inner[j + 1].b;
            if (streamer->depth[j] < 0) {
                streamer->depth[j] = 0;
            }
            flags[i][j] = 0;
            if (streamer->depth[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(poly, ot, streamer->depth[j]);
            }
            poly->x0 = streamer->sa[j] - streamer->ox[j];
            poly->y0 = (streamer->sa[j] >> 16) - streamer->oy[j];
            poly->x1 = streamer->sa[j + 1] - streamer->ox[j + 1];
            poly->y1 = (streamer->sa[j + 1] >> 16) - streamer->oy[j + 1];
            poly->x2 = streamer->sa[j];
            poly->y2 = streamer->sa[j] >> 16;
            poly->x3 = streamer->sa[j + 1];
            poly->y3 = streamer->sa[j + 1] >> 16;
            poly->r0 = streamer->outer[j].r;
            poly->g0 = streamer->outer[j].g;
            poly->b0 = streamer->outer[j].b;
            poly->r1 = streamer->outer[j + 1].r;
            poly->g1 = streamer->outer[j + 1].g;
            poly->b1 = streamer->outer[j + 1].b;
            poly->r2 = streamer->inner[j].r;
            poly->g2 = streamer->inner[j].g;
            poly->b2 = streamer->inner[j].b;
            poly->r3 = streamer->inner[j + 1].r;
            poly->g3 = streamer->inner[j + 1].g;
            poly->b3 = streamer->inner[j + 1].b;
            if (streamer->depth[j] < 0) {
                streamer->depth[j] = 0;
            }
            flags[i][j] = 0;
            if (streamer->depth[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(poly, ot, streamer->depth[j]);
            }
        }
    }
    if (work->radius < 1024 && work->phase < 2) {
        work->radius += work->step << 5;
        if (work->radius >= 1024) {
            work->radius = 1024;
        }
    } else if (work->radius < 2048 && work->phase >= 2) {
        work->radius += work->step << 7;
        if (work->radius >= 2048) {
            work->radius = 2048;
        }
    }
    if (work->phase >= 2) {
        work->rotation += 48;
    } else {
        work->rotation += 32;
    }
}
