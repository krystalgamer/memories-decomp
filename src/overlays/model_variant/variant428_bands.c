#include "../../types.h"

#include "model_variant.h"
#include "variant428_entry.h"

/* Draws one band of nine segments as pairs of POLY_GT4 quads along the variant's
 * path, fading its radius in and out with the two phases timed by its
 * config record. */
void ModelVariant428_DrawBands(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag[1][9];
    Variant428EntryState *work;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *poly;
    ModelVariantColorBand *band;
    ModelVariantColorBand *col;
    s16 i;
    s16 j;
    s16 r;

    work = (Variant428EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    poly = work->packets.textured;
    angle = ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx) + 0x800;
    if (!(work->clock.frame_count & 1)) {
        r = work->scale * 48 / 4096;
    } else {
        r = work->scale * 56 / 4096;
    }
    band = work->bands;
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
                m.t[0] = work->matrix.t[0] + work->aim.direction.vx * work->progress / 1024 * j / 8;
                m.t[1] = work->matrix.t[1] + work->aim.direction.vy * work->progress / 1024 * j / 8;
                m.t[2] = work->matrix.t[2] + work->aim.direction.vz * work->progress / 1024 * j / 8;
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
            poly->x1 = col->sc[1];
            poly->y1 = col->sc[1] >> 16;
            poly->x2 = col->sb[0];
            poly->y2 = col->sb[0] >> 16;
            poly->x3 = col->sb[1];
            poly->y3 = col->sb[1] >> 16;
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
    if (work->tail.phase == 1) {
        if (work->progress < 0x401) {
            work->progress = ((work->clock.frame - work->config->field_20) << 10) /
                             (u32)(work->config->field_24 - work->config->field_20);
            if (work->progress >= 0x400) {
                work->progress = 0x400;
                work->tail.phase = 2;
            }
        }
    } else if (work->tail.phase < 3 && work->scale > 0) {
        if (work->clock.frame >= (u32)work->config->field_28) {
            work->scale = 0x1000 - ((work->clock.frame - work->config->field_28) << 12) /
                                          (u32)(work->config->field_2C - work->config->field_28);
            if (work->scale <= 0) {
                work->scale = 0;
                work->tail.phase = 3;
            }
        }
    }
}
