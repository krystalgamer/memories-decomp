#include "../../types.h"

#include "variant324_bands.h"

/* Draws one band of nine points as pairs of POLY_GT4 quads along the variant's
 * path, its radius set by work + 0xF44 and its colours faded by work + 0xF46
 * from phase 3. When the timing record's last entry is reached the band
 * stretches (phase 1), widens (phase 2) and then fades out (phase 3) by
 * frame steps. */
void func_8013C7D4(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    u8 *work;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *poly;
    Variant324Band *band;
    s16 i;
    s16 j;
    s16 r;

    work = ctx;
    ot = func_80058F10();
    band = (Variant324Band *)(work + 0x4E0);
    poly = (POLY_GT4 *)(work + 0xDB4);
    angle = ratan2(MODEL_VARIANT_HALF(work, 0xEFE), MODEL_VARIANT_HALF(work, 0xEFC)) + 0x800;
    r = MODEL_VARIANT_HALF(work, 0xF44) / 32;
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 9; j++) {
            setVector(&band->a[j], rcos(0x400) * r >> 12, rsin(0x400) * r >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(0xC00) * r >> 12, rsin(0xC00) * r >> 12, 0);
            rot.vx = 0;
            rot.vy = 0;
            rot.vz = angle;
            if (j == 0) {
                m.t[0] = MODEL_VARIANT_WORD(work, 0xED8);
                m.t[1] = MODEL_VARIANT_WORD(work, 0xEDC);
                m.t[2] = MODEL_VARIANT_WORD(work, 0xEE0);
            } else {
                m.t[0] = MODEL_VARIANT_WORD(work, 0xED8) + MODEL_VARIANT_WORD(work, 0xEEC) * j / 8 * MODEL_VARIANT_WORD(work, 0xF48) / 1024;
                m.t[1] = MODEL_VARIANT_WORD(work, 0xEDC) + MODEL_VARIANT_WORD(work, 0xEF0) * j / 8 * MODEL_VARIANT_WORD(work, 0xF48) / 1024;
                m.t[2] = MODEL_VARIANT_WORD(work, 0xEE0) + MODEL_VARIANT_WORD(work, 0xEF4) * j / 8 * MODEL_VARIANT_WORD(work, 0xF48) / 1024;
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
    band = (Variant324Band *)(work + 0x4E0);
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
            if (MODEL_VARIANT_WORD(work, 0xF50) >= 3) {
                poly->r0 = band->ca[j][0] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->g0 = band->ca[j][1] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->b0 = band->ca[j][2] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->r1 = band->ca[j + 1][0] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->g1 = band->ca[j + 1][1] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->b1 = band->ca[j + 1][2] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->r2 = band->cb[j][0] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->g2 = band->cb[j][1] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->b2 = band->cb[j][2] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->r3 = band->cb[j + 1][0] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->g3 = band->cb[j + 1][1] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->b3 = band->cb[j + 1][2] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
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
            if (MODEL_VARIANT_WORD(work, 0xF50) >= 3) {
                poly->r0 = band->ca[j][0] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->g0 = band->ca[j][1] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->b0 = band->ca[j][2] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->r1 = band->ca[j + 1][0] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->g1 = band->ca[j + 1][1] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->b1 = band->ca[j + 1][2] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->r2 = band->cb[j][0] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->g2 = band->cb[j][1] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->b2 = band->cb[j][2] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->r3 = band->cb[j + 1][0] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->g3 = band->cb[j + 1][1] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
                poly->b3 = band->cb[j + 1][2] * MODEL_VARIANT_HALF(work, 0xF46) / 1024;
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
    if (MODEL_VARIANT_HALF(work, 0xF40) + 1 == *(u16 *)(*(u8 *G32 *)(work + 0xF2C) + 0xC)) {
        if (MODEL_VARIANT_WORD(work, 0xF50) == 1) {
            if (MODEL_VARIANT_WORD(work, 0xF48) < 0x401) {
                MODEL_VARIANT_WORD(work, 0xF48) += MODEL_VARIANT_WORD(work, 0xF24) * 64;
                MODEL_VARIANT_HALF(work, 0xF44) = 0x100;
                if (MODEL_VARIANT_WORD(work, 0xF48) >= 0x400) {
                    MODEL_VARIANT_WORD(work, 0xF48) = 0x400;
                    MODEL_VARIANT_WORD(work, 0xF50) = 2;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0xF50) == 2) {
            if (MODEL_VARIANT_HALF(work, 0xF44) < 0x401) {
                MODEL_VARIANT_HALF(work, 0xF44) += MODEL_VARIANT_WORD(work, 0xF24) * 32;
                if (MODEL_VARIANT_HALF(work, 0xF44) >= 0x400) {
                    MODEL_VARIANT_HALF(work, 0xF44) = 0x400;
                    MODEL_VARIANT_WORD(work, 0xF50) = 3;
                }
            }
        } else if (MODEL_VARIANT_WORD(work, 0xF50) == 3) {
            if (MODEL_VARIANT_HALF(work, 0xF46) > 0) {
                MODEL_VARIANT_HALF(work, 0xF46) -= MODEL_VARIANT_WORD(work, 0xF24) * 32;
                if (MODEL_VARIANT_HALF(work, 0xF46) <= 0) {
                    MODEL_VARIANT_HALF(work, 0xF46) = 0;
                    MODEL_VARIANT_WORD(work, 0xF50) = 4;
                }
            }
        }
    }
}
