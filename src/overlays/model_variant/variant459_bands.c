#include "../../types.h"

#include "../model_variant/model_variant.h"
#include "variant459_entry_pal.h"

/* Draws every band of the timing record's band count as nine-point strips of
 * paired POLY_GT4 quads along the variant's direction. The radius follows the
 * record's size and the entry's shrinking scale; the bands grow
 * along their own directions through phase 1 and shrink from the record's
 * fade window until phase 3. */
void SpanishModelVariant459_DrawTimedBands(Variant459EntryState *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag[6][9];
    Variant459EntryState *work;
    GsOT *ot;
    POLY_GT4 *poly;
    ModelVariantColorBand *band;
    ModelVariantColorBand *col;
    s16 i;
    s16 j;
    s16 r;
    s32 angle;

    work = context;
    ot = Graphics_GetDefaultOrderingTable();
    poly = work->packets.textured;
    angle = ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx) + 0x800;
    if (!(work->clock.frame_count & 1)) {
        r = work->scale * work->config->band_radius / 4096;
    } else {
        r = work->scale *
            (work->config->band_radius +
             (rsin(0x100) * work->config->band_radius >> 12)) / 4096;
    }
    band = work->bands;
    for (i = 0; i < work->config->count; i++, band++) {
        for (j = 0; j < 9; j++) {
            setVector(&band->a[j], rcos(0x400) * r >> 12, rsin(0x400) * r >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(0xC00) * r >> 12, rsin(0xC00) * r >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            if (j == 0) {
                m.t[0] = work->points[i].vx;
                m.t[1] = work->points[i].vy;
                m.t[2] = work->points[i].vz;
            } else {
                m.t[0] = work->points[i].vx + work->directions[i].vx * work->progress / 1024 * j / 8;
                m.t[1] = work->points[i].vy + work->directions[i].vy * work->progress / 1024 * j / 8;
                m.t[2] = work->points[i].vz + work->directions[i].vz * work->progress / 1024 * j / 8;
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
                                         &band->sc[j], &p, &flag[i][j]);
        }
    }
    band = work->bands;
    for (i = 0; i < work->config->count; i++, band++) {
        for (j = 0; j < 8; j++) {
            poly->x0 = band->sa[j];
            poly->y0 = band->sa[j] >> 16;
            poly->x1 = band->sa[j + 1];
            poly->y1 = band->sa[j + 1] >> 16;
            poly->x2 = band->sb[j];
            poly->y2 = band->sb[j] >> 16;
            poly->x3 = band->sb[j + 1];
            poly->y3 = band->sb[j + 1] >> 16;
            poly->r0 = band->inner[j].r;
            poly->g0 = band->inner[j].g;
            poly->b0 = band->inner[j].b;
            poly->r1 = band->inner[j + 1].r;
            poly->g1 = band->inner[j + 1].g;
            poly->b1 = band->inner[j + 1].b;
            poly->r2 = band->outer[j].r;
            poly->g2 = band->outer[j].g;
            poly->b2 = band->outer[j].b;
            poly->r3 = band->outer[j + 1].r;
            poly->g3 = band->outer[j + 1].g;
            poly->b3 = band->outer[j + 1].b;
            if (band->otz[j] < 0) {
                band->otz[j] = 0;
            }
            flag[i][j] = 0;
            if (band->otz[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
            /* Every field is a four-byte array, so moving the record pointer
             * by j words views column j as index 0. */
            col = (ModelVariantColorBand *)((s32 *)band + j);
            poly->x0 = col->sc[0];
            poly->y0 = col->sc[0] >> 16;
            poly->x1 = band->sc[j + 1];
            poly->y1 = band->sc[j + 1] >> 16;
            poly->x2 = col->sb[0];
            poly->y2 = col->sb[0] >> 16;
            poly->x3 = band->sb[j + 1];
            poly->y3 = band->sb[j + 1] >> 16;
            poly->r0 = col->inner[0].r;
            poly->g0 = col->inner[0].g;
            poly->b0 = col->inner[0].b;
            poly->r1 = band->inner[j + 1].r;
            poly->g1 = band->inner[j + 1].g;
            poly->b1 = band->inner[j + 1].b;
            poly->r2 = col->outer[0].r;
            poly->g2 = col->outer[0].g;
            poly->b2 = col->outer[0].b;
            poly->r3 = band->outer[j + 1].r;
            poly->g3 = band->outer[j + 1].g;
            poly->b3 = band->outer[j + 1].b;
            if (col->otz[0] < 0) {
                col->otz[0] = 0;
            }
            col = (ModelVariantColorBand *)((s32 *)band + j + 1);
            flag[i][j] = 0;
            if (band->otz[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
        }
    }
    col = NULL;
    if (work->phase == 1) {
        if (work->progress < 0x401) {
            work->progress = (u32)((work->clock.frame - work->config->end) << 10) /
                             (work->config->band_end - work->config->end);
            if (work->progress >= 0x400) {
                work->progress = 0x400;
                work->phase = 2;
            }
        }
    } else if (work->phase < 3 && work->scale > 0) {
        if ((u32)work->clock.frame >= work->config->fade_start) {
            work->scale = 0x1000 - (((u32)work->clock.frame - work->config->fade_start) << 12) /
                                          (u32)(work->config->fade_end - work->config->fade_start);
            if (work->scale <= 0) {
                work->scale = 0;
                work->phase = 3;
            }
        }
    }
}
