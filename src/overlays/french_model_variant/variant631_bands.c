#include "../../types.h"

#include "../model_variant/variant423_bands.h"
#include "../model_variant/variant423_entry.h"

/* Stored timing/configuration pointer at (u8 *)work + 0xF00. */
#define BAND631_CONFIG(work) (*(u8 **)((u8 *)(work) + 0xF00))

/* French MODEL631/262 band helper. Same drawing as the NA variant423 band, but
 * the second quad of each segment reads its columns through col/next views of
 * the band. The redundant next/col copies and the trailing NULL stores keep
 * GCC 2.8.1 from merging those views with the first quad's addresses. */
void ModelVariant423_DrawBand(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag[1][5];
    Variant423EntryState *work;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *poly;
    Variant423Band *band;
    Variant423Band *col;
    Variant423Band *next;
    s16 i;
    s16 j;
    s16 r;

    work = (Variant423EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    poly = work->packets.textured;
    angle = ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx) + 0x800;
    if (!(work->clock.frame_count & 1)) {
        r = work->scale * 48 / 4096 +
            (rsin(work->band_wave) * (work->scale / 256) >> 12);
    } else {
        r = work->scale * 54 / 4096 +
            (rsin(work->band_wave) * (work->scale / 256) >> 12);
    }
    work->band_wave += work->clock.step * 256;
    band = (Variant423Band *)work->bands;
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 5; j++) {
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
                m.t[0] = work->matrix.t[0] + work->aim.direction.vx * work->progress / 1024 * j / 4;
                m.t[1] = work->matrix.t[1] + work->aim.direction.vy * work->progress / 1024 * j / 4;
                m.t[2] = work->matrix.t[2] + work->aim.direction.vz * work->progress / 1024 * j / 4;
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
    band = (Variant423Band *)work->bands;
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 4; j++) {
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
                GsSortPoly(poly, ot, band->otz[j + 1]);
            }
            col = (Variant423Band *)((s32 *)band + j);
            next = (Variant423Band *)((u8 *)band + (j + 1) * 4);
            poly->x0 = col->sc[0];
            poly->y0 = col->sc[0] >> 16;
            poly->x1 = next->sc[0];
            poly->y1 = next->sc[0] >> 16;
            poly->x2 = col->sb[0];
            poly->y2 = col->sb[0] >> 16;
            poly->x3 = next->sb[0];
            poly->y3 = next->sb[0] >> 16;
            poly->r0 = col->ca[0][0];
            poly->g0 = col->ca[0][1];
            poly->b0 = col->ca[0][2];
            poly->r1 = next->ca[0][0];
            poly->g1 = next->ca[0][1];
            poly->b1 = next->ca[0][2];
            poly->r2 = col->cb[0][0];
            poly->g2 = col->cb[0][1];
            poly->b2 = col->cb[0][2];
            poly->r3 = next->cb[0][0];
            poly->g3 = next->cb[0][1];
            poly->b3 = next->cb[0][2];
            if (col->otz[0] < 0) {
                next = col;
                next->otz[0] = 0;
                col = next;
            }
            flag[i][j] = 0;
            if (band->otz[j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j + 1]);
            }
        }
    }
    col = NULL;
    next = NULL;
    if (work->tail.phase == 1) {
        if (work->progress < 0x401) {
            work->progress = ((work->clock.frame - MODEL_VARIANT_WORD(BAND631_CONFIG(work), 0x20)) << 10) /
                             (u32)(MODEL_VARIANT_WORD(BAND631_CONFIG(work), 0x24) - MODEL_VARIANT_WORD(BAND631_CONFIG(work), 0x20));
            if (work->progress >= 0x400) {
                work->progress = 0x400;
                work->tail.phase = 2;
            }
        }
    } else if (work->tail.phase < 3 && work->scale > 0) {
        if (work->clock.frame >= (u32)MODEL_VARIANT_WORD(BAND631_CONFIG(work), 0x28)) {
            work->scale = 0x1000 - ((work->clock.frame - MODEL_VARIANT_WORD(BAND631_CONFIG(work), 0x28)) << 12) /
                                          (u32)(MODEL_VARIANT_WORD(BAND631_CONFIG(work), 0x2C) - MODEL_VARIANT_WORD(BAND631_CONFIG(work), 0x28));
            if (work->scale <= 0) {
                work->scale = 0;
                work->tail.phase = 3;
            }
        }
    }
}
