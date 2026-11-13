#include "../../types.h"

#include "variant321_streamers.h"
#include "../french_model_variant/variant338_entry.h"

/* Builds two seventeen-point streamers that coil around the variant's axis,
 * projects each spine and a copy of it moved along the view, and draws each
 * segment as a POLY_G4 as wide as the projected offset. Once the fourth phase
 * starts, the streamers shrink by step * 16 until they vanish. */
void FrenchModelVariant338_DrawStreamers(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant338EntryState *work;
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
    ModelVariantFlaggedStreamer *streamer;
    s16 k;
    s32 twist;
    s32 bulge;
    s32 r;
    s32 length;
    s32 wobble;
    s32 w;
    s32 phase;
    s16 reach;
    s32 dx;
    s32 dy;

    work = (Variant338EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    poly = (POLY_G4 *)work->extra_textured;
    turn = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 0xC00;
    reach = work->rings[1].size / 32;
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
    streamer = (ModelVariantFlaggedStreamer *)work->streamers;
    i = 0;
    base = work->streamer_rotation_y;
    spin = phase;
    radius = reach;
    for (; i < 2; i++, streamer++, base += step) {
        for (k = 0, wave = 0, twist = base * 2; k < 17;
             k++, twist = base * 2 + k * 1500 / 16, spin += 0x5DC, wave += 0x200) {
            bulge = rcos(wave) * 16 >> 12;
            wobble = bulge + (rcos(spin) * 8 >> 12);
            length = work->streamer_length / 64;
            setVector(&streamer->a[k], bulge + (rcos(twist) * (rcos(base) * (radius + (s16)wobble) >> 12) >> 12),
                      bulge + (rcos(twist) * (rsin(base) * (radius + (s16)wobble) >> 12) >> 12),
                      rsin(twist) * (radius + (s16)wobble) >> 12);
            setVector(&streamer->b[k], streamer->a[k].vx + (rcos(turn) * (s16)length >> 12), streamer->a[k].vy,
                      streamer->a[k].vz + (rsin(turn) * (s16)length >> 12));
        }
    }
    rot.vx = (s16)work->streamer_rotation_x + rx;
    rot.vy = (s16)work->streamer_rotation_y + ry;
    rot.vz = (s16)work->streamer_rotation_z;
    if (work->clock.frame_count % 10 == 0) {
        work->streamer_rotation_y += 0x514;
    }
    work->streamer_rotation_y += 0x20;
    work->streamer_rotation_z = work->streamer_rotation_z;
    m.t[0] = work->aim.target.vx;
    m.t[1] = work->aim.target.vy;
    m.t[2] = work->aim.target.vz;
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
    for (i = 0; i < 2; i++, streamer++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                streamer->otz[16] = RotTransPers4(&streamer->a[15], &streamer->a[16], &streamer->a[15], &streamer->a[16],
                                                  &streamer->sa[15], &streamer->sa[16],
                                                  &streamer->sa[15], &streamer->sa[16], &p, &streamer->flag[16]);
                RotTransPers(&streamer->b[16], &streamer->sb[16], &p, &flag);
                dx = (s16)streamer->sa[16] - (s16)streamer->sa[15];
                dy = (streamer->sa[16] >> 16) - (streamer->sa[15] >> 16);
                streamer->angle[16] = ratan2(dy, dx) + 0xC00;
                if (work->streamer_length > 0x200) {
                    w = 3;
                } else {
                    r = (s16)streamer->sb[16];
                    dx = (s16)streamer->sa[16];
                    w = r - dx;
                }
                streamer->width[16] = w;
            } else {
                streamer->otz[k] = RotTransPers4(&streamer->a[k], &streamer->a[k + 1], &streamer->a[k], &streamer->a[k + 1],
                                                 &streamer->sa[k], &streamer->sa[k + 1],
                                                 &streamer->sa[k], &streamer->sa[k + 1], &p, &streamer->flag[k]);
                RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                dx = (s16)streamer->sa[k + 1] - (s16)streamer->sa[k];
                dy = (streamer->sa[k + 1] >> 16) - (streamer->sa[k] >> 16);
                streamer->angle[k] = ratan2(dy, dx) + 0xC00;
                if (work->streamer_length > 0x200) {
                    streamer->width[k] = 3;
                } else {
                    streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                }
            }
            if (work) {
                streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
            } else {
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
            if (streamer->otz[k] >= 0) {
                if (streamer->flag[k] >= 0) {
                    GsSortPoly(poly, ot, streamer->otz[k]);
                }
            }
        }
    }
    if (work->phase == 4 && work->streamer_length > 0) {
        work->streamer_length -= work->clock.step << 4;
        if (work->streamer_length <= 0) {
            work->streamer_length = 0;
            work->phase = 5;
        }
    }
}
