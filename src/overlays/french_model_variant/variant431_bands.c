#include "../../types.h"
#include "variant431_entry.h"

void func_8013C7C8(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG status[5][5];
    PSXLONG p;
    Variant431EntryState *work;
    Variant431EntryBand *band;
    POLY_GT4 *poly;
    GsOT *ot;
    s16 i;
    s16 j;
    s16 k;
    s32 reset;
    s32 angle;
    s32 radius;
    s32 distance;
    Variant414Fan *fan;

    work = (Variant431EntryState *)ctx;
    band = work->bands;
    poly = &work->textured[0];
    ot = func_80058F10();
    reset = 0;
    for (i = 0; i < 5; i++, band++) {
        angle = ratan2(work->screen_y[i], work->screen_x[i]) + 2048;
        radius = band->field_108 / 64;
        for (j = 0; j < 5; j++) {
            distance = band->progress[j];
            if (distance <= 0) {
                distance = 0;
            }
            setVector(&band->a[j], rcos(1024) * radius >> 12,
                      rsin(1024) * radius >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(3072) * radius >> 12,
                      rsin(3072) * radius >> 12, 0);
            setVector(&rot, 0, 0, angle);
            m.t[0] = work->matrices[i].t[0] + work->directions[i].vx * distance / 1024;
            m.t[1] = work->matrices[i].t[1] + work->directions[i].vy * distance / 1024;
            m.t[2] = work->matrices[i].t[2] + work->directions[i].vz * distance / 1024;
            setVector(&scale, 4096, 4096, 4096);
            RotMatrix(&rot, &m);
            coord.coord = m;
            coord.super = 0;
            coord.flg = 0;
            GsGetLs(&coord, &ls);
            GsSetLsMatrix(&ls);
            ReadRotMatrix(&ls);
            RotMatrix(&rot, &ls);
            ScaleMatrix(&ls, &scale);
            SetRotMatrix(&ls);
            band->otz[j] = RotTransPers3(&band->a[j], &band->b[j], &band->c[j],
                                       &band->sa[j], &band->sb[j], &band->sc[j],
                                       &p, &status[i][j]);
            if (band->progress[j] <= 1024) {
                band->progress[j] += work->step << 7;
                work->field_18D0 = 256;
                if (band->progress[j] >= 1024) {
                    band->progress[j] = 1024;
                    if (work->phase < 2) {
                        work->phase = 2;
                    }
                    if (i + 1 == 5 && j == 4) {
                        if (work->start_index < 10) {
                            work->start_index++;
                            reset = 1;
                        } else if (work->phase == 2) {
                            work->phase = 3;
                        }
                    }
                }
            }
        }
    }
    band = work->bands;
    if (reset == 1) {
        for (i = 0; i < 5; i++, band++) {
            for (j = 0; j < 5; j++) {
                band->progress[j] = -(j << 8) - (i << 8);
            }
        }
        fan = work->fans;
        for (k = 0; k < 5; k++, fan++) {
            fan->done = 0;
        }
        band = work->bands;
    }
    for (i = 0; i < 5; i++, band++) {
        for (j = 0; j < 4; j++) {
            poly->x0 = band->sa[j];
            poly->y0 = band->sa[j] >> 16;
            poly->x1 = band->sa[j + 1];
            poly->y1 = band->sa[j + 1] >> 16;
            poly->x2 = band->sb[j];
            poly->y2 = band->sb[j] >> 16;
            poly->x3 = band->sb[j + 1];
            poly->y3 = band->sb[j + 1] >> 16;
            if (work->phase >= 3) {
                poly->r0 = band->ca[j].r * work->field_18D2 / 1024;
                poly->g0 = band->ca[j].g * work->field_18D2 / 1024;
                poly->b0 = band->ca[j].b * work->field_18D2 / 1024;
                poly->r1 = band->ca[j + 1].r * work->field_18D2 / 1024;
                poly->g1 = band->ca[j + 1].g * work->field_18D2 / 1024;
                poly->b1 = band->ca[j + 1].b * work->field_18D2 / 1024;
                poly->r2 = band->cb[j].r * work->field_18D2 / 1024;
                poly->g2 = band->cb[j].g * work->field_18D2 / 1024;
                poly->b2 = band->cb[j].b * work->field_18D2 / 1024;
                poly->r3 = band->cb[j + 1].r * work->field_18D2 / 1024;
                poly->g3 = band->cb[j + 1].g * work->field_18D2 / 1024;
                poly->b3 = band->cb[j + 1].b * work->field_18D2 / 1024;
            } else {
                poly->r0 = band->ca[j].r;
                poly->g0 = band->ca[j].g;
                poly->b0 = band->ca[j].b;
                poly->r1 = band->ca[j + 1].r;
                poly->g1 = band->ca[j + 1].g;
                poly->b1 = band->ca[j + 1].b;
                poly->r2 = band->cb[j].r;
                poly->g2 = band->cb[j].g;
                poly->b2 = band->cb[j].b;
                poly->r3 = band->cb[j + 1].r;
                poly->g3 = band->cb[j + 1].g;
                poly->b3 = band->cb[j + 1].b;
            }
            if (band->otz[j] >= 0 && status[i][j] >= 0) {
                GsSortPoly(poly, ot, (u16)band->otz[j]);
            }
            poly->x0 = band->sc[j];
            poly->y0 = band->sc[j] >> 16;
            poly->x1 = band->sc[j + 1];
            poly->y1 = band->sc[j + 1] >> 16;
            poly->x2 = band->sb[j];
            poly->y2 = band->sb[j] >> 16;
            poly->x3 = band->sb[j + 1];
            poly->y3 = band->sb[j + 1] >> 16;
            if (work->phase >= 3) {
                poly->r0 = band->ca[j].r * work->field_18D2 / 1024;
                poly->g0 = band->ca[j].g * work->field_18D2 / 1024;
                poly->b0 = band->ca[j].b * work->field_18D2 / 1024;
                poly->r1 = band->ca[j + 1].r * work->field_18D2 / 1024;
                poly->g1 = band->ca[j + 1].g * work->field_18D2 / 1024;
                poly->b1 = band->ca[j + 1].b * work->field_18D2 / 1024;
                poly->r2 = band->cb[j].r * work->field_18D2 / 1024;
                poly->g2 = band->cb[j].g * work->field_18D2 / 1024;
                poly->b2 = band->cb[j].b * work->field_18D2 / 1024;
                poly->r3 = band->cb[j + 1].r * work->field_18D2 / 1024;
                poly->g3 = band->cb[j + 1].g * work->field_18D2 / 1024;
                poly->b3 = band->cb[j + 1].b * work->field_18D2 / 1024;
            } else {
                poly->r0 = band->ca[j].r;
                poly->g0 = band->ca[j].g;
                poly->b0 = band->ca[j].b;
                poly->r1 = band->ca[j + 1].r;
                poly->g1 = band->ca[j + 1].g;
                poly->b1 = band->ca[j + 1].b;
                poly->r2 = band->cb[j].r;
                poly->g2 = band->cb[j].g;
                poly->b2 = band->cb[j].b;
                poly->r3 = band->cb[j + 1].r;
                poly->g3 = band->cb[j + 1].g;
                poly->b3 = band->cb[j + 1].b;
            }
            if (band->otz[j] >= 0 && status[i][j] >= 0) {
                GsSortPoly(poly, ot, (u16)band->otz[j]);
            }
        }
    }
    if (work->phase == 1) {
        if (work->field_18D4 <= 1024) {
            work->field_18D4 += work->step << 6;
            work->field_18D0 = 256;
            if (work->field_18D4 >= 1024) {
                work->field_18D4 = 1024;
            }
        }
    }
}
