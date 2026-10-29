#include "../../types.h"
#include "../french_model_variant/variant425_entry.h"

/* Grows a seventeen-point ribbon along the entry's direction as the progress
 * rises, moves a copy of it along the view by a length that pulses along the
 * ribbon, projects both and draws each segment as two POLY_GT4 halves (blue to
 * grey) where the depth and the projection flag are not negative. Afterwards it
 * advances the two spins, raises the progress over the configured span (phase 2
 * moves to 3 at its end) and, from the configured fade start, shrinks the width
 * (phase 4 moves to 5 at zero). The view pitch, the per-ribbon turn and the
 * per-point bend of the related ribbon forms are still computed but unused. */
void func_8013CD7C(u8 *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flags[1][17];
    PSXLONG p;
    PSXLONG flag;
    EntryState425 *work;
    EntryRibbon425 *ribbon;
    POLY_GT4 *poly;
    GsOT *ot;
    s16 i;
    s16 k;
    s16 len;
    s16 amp;
    s32 wave;
    s32 bend;
    s32 turn;
    s32 yaw;
    s32 pitch;
    s32 dx;
    s32 dy;
    s32 y;
    s32 z;

    work = (EntryState425 *)context;
    ot = func_80058F10();
    yaw = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    pitch = ratan2(work->view_delta.vy, work->view_delta.vx) + 0xC00;
    poly = work->textured;
    if (!(work->frame_count & 1)) {
        len = work->width * 80 / 1024;
        amp = work->width / 64;
    } else {
        len = work->width * 96 / 1024;
        amp = work->width / 64;
    }
    ribbon = work->ribbons;
    for (i = 0; i < 1; i++, ribbon++) {
        if (i == 0) {
            turn = 0x400;
        } else if (i % 2 == 1) {
            turn = i * 360 + 0x400;
        } else {
            turn = 0x400 - i * 360;
        }
        for (k = 0, wave = -work->field_17FC; k < 17; k++, wave += 0x300) {
            y = work->direction.vy * work->progress / 1024;
            z = work->direction.vz * work->progress / 1024;
            if (k == 0 || k == 16) {
                bend = 0;
            } else {
                bend = work->field_1800 - k * 384;
            }
            setVector(&ribbon->a[k], work->direction.vx * work->progress / 1024 * k / 16, y * k / 16, z * k / 16);
            setVector(&ribbon->b[k], ribbon->a[k].vx + (rcos(yaw) * (len + (rcos(wave) * amp >> 12)) >> 12),
                      ribbon->a[k].vy,
                      ribbon->a[k].vz + (rsin(yaw) * (len + (rsin(wave) * amp >> 12)) >> 12));
        }
    }
    setVector(&rot, 0, 0, 0);
    m.t[0] = work->matrix.t[0];
    m.t[1] = work->matrix.t[1];
    m.t[2] = work->matrix.t[2];
    setVector(&scale, 4096, 4096, 4096);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    ribbon = work->ribbons;
    for (i = 0; i < 1; i++, ribbon++) {
        for (k = 0; k < 17; k++) {
            if (k == 16) {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[15], &ribbon->a[16], &ribbon->a[15], &ribbon->a[16],
                                               &ribbon->sa[15], &ribbon->sa[16],
                                               &ribbon->sa[15], &ribbon->sa[16], &p, &flags[i][16]);
                RotTransPers(&ribbon->b[16], &ribbon->sb[16], &p, &flag);
                dx = (s16)ribbon->sa[k] - (s16)ribbon->sa[15];
                dy = (s16)(ribbon->sa[k] >> 16) - (s16)(ribbon->sa[15] >> 16);
                ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                ribbon->width[k] = (s16)ribbon->sb[k] - (s16)ribbon->sa[k];
                ribbon->ox[16] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[16] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            } else {
                ribbon->otz[k] = RotTransPers4(&ribbon->a[k], &ribbon->a[k + 1], &ribbon->a[k], &ribbon->a[k + 1],
                                               &ribbon->sa[k], &ribbon->sa[k + 1],
                                               &ribbon->sa[k], &ribbon->sa[k + 1], &p, &flags[i][k]);
                RotTransPers(&ribbon->b[k], &ribbon->sb[k], &p, &flag);
                dx = (s16)ribbon->sa[k + 1] - (s16)ribbon->sa[k];
                dy = (s16)(ribbon->sa[k + 1] >> 16) - (s16)(ribbon->sa[k] >> 16);
                ribbon->angle[k] = ratan2(dy, dx) - 0x400;
                ribbon->width[k] = (s16)ribbon->sb[k] - (s16)ribbon->sa[k];
                ribbon->ox[k] = rcos(ribbon->angle[k]) * ribbon->width[k] >> 12;
                ribbon->oy[k] = rsin(ribbon->angle[k]) * ribbon->width[k] >> 12;
            }
        }
    }
    ribbon = work->ribbons;
    for (i = 0; i < 1; i++, ribbon++) {
        for (k = 0; k < 16; k++) {
            setRGB0(poly, 0, 0, 0xFF);
            setRGB1(poly, 0, 0, 0xFF);
            setRGB2(poly, 0xC0, 0xC0, 0xC0);
            setRGB3(poly, 0xC0, 0xC0, 0xC0);
            poly->x0 = (s16)ribbon->sa[k] + ribbon->ox[k];
            poly->y0 = (ribbon->sa[k] >> 16) + ribbon->oy[k];
            poly->x1 = (s16)ribbon->sa[k + 1] + ribbon->ox[k + 1];
            poly->y1 = (ribbon->sa[k + 1] >> 16) + ribbon->oy[k + 1];
            poly->x2 = (s16)ribbon->sa[k];
            poly->y2 = ribbon->sa[k] >> 16;
            poly->x3 = (s16)ribbon->sa[k + 1];
            poly->y3 = ribbon->sa[k + 1] >> 16;
            if (ribbon->otz[k] >= 0 && flags[i][k] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[k]);
            }
            poly->x0 = (s16)ribbon->sa[k] - ribbon->ox[k];
            poly->y0 = (ribbon->sa[k] >> 16) - ribbon->oy[k];
            poly->x1 = (s16)ribbon->sa[k + 1] - ribbon->ox[k + 1];
            poly->y1 = (ribbon->sa[k + 1] >> 16) - ribbon->oy[k + 1];
            poly->x2 = (s16)ribbon->sa[k];
            poly->y2 = ribbon->sa[k] >> 16;
            poly->x3 = (s16)ribbon->sa[k + 1];
            poly->y3 = ribbon->sa[k + 1] >> 16;
            if (ribbon->otz[k] >= 0 && flags[i][k] >= 0) {
                GsSortPoly(poly, ot, ribbon->otz[k]);
            }
        }
    }
    work->field_17FC += work->step * 384;
    work->field_1800 += work->step * 128;
    if (work->progress < 0x400 && work->elapsed >= work->config->field_1C) {
        work->progress = ((work->elapsed - work->config->field_1C) << 10)
                         / (work->config->travel_start - work->config->field_1C);
        if (work->progress >= 0x400) {
            work->progress = 0x400;
            if (work->phase == 2) {
                work->phase = 3;
            }
        }
    }
    if (work->elapsed >= work->config->fade_start && work->width > 0) {
        work->width = 0x400 - ((work->elapsed - work->config->fade_start) << 10)
                                  / (work->config->fade_end - work->config->fade_start);
        if (work->width <= 0) {
            work->width = 0;
            if (work->phase == 4) {
                work->phase = 5;
            }
        }
    }
}
