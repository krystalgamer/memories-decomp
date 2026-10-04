#include "../../types.h"
#include "variant450_bands.h"

void func_8013C794(u8 *context)
{
    Model450BandView *work = (Model450BandView *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG flags[3][9];
    PSXLONG interpolation;
    PSXLONG flag;
    GsOT *ot;
    Model450Band *band;
    POLY_FT4 *quad;
    s16 i, j, width;
    s32 turn, angle;
    s32 dx, dy;

    ot = func_80058F10();
    turn = ratan2(work->view_direction[2], work->view_direction[0]) + 3072;
    ratan2(work->view_direction[1], work->view_direction[0]);
    if (!(work->flags & 1)) {
        width = work->radius / 128;
    } else {
        width = work->radius * 12 / 1024;
    }
    band = work->bands;
    for (i = 0; i < 3; i++, band++) {
        for (j = 0, angle = 0; j < 9; j++, angle = j * 512) {
            setVector(&band->points[j],
                      work->delta[0] * i / 3 + (rcos(angle) * 256 >> 12),
                      work->delta[1] * i / 3 + (rsin(angle) * 256 >> 12),
                      work->delta[2] * i / 3);
            setVector(&band->edges[j],
                      band->points[j].vx + (rcos(turn) * width >> 12),
                      band->points[j].vy,
                      band->points[j].vz + (rsin(turn) * width >> 12));
        }
    }
    setVector(&rotation, 0, 0, 0);
    matrix.t[0] = work->origin[0];
    matrix.t[1] = work->origin[1];
    matrix.t[2] = work->origin[2];
    setVector(&scale, 4096, 4096, 4096);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    band = work->bands;
    for (i = 0; i < 3; i++, band++) {
        for (j = 0; j < 9; j++) {
            if (j == 8) {
                band->depth[j] = RotTransPers4(
                    &band->points[7], &band->points[8], &band->points[7], &band->points[8],
                    &band->screen[7].packed, &band->screen[8].packed,
                    &band->screen[7].packed, &band->screen[8].packed,
                    &interpolation, &flags[i][8]);
                RotTransPers(&band->edges[8], &band->edge_screen[8].packed, &interpolation, &flag);
                dx = band->screen[j].point.vx - band->screen[7].point.vx;
                dy = band->screen[j].point.vy - band->screen[7].point.vy;
                band->angle[j] = ratan2(dy, dx) - 1024;
                band->width[j] = band->edge_screen[j].point.vx - band->screen[j].point.vx;
                band->x_offset[8] = rcos(band->angle[j]) * band->width[j] >> 12;
                band->y_offset[8] = rsin(band->angle[j]) * band->width[j] >> 12;
            } else {
                band->depth[j] = RotTransPers4(
                    &band->points[j], &band->points[j + 1], &band->points[j], &band->points[j + 1],
                    &band->screen[j].packed, &band->screen[j + 1].packed,
                    &band->screen[j].packed, &band->screen[j + 1].packed,
                    &interpolation, &flags[i][j]);
                RotTransPers(&band->edges[j], &band->edge_screen[j].packed, &interpolation, &flag);
                dx = band->screen[j + 1].point.vx - band->screen[j].point.vx;
                dy = band->screen[j + 1].point.vy - band->screen[j].point.vy;
                band->angle[j] = ratan2(dy, dx) - 1024;
                band->width[j] = band->edge_screen[j].point.vx - band->screen[j].point.vx;
                band->x_offset[j] = rcos(band->angle[j]) * band->width[j] >> 12;
                band->y_offset[j] = rsin(band->angle[j]) * band->width[j] >> 12;
            }
        }
    }
    band = work->bands;
    for (i = 0; i < 3; i++, band++) {
        for (j = 0, quad = work->packets; j < 8; j++) {
            quad->x0 = band->screen[j].point.vx + band->x_offset[j];
            quad->y0 = (band->screen[j].packed >> 16) + band->y_offset[j];
            quad->x1 = band->screen[j + 1].point.vx + band->x_offset[j + 1];
            quad->y1 = (band->screen[j + 1].packed >> 16) + band->y_offset[j + 1];
            quad->x2 = band->screen[j].point.vx - band->x_offset[j];
            quad->y2 = (band->screen[j].packed >> 16) - band->y_offset[j];
            quad->x3 = band->screen[j + 1].point.vx - band->x_offset[j + 1];
            quad->y3 = (band->screen[j + 1].packed >> 16) - band->y_offset[j + 1];
            setRGB0(quad, band->color.r, band->color.g, band->color.b);
            if (band->depth[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(quad, ot, (u16)band->depth[j]);
            }
            if (!(j & 1)) {
                quad++;
            } else {
                quad--;
            }
        }
    }
}
