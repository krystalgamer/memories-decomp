#include "../../types.h"

#include "model_variant.h"
#include "variant398_entry.h"

/* Draws one band of nine segments as pairs of POLY_GT4 quads along the variant's
 * path, fading its radius in and out with the two phases of the timing record at
 * (u8 *)work + 0x1A00. */
void ModelVariant398_DrawBand(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag[1][9];
    Variant398EntryState *work;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *poly;
    ModelVariantBand *band;
    ModelVariantBand *col;
    s16 i;
    s16 j;
    s16 r;

    work = (Variant398EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    poly = work->textured;
    angle = ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx) + 0x800;
    if (!(work->frame_count & 1)) {
        r = work->band_scale / 64;
    } else {
        r = work->band_scale * 70 / 4096;
    }
    band = (ModelVariantBand *)work->bands;
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
                m.t[0] = work->matrix.t[0] + work->aim.direction.vx * work->displacement / 1024 * j / 8;
                m.t[1] = work->matrix.t[1] + work->aim.direction.vy * work->displacement / 1024 * j / 8;
                m.t[2] = work->matrix.t[2] + work->aim.direction.vz * work->displacement / 1024 * j / 8;
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
    band = (ModelVariantBand *)work->bands;
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
            if (band->otz[j] < 0) {
                band->otz[j] = 0;
            }
            flag[i][j] = 0;
            if (band->otz[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
            /* Every field is a four-byte array, so moving the record pointer
             * by j words views column j as index 0. */
            col = (ModelVariantBand *)((s32 *)band + j);
            poly->x0 = col->sc[0];
            poly->y0 = col->sc[0] >> 16;
#if defined(VERSION_FRENCH)
            poly->x1 = band->sc[j + 1];
            poly->y1 = band->sc[j + 1] >> 16;
#else
            poly->x1 = col->sc[1];
            poly->y1 = col->sc[1] >> 16;
#endif
            poly->x2 = col->sb[0];
            poly->y2 = col->sb[0] >> 16;
#if defined(VERSION_FRENCH)
            poly->x3 = band->sb[j + 1];
            poly->y3 = band->sb[j + 1] >> 16;
#else
            poly->x3 = col->sb[1];
            poly->y3 = col->sb[1] >> 16;
#endif
            poly->r0 = col->ca[0][0];
            poly->g0 = col->ca[0][1];
            poly->b0 = col->ca[0][2];
            poly->r1 = band->ca[j + 1][0];
            poly->g1 = band->ca[j + 1][1];
            poly->b1 = band->ca[j + 1][2];
            poly->r2 = col->cb[0][0];
            poly->g2 = col->cb[0][1];
            poly->b2 = col->cb[0][2];
            poly->r3 = band->cb[j + 1][0];
            poly->g3 = band->cb[j + 1][1];
            poly->b3 = band->cb[j + 1][2];
            if (col->otz[0] < 0) {
                col->otz[0] = 0;
            }
            col = (ModelVariantBand *)((s32 *)band + j + 1);
            flag[i][j] = 0;
            if (band->otz[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
        }
    }
    col = NULL;
    if (work->phase == 1) {
        if (work->displacement < 0x401) {
            work->displacement = (u32)((work->frame - work->config->burst_start) << 10) /
                             (u32)(MODEL_VARIANT_WORD(work->config, 0x24) - work->config->burst_start);
            if (work->displacement >= 0x400) {
                work->displacement = 0x400;
                work->phase = 2;
            }
        }
    } else if (work->phase < 3 && work->band_scale > 0) {
        if ((u32)work->frame >= (u32)MODEL_VARIANT_WORD(work->config, 0x28)) {
            work->band_scale = 0x1000 - (u32)(((u32)work->frame - MODEL_VARIANT_WORD(work->config, 0x28)) << 12) /
                                          (u32)(MODEL_VARIANT_WORD(work->config, 0x2C) - MODEL_VARIANT_WORD(work->config, 0x28));
            if (work->band_scale <= 0) {
                work->band_scale = 0;
                work->phase = 3;
            }
        }
    }
}
