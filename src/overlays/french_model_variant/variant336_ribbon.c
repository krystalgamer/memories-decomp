#include "../../types.h"
#include "variant336_ribbon.h"
#include "variant336_entry.h"

void FrenchModelVariant336_DrawRibbon(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant336EntryState *work;
    GsOT *ot;
    s16 i;
    s32 wave;
    s32 phase;
    s32 yaw;
    s16 len;
    ModelVariant336Ribbon *ribbon;
    POLY_FT4 *poly;
    s16 k;
    s32 bend;
    s32 dx;
    s32 dy;

    work = (Variant336EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    yaw = ratan2(work->aim.view_delta.vz, work->aim.view_delta.vx) + 0xC00;
    if (work->phase > 0) {
        ribbon = (ModelVariant336Ribbon *)work;
        for (i = 0, phase = 0; i < 4; i++, ribbon++, phase = i * 0x400) {
            len = ribbon->extent * 48 / 1024;
            ribbon->origin.vx = work->aim.direction.vx * (i + 1) / 4;
            ribbon->origin.vy = -1024;
            ribbon->origin.vz = work->aim.direction.vz * (i + 1) / 4;
            ribbon->end.vx = ribbon->origin.vx;
            ribbon->end.vy = 0;
            ribbon->end.vz = ribbon->origin.vz;
            MODEL_VARIANT_WORD(work, 0x1F80) = ribbon->end.vx - ribbon->origin.vx;
            MODEL_VARIANT_WORD(work, 0x1F84) = ribbon->end.vy - ribbon->origin.vy;
            MODEL_VARIANT_WORD(work, 0x1F88) = ribbon->end.vz - ribbon->origin.vz;
            for (k = 0, wave = phase - work->wave0; k < 17; k++, wave += 1100) {
                if (k == 0 || k == 16) {
                    bend = 0;
                } else {
                    bend = phase - k * 384 + work->wave1;
                }
                setVector(&ribbon->spine.a[k],
                          ribbon->origin.vx + MODEL_VARIANT_WORD(work, 0x1F80) * k / 16
                              + (rcos(yaw) * ((rsin(bend) * 64 >> 12) + (rsin(wave) * 48 >> 12)) >> 12),
                          ribbon->origin.vy + MODEL_VARIANT_WORD(work, 0x1F84) * k / 16,
                          ribbon->origin.vz + MODEL_VARIANT_WORD(work, 0x1F88) * k / 16
                              + (rsin(yaw) * ((rsin(bend) * 64 >> 12) + (rsin(wave) * 48 >> 12)) >> 12));
                setVector(&ribbon->spine.b[k], ribbon->spine.a[k].vx + (rcos(yaw) * len >> 12), ribbon->spine.a[k].vy,
                          ribbon->spine.a[k].vz + (rsin(yaw) * len >> 12));
            }
        }
        setVector(&rot, 0, 0, 0);
        m.t[0] = work->origin[0];
        m.t[1] = 0;
        m.t[2] = work->origin[2];
        setVector(&scale, 4096, 4096, 4096);
        RotMatrix(&rot, &m);
        ScaleMatrix(&m, &scale);
        coord.coord = m;
        coord.super = 0;
        coord.flg = 0;
        GsGetLs(&coord, &ls);
        GsSetLsMatrix(&ls);
        ribbon = (ModelVariant336Ribbon *)work;
        for (i = 0; i < 4; i++, ribbon++) {
            for (k = 0; k < 17; k++) {
                if (k == 16) {
                    ribbon->offsets.otz[k] = RotTransPers4(&ribbon->spine.a[15], &ribbon->spine.a[k], &ribbon->spine.a[15], &ribbon->spine.a[k],
                                                 &ribbon->spine.sa[15], &ribbon->spine.sa[k], &ribbon->spine.sa[15], &ribbon->spine.sa[k],
                                                 &p, &ribbon->flag[k]);
                    RotTransPers(&ribbon->spine.b[k], &ribbon->spine.sb[k], &p, &flag);
                    dx = (s16)ribbon->spine.sa[k] - (s16)ribbon->spine.sa[15];
                    dy = (ribbon->spine.sa[k] >> 16) - (ribbon->spine.sa[15] >> 16);
                    ribbon->spine.angle[k] = ratan2(dy, dx) - 0x400;
                    ribbon->spine.width[k] = (s16)ribbon->spine.sb[k] - (s16)ribbon->spine.sa[k];
                    ribbon->offsets.ox[k] = rcos(ribbon->spine.angle[k]) * ribbon->spine.width[k] >> 12;
                    ribbon->offsets.oy[k] = rsin(ribbon->spine.angle[k]) * ribbon->spine.width[k] >> 12;
                } else {
                    ribbon->offsets.otz[k] = RotTransPers4(&ribbon->spine.a[k], &ribbon->spine.a[k + 1], &ribbon->spine.a[k], &ribbon->spine.a[k + 1],
                                                 &ribbon->spine.sa[k], &ribbon->spine.sa[k + 1], &ribbon->spine.sa[k], &ribbon->spine.sa[k + 1],
                                                 &p, &ribbon->flag[k]);
                    RotTransPers(&ribbon->spine.b[k], &ribbon->spine.sb[k], &p, &flag);
                    dx = (s16)ribbon->spine.sa[k + 1] - (s16)ribbon->spine.sa[k];
                    dy = (ribbon->spine.sa[k + 1] >> 16) - (ribbon->spine.sa[k] >> 16);
                    ribbon->spine.angle[k] = ratan2(dy, dx) - 0x400;
                    ribbon->spine.width[k] = (s16)ribbon->spine.sb[k] - (s16)ribbon->spine.sa[k];
                    ribbon->offsets.ox[k] = rcos(ribbon->spine.angle[k]) * ribbon->spine.width[k] >> 12;
                    ribbon->offsets.oy[k] = rsin(ribbon->spine.angle[k]) * ribbon->spine.width[k] >> 12;
                }
            }
        }
        ribbon = (ModelVariant336Ribbon *)work;
        for (i = 0; i < 4; i++, ribbon++) {
            k = 0;
            poly = work->flat_textured;
            for (; k < ribbon->count; k++) {
                poly->x0 = ribbon->spine.sa[k] + ribbon->offsets.ox[k];
                poly->y0 = (ribbon->spine.sa[k] >> 16) + ribbon->offsets.oy[k];
                poly->x1 = ribbon->spine.sa[k + 1] + ribbon->offsets.ox[k + 1];
                poly->y1 = (ribbon->spine.sa[k + 1] >> 16) + ribbon->offsets.oy[k + 1];
                poly->x2 = ribbon->spine.sa[k] - ribbon->offsets.ox[k];
                poly->y2 = (ribbon->spine.sa[k] >> 16) - ribbon->offsets.oy[k];
                poly->x3 = ribbon->spine.sa[k + 1] - ribbon->offsets.ox[k + 1];
                poly->y3 = (ribbon->spine.sa[k + 1] >> 16) - ribbon->offsets.oy[k + 1];
                poly->r0 = ribbon->color[0];
                poly->g0 = ribbon->color[1];
                poly->b0 = ribbon->color[2];
                if (ribbon->offsets.otz[k] >= 0 && ribbon->flag[k] >= 0) {
                    GsSortPoly(poly, ot, ribbon->offsets.otz[k]);
                }
                if (!(k & 1)) {
                    poly++;
                } else {
                    poly--;
                }
            }
            if (ribbon->state == 1) {
                if (ribbon->count < 16) {
                    ribbon->count += work->clock.step * 2;
                    if (ribbon->count >= 16) {
                        ribbon->count = 16;
                        work->sheet_count = i * 2 + 3;
                        if (work->sheet_count >= 8) {
                            work->sheet_count = 8;
                        }
                    }
                }
            } else if (ribbon->state == 2 && ribbon->extent > 0) {
                ribbon->extent -= work->clock.step * 8;
                if (ribbon->extent <= 0) {
                    ribbon->extent = 0;
                    if (i == 3) {
                        work->phase = 3;
                    }
                }
            }
        }
    }
    work->wave0 += work->clock.step * 650;
    work->wave1 += work->clock.step * 100;
}
