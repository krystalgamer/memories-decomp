#include "../../types.h"
#include "variant476_globe.h"
#include "../model_variant/variant459_entry.h"

void ModelVariant459_DrawGlobe(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    ModelVariantMesh *globe;
    GsOT *ot;
    POLY_GT4 *poly;
    Variant459EntryState *work;
    Variant459EntryConfig *timing;
    s32 i;
    s32 j;
    s32 otz;

    work = (Variant459EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    globe = (ModelVariantMesh *)((u8 *)work + 0xFF8);
    ratan2(work->direction.vz, work->direction.vx);
    poly = &work->veil;
    ratan2(work->direction.vy, work->direction.vz);
    if (work->slot == 0) {
        rot.vx = (s16)work->globe_rotation_x;
        rot.vy = 0;
        rot.vz = 0;
    } else {
        rot.vx = -(s16)work->globe_rotation_x;
        rot.vy = 0;
        rot.vz = 0;
    }
    m.t[0] = work->target.vx;
    m.t[1] = work->target.vy;
    m.t[2] = work->target.vz;
    scale.vx = work->globe_scale;
    scale.vy = work->globe_scale;
    scale.vz = work->globe_scale;
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    if (work->phase < 5) {
        work->globe_brightness = 1024 - (work->globe_scale - 8192) / 8;
        for (i = 0; i < 9; i++) {
            globe->colors[i][0] = (i * 255 / 8) * work->globe_brightness / 1024;
            globe->colors[i][1] = work->globe_brightness / 16;
            globe->colors[i][2] = (255 - i * 255 / 8) * work->globe_brightness / 1024;
        }
    } else {
        for (i = 0; i < 9; i++) {
            globe->colors[i][0] = work->globe_brightness / 8;
            globe->colors[i][1] = work->globe_brightness / 8;
            globe->colors[i][2] = work->globe_brightness / 8;
        }
    }
    for (i = 0; i < 8; i++) {
        setRGB0(poly, globe->colors[i][0], globe->colors[i][1], globe->colors[i][2]);
        setRGB1(poly, globe->colors[i][0], globe->colors[i][1], globe->colors[i][2]);
        setRGB2(poly, globe->colors[i + 1][0], globe->colors[i + 1][1], globe->colors[i + 1][2]);
        setRGB3(poly, globe->colors[i + 1][0], globe->colors[i + 1][1], globe->colors[i + 1][2]);
        for (j = 0; j < 16; j++) {
            otz = RotTransPers4(&globe->rows[i][j], &globe->rows[i][j + 1],
                               &globe->rows[i + 1][j], &globe->rows[i + 1][j + 1],
                               (long *)&poly->x0, (long *)&poly->x1,
                               (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
    }
    if (work->field_2814 <= 128) {
        work->field_2814 += 8;
        if (work->field_2814 >= 128) {
            work->field_2814 = 0;
        }
    }
    if (work->phase < 3) {
        if (work->globe_scale < 8192) {
            work->globe_scale += work->clock.step * 512;
            if (work->globe_scale >= 8192) {
                work->globe_scale = 8192;
                work->phase = 3;
            }
        }
    } else if (work->phase == 3) {
        work->globe_scale = (rcos(work->globe_pulse_angle) * 2048 >> 12) + 6144;
        work->globe_pulse_angle += work->clock.step * 64;
        timing = work->config;
        if ((u32)timing->field_34 < (u32)work->clock.frame) {
            work->phase = 4;
        }
    } else if (work->phase == 4) {
        work->globe_scale += work->clock.step * 256;
        if (work->globe_scale >= 16384) {
            work->phase = 5;
            work->globe_brightness = 1024;
        }
    } else if (work->phase == 5) {
        work->globe_scale += work->clock.step * 256;
        if (work->globe_brightness > 0) {
            work->globe_brightness -= work->clock.step * 32;
            if (work->globe_brightness <= 0) {
                work->globe_brightness = 0;
                work->phase = 6;
            }
        }
    }
    work->globe_rotation_x += work->clock.step * 64;
}
