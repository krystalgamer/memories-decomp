#include "../../types.h"

#include "variant416_bands.h"

/* Draws two pairs of POLY_GT4 wings, each a two-point band projected with
 * RotTransPers3, and grows each wing's level by step * 64 until both are
 * full; the phase word at work + 0x1A84 moves to 2 when the first wing is
 * full and to 3 when the last one is done. */
void func_8013C054(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flag[2][2];
    PSXLONG p;
    u8 *work;
    GsOT *ot;
    POLY_GT4 *poly;
    Variant416Band *band;
    s16 i;
    s16 j;
    s32 done;
    s32 angle;
    s16 r;
    s32 size;

    work = ctx;
    ot = func_80058F10();
    done = 1;
    angle = ratan2(MODEL_VARIANT_HALF(work, 0x1A32), MODEL_VARIANT_HALF(work, 0x1A30)) + 0x800;
    poly = (POLY_GT4 *)(work + 0x18E8);
    /* Both arms fold to the same division, so the test leaves no code; the
     * split assignment keeps the srl/sll/sra form of the radius. */
    if (!(MODEL_VARIANT_WORD(work, 0x1A4C) & 1)) {
        r = MODEL_VARIANT_HALF(work, 0x1A78) / 256;
    } else {
        r = MODEL_VARIANT_HALF(work, 0x1A78) * 16 / 4096;
    }
    band = (Variant416Band *)(work + 0xF60);
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
            m.t[0] = MODEL_VARIANT_WORD(work, 0x1A0C) + MODEL_VARIANT_WORD(work, 0x1A20) * size / 1024;
            m.t[1] = MODEL_VARIANT_WORD(work, 0x1A10) + MODEL_VARIANT_WORD(work, 0x1A24) * size / 1024;
            m.t[2] = MODEL_VARIANT_WORD(work, 0x1A14) + MODEL_VARIANT_WORD(work, 0x1A28) * size / 1024;
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
            if (MODEL_VARIANT_WORD(work, 0x1A84) < 3) {
                if (band->level[j] < 0x400) {
                    band->level[j] += MODEL_VARIANT_WORD(work, 0x1A58) << 6;
                    if (band->level[j] >= 0x400) {
                        if (i == 0 && j == 0 && MODEL_VARIANT_WORD(work, 0x1A84) < 2) {
                            MODEL_VARIANT_WORD(work, 0x1A84) = 2;
                        }
                        if (band->level[1] >= 0x400) {
                            if ((u32)MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x1A60), 0x28) <=
                                (u32)MODEL_VARIANT_WORD(work, 0x1A50)) {
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
                if (i + 1 == 2 && j == 1 && done == 1 && MODEL_VARIANT_WORD(work, 0x1A84) == 2) {
                    MODEL_VARIANT_WORD(work, 0x1A84) = 3;
                }
            }
        }
    }
    band = (Variant416Band *)(work + 0xF60);
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
