#include "../../types.h"

#include "variant324_bands.h"
#include "../french_model_variant/variant341_entry.h"

/* Draws one band of nine points as pairs of POLY_GT4 quads along the variant's
 * path, its radius set by (u8 *)work + 0xF44 and its colours faded by (u8 *)work + 0xF46
 * from phase 3. When the timing record's last entry is reached the band
 * stretches (phase 1), widens (phase 2) and then fades out (phase 3) by
 * frame steps. */
void ModelVariant324_DrawBand(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    Variant341EntryState *work;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *poly;
    Variant324Band *band;
    s16 i;
    s16 j;
    s16 r;

    work = (Variant341EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    band = (Variant324Band *)work->bands;
    poly = work->packets.textured;
    angle = ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx) + 0x800;
    r = work->radius / 32;
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 9; j++) {
            setVector(&band->a[j], rcos(0x400) * r >> 12, rsin(0x400) * r >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(0xC00) * r >> 12, rsin(0xC00) * r >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            if (j == 0) {
                m.t[0] = work->matrix.t[0];
                m.t[1] = work->matrix.t[1];
                m.t[2] = work->matrix.t[2];
            } else {
                m.t[0] = work->matrix.t[0] + work->aim.direction.vx * j / 8 * work->progress / 1024;
                m.t[1] = work->matrix.t[1] + work->aim.direction.vy * j / 8 * work->progress / 1024;
                m.t[2] = work->matrix.t[2] + work->aim.direction.vz * j / 8 * work->progress / 1024;
            }
            scale.vx = 0x1000;
            scale.vy = 0x1000;
            scale.vz = 0x1000;
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
                                         &band->sa[j], &band->sb[j],
                                         &band->sc[j], &p, &band->flag[j]);
        }
    }
    band = (Variant324Band *)work->bands;
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 8; j++) {
            poly->x0 = band->sa[j];
            poly->y0 = band->sa[j] >> 16;
            poly->x1 = band->sa[j + 1];
            poly->y1 = band->sa[j + 1] >> 16;
            poly->x2 = band->sb[j];
            poly->y2 = band->sb[j] >> 16;
            poly->x3 = band->sb[j + 1];
            poly->y3 = band->sb[j + 1] >> 16;
            if (work->tail.phase >= 3) {
                poly->r0 = band->ca[j][0] * work->brightness / 1024;
                poly->g0 = band->ca[j][1] * work->brightness / 1024;
                poly->b0 = band->ca[j][2] * work->brightness / 1024;
                poly->r1 = band->ca[j + 1][0] * work->brightness / 1024;
                poly->g1 = band->ca[j + 1][1] * work->brightness / 1024;
                poly->b1 = band->ca[j + 1][2] * work->brightness / 1024;
                poly->r2 = band->cb[j][0] * work->brightness / 1024;
                poly->g2 = band->cb[j][1] * work->brightness / 1024;
                poly->b2 = band->cb[j][2] * work->brightness / 1024;
                poly->r3 = band->cb[j + 1][0] * work->brightness / 1024;
                poly->g3 = band->cb[j + 1][1] * work->brightness / 1024;
                poly->b3 = band->cb[j + 1][2] * work->brightness / 1024;
            } else {
                poly->r0 = band->ca[j][0];
                poly->g0 = band->ca[j][1];
                poly->b0 = band->ca[j][2];
                poly->r1 = band->ca[j + 1][0];
                poly->g1 = band->ca[j + 1][1];
                poly->b1 = band->ca[j + 1][2];
                poly->r2 = band->cb[j][0];
                poly->g2 = band->cb[j][1];
                poly->b2 = band->cb[j][2];
                poly->r3 = band->cb[j + 1][0];
                poly->g3 = band->cb[j + 1][1];
                poly->b3 = band->cb[j + 1][2];
            }
            if (band->otz[j] >= 0 && band->flag[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
            poly->x0 = band->sc[j];
            poly->y0 = band->sc[j] >> 16;
            poly->x1 = band->sc[j + 1];
            poly->y1 = band->sc[j + 1] >> 16;
            poly->x2 = band->sb[j];
            poly->y2 = band->sb[j] >> 16;
            poly->x3 = band->sb[j + 1];
            poly->y3 = band->sb[j + 1] >> 16;
            if (work->tail.phase >= 3) {
                poly->r0 = band->ca[j][0] * work->brightness / 1024;
                poly->g0 = band->ca[j][1] * work->brightness / 1024;
                poly->b0 = band->ca[j][2] * work->brightness / 1024;
                poly->r1 = band->ca[j + 1][0] * work->brightness / 1024;
                poly->g1 = band->ca[j + 1][1] * work->brightness / 1024;
                poly->b1 = band->ca[j + 1][2] * work->brightness / 1024;
                poly->r2 = band->cb[j][0] * work->brightness / 1024;
                poly->g2 = band->cb[j][1] * work->brightness / 1024;
                poly->b2 = band->cb[j][2] * work->brightness / 1024;
                poly->r3 = band->cb[j + 1][0] * work->brightness / 1024;
                poly->g3 = band->cb[j + 1][1] * work->brightness / 1024;
                poly->b3 = band->cb[j + 1][2] * work->brightness / 1024;
            } else {
                poly->r0 = band->ca[j][0];
                poly->g0 = band->ca[j][1];
                poly->b0 = band->ca[j][2];
                poly->r1 = band->ca[j + 1][0];
                poly->g1 = band->ca[j + 1][1];
                poly->b1 = band->ca[j + 1][2];
                poly->r2 = band->cb[j][0];
                poly->g2 = band->cb[j][1];
                poly->b2 = band->cb[j][2];
                poly->r3 = band->cb[j + 1][0];
                poly->g3 = band->cb[j + 1][1];
                poly->b3 = band->cb[j + 1][2];
            }
            if (band->otz[j] >= 0 && band->flag[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
        }
    }
    if (work->substep + 1 == *(u16 *)(*(u8 **)((u8 *)work + 0xF2C) + 0xC)) {
        if (work->tail.phase == 1) {
            if (work->progress < 0x401) {
                work->progress += work->step * 64;
                work->radius = 0x100;
                if (work->progress >= 0x400) {
                    work->progress = 0x400;
                    work->tail.phase = 2;
                }
            }
        } else if (work->tail.phase == 2) {
            if (work->radius < 0x401) {
                work->radius += work->step * 32;
                if (work->radius >= 0x400) {
                    work->radius = 0x400;
                    work->tail.phase = 3;
                }
            }
        } else if (work->tail.phase == 3) {
            if (work->brightness > 0) {
                work->brightness -= work->step * 32;
                if (work->brightness <= 0) {
                    work->brightness = 0;
                    work->tail.phase = 4;
                }
            }
        }
    }
}
