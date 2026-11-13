#include "../../types.h"
#include "variant459_grid.h"
#include "variant459_entry.h"

/* Draws the header-459 grid at (u8 *)work + 0xFF8 (nine rows of seventeen points)
 * as POLY_GT4 strips shaded per row, sorted when the depth and the flag are
 * not negative. Before phase 5 the row colours fade from blue to red by the
 * scale at (u8 *)work + 0x27FC; the scale grows by step << 9 to 0x2000, swings
 * around 0x1800 on rcos in phase 3, grows to 0x4000 in phase 4 and fades out
 * in phase 5. The two ratan2 results are unused. */

void ModelVariant459_DrawGlobe(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    ModelVariantMesh *grid;
    GsOT *ot;
    Variant459EntryState *work;
    POLY_GT4 *poly;
    s32 i;
    s32 j;
    s32 otz;

    work = (Variant459EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    ratan2(work->direction.vz, work->direction.vx);
    ratan2(work->direction.vy, work->direction.vz);
    grid = (ModelVariantMesh *)((u8 *)work + 0xFF8);
    poly = &work->veil;
    /* Retail reads only the signed low halfword of globe_rotation_x here. */
    if (work->slot == 0) {
        setVector(&rot, MODEL_VARIANT_HALF(work, 0x2804), 0, 0);
    } else {
        setVector(&rot, -MODEL_VARIANT_HALF(work, 0x2804), 0, 0);
    }
    m.t[0] = work->target.vx;
    m.t[1] = work->target.vy;
    m.t[2] = work->target.vz;
    scale.vx = work->globe_scale;
    scale.vy = work->globe_scale;
    scale.vz = work->globe_scale;
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    if (work->phase < 5) {
        work->globe_brightness = 0x400 - (work->globe_scale - 0x2000) / 8;
        for (i = 0; i < 9; i++) {
            grid->colors[i][0] = i * 255 / 8 * work->globe_brightness / 1024;
            grid->colors[i][1] = work->globe_brightness / 16;
            grid->colors[i][2] = (0xFF - i * 255 / 8) * work->globe_brightness / 1024;
        }
    } else {
        for (i = 0; i < 9; i++) {
            grid->colors[i][0] = work->globe_brightness / 8;
            grid->colors[i][1] = work->globe_brightness / 8;
            grid->colors[i][2] = work->globe_brightness / 8;
        }
    }
    for (i = 0; i < 8; i++) {
        poly->r0 = grid->colors[i][0];
        poly->g0 = grid->colors[i][1];
        poly->b0 = grid->colors[i][2];
        poly->r1 = grid->colors[i][0];
        poly->g1 = grid->colors[i][1];
        poly->b1 = grid->colors[i][2];
        poly->r2 = grid->colors[i + 1][0];
        poly->g2 = grid->colors[i + 1][1];
        poly->b2 = grid->colors[i + 1][2];
        poly->r3 = grid->colors[i + 1][0];
        poly->g3 = grid->colors[i + 1][1];
        poly->b3 = grid->colors[i + 1][2];
        for (j = 0; j < 16; j++) {
            otz = RotTransPers4(&grid->rows[i][j], &grid->rows[i][j + 1], &grid->rows[i + 1][j],
                                &grid->rows[i + 1][j + 1], (long *)&poly->x0, (long *)&poly->x1,
                                (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
    }
    if (work->field_2814 < 0x81) {
        work->field_2814 += 8;
        if (work->field_2814 >= 0x80) {
            work->field_2814 = 0;
        }
    }
    if (work->phase < 3) {
        if (work->globe_scale < 0x2000) {
            work->globe_scale += work->clock.step << 9;
            if (work->globe_scale >= 0x2000) {
                work->globe_scale = 0x2000;
                work->phase = 3;
            }
        }
    } else if (work->phase == 3) {
        work->globe_scale = (rcos(work->globe_pulse_angle) * 0x800 >> 12) + 0x1800;
        work->globe_pulse_angle += work->clock.step << 6;
        if (*(u32 *)(*(u8 **)((u8 *)work + 0x27C0) + 0x34) < (u32)MODEL_VARIANT_WORD(work, 0x27B0)) {
            work->phase = 4;
        }
    } else if (work->phase == 4) {
        work->globe_scale += work->clock.step << 8;
        if (work->globe_scale >= 0x4000) {
            work->phase = 5;
            work->globe_brightness = 0x400;
        }
    } else if (work->phase == 5) {
        work->globe_scale += work->clock.step << 8;
        if (work->globe_brightness > 0) {
            work->globe_brightness -= work->clock.step << 5;
            if (work->globe_brightness <= 0) {
                work->globe_brightness = 0;
                work->phase = 6;
            }
        }
    }
    work->globe_rotation_x += work->clock.step << 6;
}
