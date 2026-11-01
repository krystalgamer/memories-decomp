#include "../../types.h"
#include "variant415_entry.h"

/* Header 415's ribbon helper: projects the two ends of the three entry
 * ribbons from each of the five anchors, grows the sweep amounts on the last
 * anchor, restarts them once field_30DC is set, and draws one Gouraud quad
 * per ribbon. */
void ModelVariant415_DrawRibbons(Variant415EntryRibbon *ribbon)
{
    SVECTOR rot;
    VECTOR scale;
    s16 j;
    MATRIX m;
    MATRIX ls;
    PSXLONG p;
    Variant415EntryState *work;
    GsCOORDINATE2 coord;
    GsOT *ot;
    s16 i;
    s32 angle;
    s16 k;
    POLY_GT4 *poly;
    PSXLONG flag[3][2];
    s32 amount;

    work = (Variant415EntryState *)ribbon;
    poly = (POLY_GT4 *)((u8 *)work + 0x2B90);
    ot = func_80058F10();
    for (i = 0; i < 5; i++) {
        angle = ratan2(work->screen_y[i], work->screen_x[i]) + 0x800;
        ribbon = work->ribbons;
        /* A no-op: the for statement below sets k anyway. Its branch lasts
         * until after register allocation, and the retail registers need
         * it: it splits this block, so the sign-extended i is allocated
         * globally (s2) rather than locally (s0), and it changes the
         * cursor's and the packet pointer's live ranges enough that the
         * packet pointer gets the higher priority and s0. */
        if (angle != 0) {
            k = 0;
        }
        for (k = 0; k < 3; k++, ribbon++) {
            for (j = 0; j < 2; j++) {
                amount = ribbon->amount[j];
                if (amount < 0) {
                    amount = 0;
                }
                rot.vx = 0;
                rot.vy = 0;
                rot.vz = angle;
                m.t[0] = work->matrices[i].t[0] + work->directions[i].vx * amount / 1024;
                m.t[1] = work->matrices[i].t[1] + work->directions[i].vy * amount / 1024;
                m.t[2] = work->matrices[i].t[2] + work->directions[i].vz * amount / 1024;
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
                ribbon->otz[j] = RotTransPers3(&ribbon->point[0][j], &ribbon->point[1][j],
                                               &ribbon->point[2][j], &ribbon->screen[0][j],
                                               &ribbon->screen[1][j], &ribbon->screen[2][j], &p,
                                               &flag[k][j]);
                if (ribbon->amount[j] < 0x400 && i + 1 == 5) {
                    ribbon->amount[j] += work->step * 64;
                    if (ribbon->amount[j] >= 0x400) {
                        ribbon->amount[j] = 0x400;
                        if (work->phase < 2) {
                            work->phase = 2;
                        }
                        if (k + 1 == 3 && j == 1) {
                            if (work->start_index < 3) {
                                work->start_index++;
                                work->field_30DC = j;
                            } else if (work->phase == 2) {
                                work->phase = k + 1;
                            }
                        }
                    }
                }
            }
        }
        if (work->field_30DC == 1 && i + 1 == 5) {
            ribbon = work->ribbons;
            for (k = 0; k < 3; k++, ribbon++) {
                for (j = 0; j < 2; j++) {
                    ribbon->amount[j] = -(j << 8) - (k << 9);
                }
            }
            work->field_30DC = 0;
        }
        ribbon = work->ribbons;
        for (k = 0; k < 3; ribbon++, k++) {
            poly->r0 = 255;
            poly->g0 = 255;
            poly->b0 = 255;
            poly->r1 = 16;
            poly->g1 = 16;
            poly->b1 = 16;
            poly->r2 = 255;
            poly->g2 = 255;
            poly->b2 = 255;
            poly->r3 = 16;
            poly->g3 = 16;
            poly->b3 = 16;
            for (j = 0; j < 1; j++) {
                poly->x0 = ribbon->screen[0][j];
                poly->y0 = ribbon->screen[0][j] >> 16;
                poly->x1 = ribbon->screen[0][j + 1];
                poly->y1 = ribbon->screen[0][j + 1] >> 16;
                poly->x2 = ribbon->screen[2][j];
                poly->y2 = ribbon->screen[2][j] >> 16;
                poly->x3 = ribbon->screen[2][j + 1];
                poly->y3 = ribbon->screen[2][j + 1] >> 16;
                if (ribbon->otz[j] >= 0 && flag[k][j] >= 0) {
                    GsSortPoly(poly, ot, ribbon->otz[j]);
                }
            }
        }
    }
}
