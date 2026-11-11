#include "../../types.h"

#include "variant433_strips.h"
#include "../model_variant/variant416_entry.h"

void ModelVariant416_DrawBands(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long flag[2][2];
    long p;
    GsOT *ot;
    s16 i;
    s16 j;
    Variant416EntryState *work;
    struct ModelVariant433Strip *strip;
    POLY_GT4 *poly;
    s32 done;
    s32 angle;
    s16 radius;
    s32 raw_radius;
    s32 amount;

    work = (Variant416EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    done = 1;
    angle = ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx) + 0x800;
    poly = work->textured;
    raw_radius = work->field_1A78;
    /* Branch-local shifts retain the target's signed16 narrowing. */
    radius = raw_radius < 0 ? (u32)(raw_radius + 255) >> 8 : (u32)raw_radius >> 8;
    strip = (struct ModelVariant433Strip *)work->bands;
    for (i = 0; i < 2; i++, strip++) {
        for (j = 0; j < 2; j++) {
            amount = 0;
            if (strip->progress[j] > 0) {
                amount = 0x400;
                if (strip->progress[j] < amount) {
                    amount = strip->progress[j];
                }
            }
            setVector(&strip->a[j], rcos(0x400) * radius >> 12, rsin(0x400) * radius >> 12, 0);
            setVector(&strip->b[j], 0, 0, 0);
            setVector(&strip->c[j], rcos(0xC00) * radius >> 12, rsin(0xC00) * radius >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            m.t[0] = work->matrix.t[0] + work->aim.direction.vx * amount / 0x400;
            m.t[1] = work->matrix.t[1] + work->aim.direction.vy * amount / 0x400;
            m.t[2] = work->matrix.t[2] + work->aim.direction.vz * amount / 0x400;
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
            strip->otz[j] = RotTransPers3(&strip->a[j], &strip->b[j], &strip->c[j],
                                        &strip->sa[j], &strip->sb[j], &strip->sc[j], &p, &flag[i][j]);
            if (work->tail.phase < 3) {
                if (strip->progress[j] < 0x400) {
                    strip->progress[j] += work->clock.step << 6;
                    if (strip->progress[j] >= 0x400) {
                        if (i == 0 && j == 0 && work->tail.phase < 2) {
                            work->tail.phase = 2;
                        }
                        if (strip->progress[1] >= 0x400) {
                            if ((u32)MODEL_VARIANT_WORD(*(u8 **)((u8 *)work + 0x1A60), 0x28) <=
                                (u32)work->clock.frame) {
                                strip->progress[j] = 0x400;
                                strip->done[j] = 1;
                            } else {
                                /* Retail reuses j; the next word view then reads color at +0x78. */
                                for (j = 0; j < 2; j++) {
                                    s32 *column = (s32 *)strip + j;
                                    s32 decrement = (j << 8) + 0x400;
                                    MODEL_VARIANT_WORD(column, 0x68) -= decrement;
                                }
                            }
                        }
                    }
                    {
                        s32 *column = (s32 *)strip + j;
                        done *= MODEL_VARIANT_WORD(column, 0x70);
                    }
                }
                if (i + 1 == 2 && j == 1 && done == 1 && work->tail.phase == 2) {
                    work->tail.phase = 3;
                }
            }
        }
    }
    strip = (struct ModelVariant433Strip *)work->bands;
    for (i = 0; i < 2; i++, strip++) {
        for (j = 0; j < 1; j++) {
            poly->x0 = strip->sa[j];
            poly->y0 = strip->sa[j] >> 16;
            poly->x1 = strip->sa[j + 1];
            poly->y1 = strip->sa[j + 1] >> 16;
            poly->x2 = strip->sb[j];
            poly->y2 = strip->sb[j] >> 16;
            poly->x3 = strip->sb[j + 1];
            poly->y3 = strip->sb[j + 1] >> 16;
            poly->r0 = strip->ca[j][0];
            poly->g0 = strip->ca[j][1];
            poly->b0 = strip->ca[j][2];
            poly->r1 = strip->ca[j + 1][0];
            poly->g1 = strip->ca[j + 1][1];
            poly->b1 = strip->ca[j + 1][2];
            poly->r2 = strip->cb[j][0];
            poly->g2 = strip->cb[j][1];
            poly->b2 = strip->cb[j][2];
            poly->r3 = strip->cb[j + 1][0];
            poly->g3 = strip->cb[j + 1][1];
            poly->b3 = strip->cb[j + 1][2];
            if (strip->otz[j] >= 0 && flag[i][j] >= 0) {
                GsSortPoly(poly, ot, strip->otz[j]);
            }
            poly->x0 = strip->sc[j];
            poly->y0 = strip->sc[j] >> 16;
            poly->x1 = strip->sc[j + 1];
            poly->y1 = strip->sc[j + 1] >> 16;
            poly->x2 = strip->sb[j];
            poly->y2 = strip->sb[j] >> 16;
            poly->x3 = strip->sb[j + 1];
            poly->y3 = strip->sb[j + 1] >> 16;
            poly->r0 = strip->ca[j][0];
            poly->g0 = strip->ca[j][1];
            poly->b0 = strip->ca[j][2];
            poly->r1 = strip->ca[j + 1][0];
            poly->g1 = strip->ca[j + 1][1];
            poly->b1 = strip->ca[j + 1][2];
            poly->r2 = strip->cb[j][0];
            poly->g2 = strip->cb[j][1];
            poly->b2 = strip->cb[j][2];
            poly->r3 = strip->cb[j + 1][0];
            poly->g3 = strip->cb[j + 1][1];
            poly->b3 = strip->cb[j + 1][2];
            if (strip->otz[j] >= 0 && flag[i][j] >= 0) {
                GsSortPoly(poly, ot, strip->otz[j]);
            }
        }
    }
}
