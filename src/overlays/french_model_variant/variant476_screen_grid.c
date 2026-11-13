#include "../../types.h"
#include "variant476_screen_grid.h"
#include "../model_variant/variant459_entry.h"

void FrenchModelVariant476_DrawScreenGrid(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    long p;
    long flag;
    Variant476ScreenGrid *grid;
    GsOT *ot;
    POLY_GT4 *poly;
    Variant459EntryState *work;
    s32 i;
    s32 j;
    s32 active;
    s32 tpage;
    s32 otz;

    work = (Variant459EntryState *)ctx;
    ot = Graphics_GetDefaultOrderingTable();
    active = GsGetActiveBuff();
    rot.vx = -(s16)work->facing - 1024;
    rot.vy = 0;
    rot.vz = 0;
    m.t[0] = work->matrix.t[0];
    m.t[1] = work->matrix.t[1];
    m.t[2] = work->matrix.t[2];
    scale.vx = 4096;
    scale.vy = 4096;
    scale.vz = 4096;
    RotMatrix(&rot, &m);
    ScaleMatrix(&m, &scale);
    coord.coord = m;
    coord.super = 0;
    coord.flg = 0;
    GsGetLs(&coord, &ls);
    GsSetLsMatrix(&ls);
    grid = (Variant476ScreenGrid *)((u8 *)work + 0x14E4);
    poly = (POLY_GT4 *)((u8 *)work + 0x2390);
    if (work->phase < 5) {
        work->screen_grid_brightness = 1024 - (work->field_2818 - 8192) / 8;
        for (i = 0; i < 9; i++) {
            grid->color[i][0] = (i * 255 / 8) * work->screen_grid_brightness / 1024;
            grid->color[i][1] = work->screen_grid_brightness / 16;
            grid->color[i][2] = (255 - i * 255 / 8) * work->screen_grid_brightness / 1024;
        }
    } else {
        for (i = 0; i < 9; i++) {
            grid->color[i][0] = work->screen_grid_brightness / 8;
            grid->color[i][1] = work->screen_grid_brightness / 8;
            grid->color[i][2] = work->screen_grid_brightness / 8;
        }
    }
    for (i = 0; i < 8; i++) {
        setRGB0(poly, grid->color[i][0], grid->color[i][1], grid->color[i][2]);
        setRGB1(poly, grid->color[i][0], grid->color[i][1], grid->color[i][2]);
        setRGB2(poly, grid->color[i + 1][0], grid->color[i + 1][1], grid->color[i + 1][2]);
        setRGB3(poly, grid->color[i + 1][0], grid->color[i + 1][1], grid->color[i + 1][2]);
        for (j = 0; j < 8; j++) {
            RotTransPers4(&grid->b[i][j], &grid->b[i][j + 1],
                          &grid->b[i + 1][j], &grid->b[i + 1][j + 1],
                          (long *)&poly->x0, (long *)&poly->x1,
                          (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
            if (poly->x0 < 160) {
                /* Both buffer branches fold to identical page arguments. */
                if (active == 0) {
                    tpage = GetTPage(2, 1, 320, 0);
                } else {
                    tpage = GetTPage(2, 1, 320, 0);
                }
                SetPolyGT4(poly);
                poly->tpage = tpage;
                setUV4(poly, poly->x0, poly->y0, poly->x1, poly->y1,
                       poly->x2, poly->y2, poly->x3, poly->y3);
            } else {
                if (active == 0) {
                    tpage = GetTPage(2, 1, 448, 0);
                } else {
                    tpage = GetTPage(2, 1, 448, 0);
                }
                SetPolyGT4(poly);
                poly->tpage = tpage;
                setUV4(poly, poly->x0 - 128, poly->y0, poly->x1 - 128, poly->y1,
                       poly->x2 - 128, poly->y2, poly->x3 - 128, poly->y3);
            }
            otz = RotTransPers4(&grid->a[i][j], &grid->a[i][j + 1],
                               &grid->a[i + 1][j], &grid->a[i + 1][j + 1],
                               (long *)&poly->x0, (long *)&poly->x1,
                               (long *)&poly->x2, (long *)&poly->x3, &p, &flag);
            SetSemiTrans(poly, 0);
            SetShadeTex(poly, 0);
            if (otz >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, otz);
            }
        }
    }
}
