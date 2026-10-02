#include "../../types.h"
#include "variant459_grid.h"

/* Draws the header-459 grid at work + 0xFF8 (nine rows of seventeen points)
 * as POLY_GT4 strips shaded per row, sorted when the depth and the flag are
 * not negative. Before phase 5 the row colours fade from blue to red by the
 * scale at work + 0x27FC; the scale grows by step << 9 to 0x2000, swings
 * around 0x1800 on rcos in phase 3, grows to 0x4000 in phase 4 and fades out
 * in phase 5. The two ratan2 results are unused. */

void func_8013D800(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG p;
    PSXLONG flag;
    Variant459Grid *grid;
    GsOT *ot;
    u8 *work;
    POLY_GT4 *poly;
    s32 i;
    s32 j;
    s32 otz;

    work = ctx;
    ot = func_80058F10();
    ratan2(MODEL_VARIANT_WORD(work, 0x2778), MODEL_VARIANT_WORD(work, 0x2770));
    ratan2(MODEL_VARIANT_WORD(work, 0x2774), MODEL_VARIANT_WORD(work, 0x2778));
    grid = (Variant459Grid *)(work + 0xFF8);
    poly = (POLY_GT4 *)(work + 0x26D0);
    if (MODEL_VARIANT_HALF(work, 0x2860) == 0) {
        setVector(&rot, MODEL_VARIANT_HALF(work, 0x2804), 0, 0);
    } else {
        setVector(&rot, -MODEL_VARIANT_HALF(work, 0x2804), 0, 0);
    }
    m.t[0] = MODEL_VARIANT_HALF(work, 0x2758);
    m.t[1] = MODEL_VARIANT_HALF(work, 0x275A);
    m.t[2] = MODEL_VARIANT_HALF(work, 0x275C);
    scale.vx = MODEL_VARIANT_WORD(work, 0x27FC);
    scale.vy = MODEL_VARIANT_WORD(work, 0x27FC);
    scale.vz = MODEL_VARIANT_WORD(work, 0x27FC);
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    if (MODEL_VARIANT_WORD(work, 0x284C) < 5) {
        MODEL_VARIANT_WORD(work, 0x280C) = 0x400 - (MODEL_VARIANT_WORD(work, 0x27FC) - 0x2000) / 8;
        for (i = 0; i < 9; i++) {
            grid->color[i][0] = i * 255 / 8 * MODEL_VARIANT_WORD(work, 0x280C) / 1024;
            grid->color[i][1] = MODEL_VARIANT_WORD(work, 0x280C) / 16;
            grid->color[i][2] = (0xFF - i * 255 / 8) * MODEL_VARIANT_WORD(work, 0x280C) / 1024;
        }
    } else {
        for (i = 0; i < 9; i++) {
            grid->color[i][0] = MODEL_VARIANT_WORD(work, 0x280C) / 8;
            grid->color[i][1] = MODEL_VARIANT_WORD(work, 0x280C) / 8;
            grid->color[i][2] = MODEL_VARIANT_WORD(work, 0x280C) / 8;
        }
    }
    for (i = 0; i < 8; i++) {
        poly->r0 = grid->color[i][0];
        poly->g0 = grid->color[i][1];
        poly->b0 = grid->color[i][2];
        poly->r1 = grid->color[i][0];
        poly->g1 = grid->color[i][1];
        poly->b1 = grid->color[i][2];
        poly->r2 = grid->color[i + 1][0];
        poly->g2 = grid->color[i + 1][1];
        poly->b2 = grid->color[i + 1][2];
        poly->r3 = grid->color[i + 1][0];
        poly->g3 = grid->color[i + 1][1];
        poly->b3 = grid->color[i + 1][2];
        for (j = 0; j < 16; j++) {
            otz = RotTransPers4(&grid->point[i][j], &grid->point[i][j + 1], &grid->point[i + 1][j],
                                &grid->point[i + 1][j + 1], (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1,
                                (PSXLONG *)&poly->x2, (PSXLONG *)&poly->x3, &p, &flag);
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x2814) < 0x81) {
        MODEL_VARIANT_WORD(work, 0x2814) += 8;
        if (MODEL_VARIANT_WORD(work, 0x2814) >= 0x80) {
            MODEL_VARIANT_WORD(work, 0x2814) = 0;
        }
    }
    if (MODEL_VARIANT_WORD(work, 0x284C) < 3) {
        if (MODEL_VARIANT_WORD(work, 0x27FC) < 0x2000) {
            MODEL_VARIANT_WORD(work, 0x27FC) += MODEL_VARIANT_WORD(work, 0x27B8) << 9;
            if (MODEL_VARIANT_WORD(work, 0x27FC) >= 0x2000) {
                MODEL_VARIANT_WORD(work, 0x27FC) = 0x2000;
                MODEL_VARIANT_WORD(work, 0x284C) = 3;
            }
        }
    } else if (MODEL_VARIANT_WORD(work, 0x284C) == 3) {
        MODEL_VARIANT_WORD(work, 0x27FC) = (rcos(MODEL_VARIANT_WORD(work, 0x2808)) * 0x800 >> 12) + 0x1800;
        MODEL_VARIANT_WORD(work, 0x2808) += MODEL_VARIANT_WORD(work, 0x27B8) << 6;
        if (*(u32 *)(*(u8 *G32 *)(work + 0x27C0) + 0x34) < (u32)MODEL_VARIANT_WORD(work, 0x27B0)) {
            MODEL_VARIANT_WORD(work, 0x284C) = 4;
        }
    } else if (MODEL_VARIANT_WORD(work, 0x284C) == 4) {
        MODEL_VARIANT_WORD(work, 0x27FC) += MODEL_VARIANT_WORD(work, 0x27B8) << 8;
        if (MODEL_VARIANT_WORD(work, 0x27FC) >= 0x4000) {
            MODEL_VARIANT_WORD(work, 0x284C) = 5;
            MODEL_VARIANT_WORD(work, 0x280C) = 0x400;
        }
    } else if (MODEL_VARIANT_WORD(work, 0x284C) == 5) {
        MODEL_VARIANT_WORD(work, 0x27FC) += MODEL_VARIANT_WORD(work, 0x27B8) << 8;
        if (MODEL_VARIANT_WORD(work, 0x280C) > 0) {
            MODEL_VARIANT_WORD(work, 0x280C) -= MODEL_VARIANT_WORD(work, 0x27B8) << 5;
            if (MODEL_VARIANT_WORD(work, 0x280C) <= 0) {
                MODEL_VARIANT_WORD(work, 0x280C) = 0;
                MODEL_VARIANT_WORD(work, 0x284C) = 6;
            }
        }
    }
    MODEL_VARIANT_WORD(work, 0x2804) += MODEL_VARIANT_WORD(work, 0x27B8) << 6;
}
