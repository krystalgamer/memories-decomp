#include "../../types.h"
#include "variant400_streamers.h"

void func_8013CDB8(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[2][17];
    PSXLONG p;
    PSXLONG flag;
    Model400FirstRibbon *ribbon;
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
    s16 count;
    s16 first;
    s32 radius;
    Model400Size *sizes;
    Model400Size *size;
    s32 n;
    u8 *work;
    Model400Streamer *streamer;
    s16 k;
    s32 twist;
    s32 bulge;
    s32 length;
    s32 wobble;
    s32 phase;
    s16 reach;
    s32 dx;
    s32 dy;

    work = ctx;
    ribbon = (Model400FirstRibbon *)work;
    ot = func_80058F10();
    poly = (POLY_FT4 *)(work + 0x1F80);
    turn = ratan2(MODEL_VARIANT_WORD(work, 0x2034), MODEL_VARIANT_WORD(work, 0x202C)) + 3072;
    reach = 128;
    if (!(MODEL_VARIANT_WORD(work, 0x2044) & 1)) {
        rx = 512;
        ry = 0;
    } else {
        rx = 1536;
        ry = 1024;
    }
    if ((u32)MODEL_VARIANT_WORD(work, 0x2044) % 4 < 2) {
        phase = 0;
    } else {
        phase = 1500;
    }
    streamer = (Model400Streamer *)(work + 0x1558);
    i = 0;
    base = MODEL_VARIANT_WORD(work, 0x2080);
    spin = phase;
    radius = reach;
    for (; i < 2; i++, streamer++, base += 1024) {
        for (k = 0, wave = 0, twist = base * 2; k < 17;
             k++, twist = base * 2 + k * 1500 / 16, spin += 1500, wave += 512) {
            bulge = rcos(wave) * 16 >> 12;
            wobble = bulge + (rcos(spin) * 8 >> 12);
            length = MODEL_VARIANT_WORD(work, 0x2078) / 64;
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
    rot.vx = (s16)MODEL_VARIANT_WORD(work, 0x207C) + rx;
    rot.vy = (s16)MODEL_VARIANT_WORD(work, 0x2080) + ry;
    rot.vz = (s16)MODEL_VARIANT_WORD(work, 0x2084);
    if ((u32)MODEL_VARIANT_WORD(work, 0x2044) % 10 == 0) {
        MODEL_VARIANT_WORD(work, 0x2080) += 1300;
    }
    MODEL_VARIANT_WORD(work, 0x2080) += 32;
    if (MODEL_VARIANT_WORD(work, 0x2090) < 6) {
        first = 0;
        count = 6;
    } else {
        first = 1;
        count = 7;
    }
    for (j = first, n = 0; j < count; j++, n++) {
        sizes = (Model400Size *)(work + 0x11B8);
        if (j == 0) {
            m.t[0] = MODEL_VARIANT_WORD(work, 0x1FF4);
            m.t[1] = MODEL_VARIANT_WORD(work, 0x1FF8);
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1FFC);
            setVector(&scale, sizes[n].scale, sizes[n].scale, sizes[n].scale);
        } else {
            size = &sizes[n];
            if (j < 6) {
                m.t[0] = ribbon->position.vx;
                m.t[1] = ribbon->position.vy;
                m.t[2] = ribbon->position.vz;
                setVector(&scale, size->scale, size->scale, size->scale);
                ribbon++;
            } else {
                m.t[0] = MODEL_VARIANT_HALF(work, 0x2000);
                m.t[1] = MODEL_VARIANT_HALF(work, 0x2002);
                m.t[2] = MODEL_VARIANT_HALF(work, 0x2004);
                setVector(&scale, 4096, 4096, 4096);
            }
        }
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        streamer = (Model400Streamer *)(work + 0x1558);
        for (i = 0; i < 2; i++, streamer++) {
            for (k = 0; k < 17; k++) {
                if (k == 16) {
                    streamer->depth[k] = RotTransPers4(
                        &streamer->a[15], &streamer->a[k], &streamer->a[15], &streamer->a[k],
                        &streamer->sa[15], &streamer->sa[k], &streamer->sa[15], &streamer->sa[k],
                        &p, &flags[i][k]);
                    RotTransPers(&streamer->b[k], &streamer->sb[k], &p, &flag);
                    dx = (s16)streamer->sa[k] - (s16)streamer->sa[15];
                    dy = (streamer->sa[k] >> 16) - (streamer->sa[15] >> 16);
                    streamer->angle[k] = ratan2(dy, dx) + 3072;
                    if (MODEL_VARIANT_WORD(work, 0x2078) > 512) {
                        streamer->width[k] = 1;
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
                    if (MODEL_VARIANT_WORD(work, 0x2078) > 512) {
                        streamer->width[k] = 1;
                    } else {
                        streamer->width[k] = (s16)streamer->sb[k] - (s16)streamer->sa[k];
                    }
                    streamer->ox[k] = rcos(streamer->angle[k]) * streamer->width[k] >> 12;
                    streamer->oy[k] = rsin(streamer->angle[k]) * streamer->width[k] >> 12;
                }
            }
        }
        streamer = (Model400Streamer *)(work + 0x1558);
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
                poly->r0 = streamer->color[k].r;
                poly->g0 = streamer->color[k].g;
                poly->b0 = streamer->color[k].b;
                if (streamer->depth[k] >= 0 && flags[i][k] >= 0) {
                    GsSortPoly(poly, ot, streamer->depth[k]);
                }
            }
        }
        if (MODEL_VARIANT_WORD(work, 0x2090) == 8 && MODEL_VARIANT_WORD(work, 0x2078) > 0) {
            MODEL_VARIANT_WORD(work, 0x2078) -= MODEL_VARIANT_WORD(work, 0x2050) * 12;
            if (MODEL_VARIANT_WORD(work, 0x2078) <= 0) {
                MODEL_VARIANT_WORD(work, 0x2078) = 0;
                MODEL_VARIANT_WORD(work, 0x2090) = 9;
            }
        }
    }
}
