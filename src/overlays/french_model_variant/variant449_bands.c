#include "../../types.h"
#include "variant449_bands.h"

void func_8013D458(u8 *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    Variant449BandView *work;
    GsOT *ot;
    s16 i;
    s32 angle;
    POLY_GT4 *poly;
    Variant449Band *band;
    s16 j;
    s16 sign;
    s16 bias;
    s32 lift;
    s32 progress;

    work = (Variant449BandView *)context;
    ot = func_80058F10();
    angle = ratan2(work->axis_y, work->axis_x) + 0x800;
    poly = &work->quads[1];
    if (!(work->flags & 1)) {
        bias = 0;
    } else {
        bias = work->scale / 8;
    }
    if (work->direction == 0) {
        sign = -1;
    } else {
        sign = 1;
    }
    band = work->bands;
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 3; j++) {
            lift = band->drift[j] * sign * work->scale / 4096;
            if (work->factor <= 0) {
                progress = 0;
            } else if (work->factor < 1024) {
                progress = work->factor;
            } else {
                progress = 1024;
            }
            if (j == 0) {
                progress += work->scale / 16 * work->spread / 1024;
            }
            setVector(&band->a[j], rcos(0x400) * 128 >> 12, rsin(0x400) * 128 >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(0xC00) * 128 >> 12, rsin(0xC00) * 128 >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            m.t[0] = work->origin.t[0] + work->delta.vx * progress / 1024;
            m.t[1] = work->origin.t[1] + work->delta.vy * progress / 1024;
            m.t[2] = work->origin.t[2] + work->delta.vz * progress / 1024 + lift;
            scale.vx = work->scale + bias;
            scale.vy = work->scale + bias;
            scale.vz = work->scale + bias;
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
                                         &band->sa[j], &band->sb[j], &band->sc[j], &p, &flag);
        }
    }
    band = work->bands;
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 2; j++) {
            if (j == 0) {
                poly->x0 = band->sa[j];
                poly->y0 = band->sa[j] >> 16;
                poly->x1 = band->sa[j + 1];
                poly->y1 = band->sa[j + 1] >> 16;
                poly->x2 = band->sb[j];
                poly->y2 = band->sb[j] >> 16;
                poly->x3 = band->sb[j + 1];
                poly->y3 = band->sb[j + 1] >> 16;
            } else {
                poly->x0 = band->sa[j + 1];
                poly->y0 = band->sa[j + 1] >> 16;
                poly->x1 = band->sa[j];
                poly->y1 = band->sa[j] >> 16;
                poly->x2 = band->sb[j + 1];
                poly->y2 = band->sb[j + 1] >> 16;
                poly->x3 = band->sb[j];
                poly->y3 = band->sb[j] >> 16;
            }
            poly->r0 = band->ca[j][0];
            poly->g0 = band->ca[j][1];
            poly->b0 = band->ca[j][2];
            poly->r1 = band->ca[j + 1][0];
            poly->g1 = band->ca[j + 1][1];
            poly->b1 = band->ca[j + 1][2];
            poly->r2 = band->ca[j + 1][0];
            poly->g2 = band->ca[j + 1][1];
            poly->b2 = band->ca[j + 1][2];
            poly->r3 = band->cb[j][0];
            poly->g3 = band->cb[j][1];
            poly->b3 = band->cb[j][2];
            if (band->otz[j] > 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
            if (j == 0) {
                poly->x0 = band->sc[j];
                poly->y0 = band->sc[j] >> 16;
                poly->x1 = band->sc[j + 1];
                poly->y1 = band->sc[j + 1] >> 16;
                poly->x2 = band->sb[j];
                poly->y2 = band->sb[j] >> 16;
                poly->x3 = band->sb[j + 1];
                poly->y3 = band->sb[j + 1] >> 16;
            } else {
                poly->x0 = band->sc[j + 1];
                poly->y0 = band->sc[j + 1] >> 16;
                poly->x1 = band->sc[j];
                poly->y1 = band->sc[j] >> 16;
                poly->x2 = band->sb[j + 1];
                poly->y2 = band->sb[j + 1] >> 16;
                poly->x3 = band->sb[j];
                poly->y3 = band->sb[j] >> 16;
            }
            poly->r0 = band->ca[j][0];
            poly->g0 = band->ca[j][1];
            poly->b0 = band->ca[j][2];
            poly->r1 = band->ca[j + 1][0];
            poly->g1 = band->ca[j + 1][1];
            poly->b1 = band->ca[j + 1][2];
            poly->r2 = band->ca[j + 1][0];
            poly->g2 = band->ca[j + 1][1];
            poly->b2 = band->ca[j + 1][2];
            poly->r3 = band->cb[j][0];
            poly->g3 = band->cb[j][1];
            poly->b3 = band->cb[j][2];
            if (band->otz[j] > 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
        }
    }
}
