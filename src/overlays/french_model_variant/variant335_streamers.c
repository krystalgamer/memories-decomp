#include "../../types.h"
#include "../model_variant/variant321_streamers.h"
#include "variant335_entry.h"

/* The North American header-318 build (gcc 2.7.2) loads the word after the
 * advanced rotation with a zero step and indexes the closing segment's
 * previous point directly instead of walking a pointer to it. */
#ifdef MODEL_VARIANT335_STREAMERS_DIRECT_PREVIOUS
#define STREAMERS335_PREVIOUS ((DVECTOR *)&streamer->sa[15])
#else
#define STREAMERS335_PREVIOUS previous
#endif

void FrenchModelVariant335_DrawStreamers(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Model335EntryState *work;
    POLY_FT4 *poly;
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
    ModelVariantFlaggedStreamer *streamer;
    s16 k;
    s32 twist;
    s32 bulge;
    s32 length;
    s32 wobble;
    s32 phase;
    s16 reach;
    s32 dx;
    s32 dy;
    DVECTOR *previous;

    work = (Model335EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    turn = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    poly = work->extra_textured;
    reach = 0x100;
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
    if ((u32)(work->clock.frame_count & 3) < 2) {
        phase = 0;
    } else {
        phase = 0x5DC;
    }
    streamer = (ModelVariantFlaggedStreamer *)work->streamers;
    i = 0;
    base = work->rotation[1];
    spin = phase;
    radius = reach;
    for (; i < 2; i++, streamer++, base += step) {
        for (k = 0, wave = 0, twist = base * 2; k < 17;
             k++, twist = base * 2 + k * 1500 / 16, spin += 0x5DC, wave += 0x200) {
            bulge = rcos(wave) * 16 >> 12;
            wobble = bulge + (rcos(spin) * 8 >> 12);
            length = work->extent / 64;
            setVector(&streamer->a[k], bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                      bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                      rsin(twist) * (radius + (s16)wobble) >> 12);
            setVector(&streamer->b[k], streamer->a[k].vx + (rcos(turn) * (s16)length >> 12), streamer->a[k].vy,
                      streamer->a[k].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    rot.vx = MODEL_VARIANT_HALF(work, 0x2E7C) + rx;
    rot.vy = MODEL_VARIANT_HALF(work, 0x2E80) + ry;
    rot.vz = MODEL_VARIANT_HALF(work, 0x2E84);
    if ((u32)work->clock.frame_count % 10 == 0) {
        work->rotation[1] += 0x514;
    }
    work->rotation[1] += 0x20;
#ifdef MODEL_VARIANT335_STREAMERS_DIRECT_PREVIOUS
    work->rotation[2] += 0;
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
    streamer = (ModelVariantFlaggedStreamer *)work->streamers;
#ifdef MODEL_VARIANT335_STREAMERS_DIRECT_PREVIOUS
    for (i = 0; i < 2; i++, streamer++) {
#else
    for (i = 0, previous = (DVECTOR *)&streamer->sa[15]; i < 2; i++,
         previous = (DVECTOR *)((u8 *)previous + sizeof(ModelVariantFlaggedStreamer)), streamer++) {
#endif
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                streamer->otz[k] = RotTransPers4(&streamer->a[15], &streamer->a[k], &streamer->a[15], &streamer->a[k],
                                               (long *)STREAMERS335_PREVIOUS, &streamer->sa[k],
                                               (long *)STREAMERS335_PREVIOUS, &streamer->sa[k], &p, &streamer->flag[k]);
                RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                dx = (s16)streamer->sa[k] - STREAMERS335_PREVIOUS->vx;
                dy = (streamer->sa[k] >> 16) - STREAMERS335_PREVIOUS->vy;
                streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                if (work->extent > 0x200) {
                    streamer->width[k] = 3;
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
                if (work->extent > 0x200) {
                    streamer->width[k] = 3;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            }
        }
    }
    streamer = (ModelVariantFlaggedStreamer *)work->streamers;
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
            if (streamer->otz[k] >= 0 && streamer->flag[k] >= 0) {
                GsSortPoly(poly, ot, streamer->otz[k]);
            }
        }
    }
    if (work->phase == 8 && work->extent > 0) {
        work->extent -= 16;
        if (work->extent <= 0) {
            work->extent = 0;
            work->phase = 9;
        }
    }
}
