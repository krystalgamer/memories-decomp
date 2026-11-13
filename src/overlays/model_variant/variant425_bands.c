#include "../../types.h"

#include "model_variant.h"
#include "variant425_entry.h"

/* Draws one band of nine segments as pairs of POLY_GT4 quads along the variant's
 * path, fading its radius in and out with the two phases of the timing record at
 * (u8 *)work + 0x2714. */
void ModelVariant425_DrawBand(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant425EntryState *work;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *poly;
    ModelVariantBand *band;
    s16 i;
    s16 j;
    s16 r;

    work = (Variant425EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    poly = work->packets.textured;
    angle = ratan2(work->screen_delta.vy, work->screen_delta.vx) + 0x800;
    if (!(work->clock.frame_count & 1)) {
        r = work->field_272C / 64;
    } else {
        r = work->field_272C * 70 / 4096;
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
                m.t[0] = work->matrix.t[0] + work->direction.vx * work->field_2730 / 1024 * j / 8;
                m.t[1] = work->matrix.t[1] + work->direction.vy * work->field_2730 / 1024 * j / 8;
                m.t[2] = work->matrix.t[2] + work->direction.vz * work->field_2730 / 1024 * j / 8;
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
                                         &band->sc[j], &p, &flag);
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
            if (band->otz[j] > 0) {
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
            if (band->otz[j] > 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
        }
    }
    if (work->phase == 1) {
        if (work->field_2730 < 0x401) {
            work->field_2730 = (u32)((work->clock.frame - MODEL_VARIANT_WORD(work->config, 0x20)) << 10) /
                             (u32)(MODEL_VARIANT_WORD(work->config, 0x24) - MODEL_VARIANT_WORD(work->config, 0x20));
            if (work->field_2730 >= 0x400) {
                work->field_2730 = 0x400;
                work->phase = 2;
            }
        }
    } else if (work->phase < 3 && work->field_272C > 0) {
        if ((u32)work->clock.frame >= (u32)MODEL_VARIANT_WORD(work->config, 0x28)) {
            work->field_272C = 0x1000 - (((u32)work->clock.frame - MODEL_VARIANT_WORD(work->config, 0x28)) << 12) /
                                          (u32)(MODEL_VARIANT_WORD(work->config, 0x2C) - MODEL_VARIANT_WORD(work->config, 0x28));
            if (work->field_272C <= 0) {
                work->field_272C = 0;
                work->phase = 3;
            }
        }
    }
}
