#include "../../types.h"

#include "variant416_bands.h"
#include "variant416_entry.h"

/* Draws two pairs of POLY_GT4 wings, each a two-point band projected with
 * RotTransPers3, and grows each wing's level by step * 64 until both are
 * full; the phase word at (u8 *)work + 0x1A84 moves to 2 when the first wing is
 * full and to 3 when the last one is done. */
void ModelVariant416_DrawBands(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long flag[2][2];
    long p;
    Variant416EntryState *work;
    GsOT *ot;
    POLY_GT4 *poly;
    Variant416Band *band;
    s16 i;
    s16 j;
    s32 done;
    s32 angle;
    s16 r;
    s32 size;

    work = (Variant416EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    done = 1;
    angle = ratan2(work->aim.screen_delta.vy, work->aim.screen_delta.vx) + 0x800;
    poly = work->packets.textured;
    /* Both arms fold to the same division, so the test leaves no code; the
     * split assignment keeps the srl/sll/sra form of the radius. */
    if (!(work->clock.frame_count & 1)) {
        r = work->field_1A78 / 256;
    } else {
        r = work->field_1A78 * 16 / 4096;
    }
    band = (Variant416Band *)work->bands;
    for (i = 0; i < 2; i++, band++) {
        for (j = 0; j < 2; j++) {
            if (band->level[j] <= 0) {
                size = 0;
            } else if (band->level[j] < 0x400) {
                size = band->level[j];
            } else {
                size = 0x400;
            }
            setVector(&band->a[j], rcos(0x400) * r >> 12, rsin(0x400) * r >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(0xC00) * r >> 12, rsin(0xC00) * r >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            m.t[0] = work->matrix.t[0] + work->aim.direction.vx * size / 1024;
            m.t[1] = work->matrix.t[1] + work->aim.direction.vy * size / 1024;
            m.t[2] = work->matrix.t[2] + work->aim.direction.vz * size / 1024;
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
            if (work->tail.phase < 3) {
                if (band->level[j] < 0x400) {
                    band->level[j] += work->clock.step << 6;
                    if (band->level[j] >= 0x400) {
                        if (i == 0 && j == 0 && work->tail.phase < 2) {
                            work->tail.phase = 2;
                        }
                        if (band->level[1] >= 0x400) {
                            if ((u32)work->config->field_28 <=
                                work->clock.frame) {
                                band->level[j] = 0x400;
                                band->done[j] = 1;
                            } else {
                                for (j = 0; j < 2; j++) {
                                    band->level[j] = band->level[j] - 0x400 - (j << 8);
                                }
                            }
                        }
                    }
                    /* After the reset loop j is 2 here, as in the original. */
                    done *= band->done[j];
                }
                if (i + 1 == 2 && j == 1 && done == 1 && work->tail.phase == 2) {
                    work->tail.phase = 3;
                }
            }
        }
    }
    band = (Variant416Band *)work->bands;
    for (i = 0; i < 2; i++, band++) {
        for (j = 0; j < 1; j++) {
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
            if (band->otz[j] >= 0 && flag[i][j] >= 0) {
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
            if (band->otz[j] >= 0 && flag[i][j] >= 0) {
                GsSortPoly(poly, ot, band->otz[j]);
            }
        }
    }
}
