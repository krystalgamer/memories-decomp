#include "../../types.h"
#include "variant397_beams.h"

/* Header 397's beams: for every configured record, a thirteen-point beam
 * coils from the record's matrix along its direction, is projected with a
 * view-shifted copy, and is drawn as flat POLY_FT4 strips whose packet pair
 * follows the record's mode. A per-record state machine grows the drawn
 * length, advances the mode and raises the shared phase when every record is
 * done. As in header 376, the length offset is an s16 and each branch of the
 * last-point test computes its own width offset. */

void func_8013BDE4(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    Variant397BeamsState *work;
    ModelVariantSheetSet *sheet;
    Variant397Beam *beam;
    GsOT *ot;
    s16 i;
    s16 k;
    s32 bend;
    s32 wave;
    s32 offset;
    s32 twist;
    s32 done;
    s32 yaw;
    s32 coil;
    s16 len;
    s32 count;
    POLY_FT4 *poly;
    s32 dx;
    s32 dy;

    work = (Variant397BeamsState *)ctx;
    sheet = work->sheets;
    ot = func_80058F10();
    yaw = ratan2(work->view_delta.vz, work->view_delta.vx) + 0xC00;
    ratan2(work->view_delta.vy, work->view_delta.vx);
    done = 0;
    len = work->width * 40 / 1024;
    beam = work->beams;
    for (i = 0, offset = 0; i < work->config->count; i++, beam++, offset = i * 3 * 2048 / work->config->count) {
        if (i == 0) {
            twist = 0x400;
        } else if (i % 2 == 1) {
            twist = ((i + 1) << 11) / work->config->count + 0x400;
        } else {
            twist = 0x400 - (i << 11) / work->config->count;
        }
        for (k = 0, wave = -work->wave; k < 13; k++, wave += 0x514) {
            if (k == 0 || k == 12) {
                bend = 0;
            } else {
                bend = offset - (k << 9) + work->bend;
            }
            coil = (k << 11) / 12;
            setVector(&beam->a[k],
                      work->directions[i].vx * k / 12 -
                          (rcos(twist) * (rsin(coil) * 384 >> 12) >> 12) +
                          (rcos(yaw) * (rsin(bend) * ((rsin(wave) * 128 >> 12) + 0x80) >> 12) >> 12),
                      work->directions[i].vy * k / 12,
                      work->directions[i].vz * k / 12 -
                          (rsin(twist) * (rsin(coil) * 384 >> 12) >> 12) +
                          (rsin(yaw) * (rsin(bend) * ((rsin(wave) * 128 >> 12) + 0x80) >> 12) >> 12));
            setVector(&beam->b[k], beam->a[k].vx + (rcos(yaw) * len >> 12), beam->a[k].vy,
                      beam->a[k].vz + (rsin(yaw) * len >> 12));
        }
        rot.vx = 0;
        rot.vy = 0;
        rot.vz = 0;
        m.t[0] = work->matrices[i].t[0];
        m.t[1] = work->matrices[i].t[1];
        m.t[2] = work->matrices[i].t[2];
        scale.vx = 0x1000;
        scale.vy = 0x1000;
        scale.vz = 0x1000;
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        for (k = 0; k < 13; k++) {
            if (k == 12) {
                beam->otz[k] = RotTransPers4(&beam->a[k - 1], &beam->a[k], &beam->a[k - 1], &beam->a[k],
                                             &beam->sa[k - 1], &beam->sa[k], &beam->sa[k - 1], &beam->sa[k],
                                             &p, &beam->flag[k]);
                RotTransPers(&beam->b[k], &beam->sb[k], &p, &flag);
                dx = (s16)beam->sa[k] - (s16)beam->sa[k - 1];
                dy = (beam->sa[k] >> 16) - (beam->sa[k - 1] >> 16);
                beam->angle[k] = ratan2(dy, dx) - 0x400;
                beam->width[k] = (s16)beam->sb[k] - (s16)beam->sa[k];
                beam->ox[k] = rcos(beam->angle[k]) * beam->width[k] >> 12;
                beam->oy[k] = rsin(beam->angle[k]) * beam->width[k] >> 12;
            } else {
                beam->otz[k] = RotTransPers4(&beam->a[k], &beam->a[k + 1], &beam->a[k], &beam->a[k + 1],
                                             &beam->sa[k], &beam->sa[k + 1], &beam->sa[k], &beam->sa[k + 1],
                                             &p, &beam->flag[k]);
                RotTransPers(&beam->b[k], &beam->sb[k], &p, &flag);
                dx = (s16)beam->sa[k + 1] - (s16)beam->sa[k];
                dy = (beam->sa[k + 1] >> 16) - (beam->sa[k] >> 16);
                beam->angle[k] = ratan2(dy, dx) - 0x400;
                beam->width[k] = (s16)beam->sb[k] - (s16)beam->sa[k];
                beam->ox[k] = rcos(beam->angle[k]) * beam->width[k] >> 12;
                beam->oy[k] = rsin(beam->angle[k]) * beam->width[k] >> 12;
            }
        }
    }
    beam = work->beams;
    for (i = 0; i < work->config->count; i++, beam++, sheet++) {
        if (work->length[i] < 0) {
            count = 0;
        } else {
            count = work->length[i];
        }
        for (k = 0; k < count; k++) {
            if (work->mode[i] == 3) {
                poly = &work->flat[2];
            } else if (work->mode[i] == 4) {
                poly = &work->flat[4];
            } else {
                poly = &work->flat[0];
            }
            if (k % 2 == 1) {
                poly++;
            }
            poly->x0 = beam->sa[k] + beam->ox[k];
            poly->y0 = (beam->sa[k] >> 16) + beam->oy[k];
            poly->x1 = beam->sa[k + 1] + beam->ox[k + 1];
            poly->y1 = (beam->sa[k + 1] >> 16) + beam->oy[k + 1];
            poly->x2 = beam->sa[k] - beam->ox[k];
            poly->y2 = (beam->sa[k] >> 16) - beam->oy[k];
            poly->x3 = beam->sa[k + 1] - beam->ox[k + 1];
            poly->y3 = (beam->sa[k + 1] >> 16) - beam->oy[k + 1];
            poly->r0 = beam->color[0];
            poly->g0 = beam->color[1];
            poly->b0 = beam->color[2];
            if (work->mode[i] < 5 && beam->otz[k] >= 0 && beam->flag[k] >= 0) {
                GsSortPoly(poly, ot, beam->otz[k]);
            }
        }
        if (work->phase == 1 || work->phase >= 3) {
            if (work->length[i] < 12) {
                work->length[i] += work->step;
                if (work->length[i] >= 12) {
                    work->length[i] = 12;
                    sheet->shown = 0;
                    if (work->phase == 3) {
                        work->phase = 4;
                    }
                }
            } else if (work->mode[i] < 5) {
                work->mode[i]++;
            }
            if (work->config->grow_end < work->frame && work->phase == 1) {
                work->mode[i]++;
            }
            if (work->mode[i] >= 5) {
                if (work->config->grow_end < work->frame && work->phase == 1) {
                    work->length[i] = 12;
                    work->done[i] = 1;
                    work->mode[i] = 5;
                } else if (work->config->release_at < work->frame && work->phase >= 4) {
                    work->length[i] = 12;
                    work->done[i] = 1;
                    work->mode[i] = 5;
                } else {
                    work->length[i] = 0;
                    work->mode[i] = 0;
                }
            }
        }
        done += work->done[i];
        if (done == work->config->count) {
            if (work->phase == 1) {
                work->phase = 2;
            } else if (work->phase == 4) {
                work->phase = 5;
            }
        }
        if (work->phase == 2) {
            work->length[i] = -(i * 8);
            work->mode[i] = 0;
            work->done[i] = 0;
            if (i + 1 == work->config->count && work->config->hide_at < work->frame) {
                work->phase = 3;
            }
        }
    }
    work->wave += work->step << 7;
    work->bend += work->step << 5;
}
