#include "../../types.h"
#include "variant481_band.h"

void FrenchModelVariant481_DrawBand(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Band481State *state = (Band481State *)context;
    Band481 *band;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 yaw;
    s32 progress;
    s32 width;
    s16 j;
    s16 side;
    s16 i;
    s32 unclamped;
    s16 rise;

    band = state->bands;
    ot = func_80058F10();
    yaw = ratan2(state->view_delta.vz, state->view_delta.vx) + 0xC00;
    ratan2(state->view_delta.vy, state->view_delta.vx);
    side = -1;
    if (state->slot == 0) {
        side = 1;
    }
    rise = 80;
    for (j = 0; j < 1; j++, band++) {
        for (i = 0; i < 33; i++) {
            progress = band->offset[i];
            unclamped = progress;
            if (progress < 0) {
                progress = 0;
            }
            if (progress < 512) {
                width = rise * progress / 512 + 16;
            } else {
                width = 96;
            }
            /* Retail keeps the unclamped offset in a short-lived register: this
             * dead re-read compiles away, leaving the copy before the clamp
             * test, and enlarges the loop so offset[0]/[32] stay addressed
             * from the band pointer. */
            if (unclamped < 0) {
                unclamped = state->bands[j].offset[i];
            }
            setVector(&band->spine[i], state->direction.vx * progress / 1024,
                      state->direction.vy * progress / 1024 + (rsin(progress * 2) * 512 >> 12),
                      state->direction.vz * progress / 1024);
            setVector(&band->edge[i], band->spine[i].vx + (rcos(yaw) * width >> 12), band->spine[i].vy,
                      band->spine[i].vz + (rsin(yaw) * width >> 12));
            if (band->offset[i] < 0x800) {
                band->offset[i] += state->step * 32;
                if (band->offset[0] >= 0x400 && state->phase < 2) {
                    state->phase = 2;
                }
                if (band->offset[i] >= 0x800) {
                    band->offset[i] = 0x800;
                }
                if (band->offset[32] >= 0x800 && state->phase == 2) {
                    state->phase = 6;
                }
            }
        }
    }
    setVector(&rotation, 0, 0, 0);
    matrix.t[0] = state->matrix.t[0];
    matrix.t[1] = state->matrix.t[1];
    matrix.t[2] = state->matrix.t[2];
    setVector(&scale, 4096, 4096, 4096);
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    band = state->bands;
    for (j = 0; j < 1; j++, band++) {
        for (i = 0; i < 33; i++) {
            if (i == 32) {
                band->otz[i] = RotTransPers4(&band->spine[i - 1], &band->spine[i], &band->spine[i - 1],
                                             &band->spine[i], &band->screen[i - 1],
                                             &band->screen[i], &band->screen[i - 1],
                                             &band->screen[i], &interpolation, &band->flag[i]);
                RotTransPers(&band->edge[i], &band->edge_screen[i], &interpolation, &flag);
                band->heading[i] = ratan2(state->screen_delta.vy, state->screen_delta.vx) - 0x400;
                band->length[i] = (s16)band->edge_screen[i] - (s16)band->screen[i];
                band->dx[i] = rcos(band->heading[i]) * band->length[i] >> 12;
                band->dy[i] = rsin(band->heading[i]) * band->length[i] >> 12;
            } else {
                band->otz[i] = RotTransPers4(&band->spine[i], &band->spine[i + 1], &band->spine[i],
                                             &band->spine[i + 1], &band->screen[i],
                                             &band->screen[i + 1], &band->screen[i],
                                             &band->screen[i + 1], &interpolation, &band->flag[i]);
                RotTransPers(&band->edge[i], &band->edge_screen[i], &interpolation, &flag);
                band->heading[i] = ratan2(state->screen_delta.vy, state->screen_delta.vx) - 0x400;
                band->length[i] = (s16)band->edge_screen[i] - (s16)band->screen[i];
                band->dx[i] = rcos(band->heading[i]) * band->length[i] >> 12;
                band->dy[i] = rsin(band->heading[i]) * band->length[i] >> 12;
            }
        }
    }
    band = state->bands;
    for (j = 0; j < 1; j++, band++) {
        quad = state->beams;
        for (i = 0; i < 32; i++) {
            quad->r0 = band->color[i].r;
            quad->g0 = band->color[i].g;
            quad->b0 = band->color[i].b;
            quad->r1 = band->color[i + 1].r;
            quad->g1 = band->color[i + 1].g;
            quad->b1 = band->color[i + 1].b;
            quad->r2 = band->color[i].r;
            quad->g2 = band->color[i].g;
            quad->b2 = band->color[i].b;
            quad->r3 = band->color[i + 1].r;
            quad->g3 = band->color[i + 1].g;
            quad->b3 = band->color[i + 1].b;
            if (state->angles[1] >= -0x400 && state->angles[1] < 0x400) {
                if (side == 1) {
                    quad->x0 = (u16)band->screen[i] - band->dx[i];
                    quad->y0 = (band->screen[i] >> 16) - band->dy[i];
                    quad->x1 = (u16)band->screen[i + 1] - band->dx[i + 1];
                    quad->y1 = (band->screen[i + 1] >> 16) - band->dy[i + 1];
                    quad->x2 = (u16)band->screen[i] + band->dx[i];
                    quad->y2 = (band->screen[i] >> 16) + band->dy[i];
                    quad->x3 = (u16)band->screen[i + 1] + band->dx[i + 1];
                    quad->y3 = (band->screen[i + 1] >> 16) + band->dy[i + 1];
                } else {
                    quad->x0 = (u16)band->screen[i] + band->dx[i];
                    quad->y0 = (band->screen[i] >> 16) + band->dy[i];
                    quad->x1 = (u16)band->screen[i + 1] + band->dx[i + 1];
                    quad->y1 = (band->screen[i + 1] >> 16) + band->dy[i + 1];
                    quad->x2 = (u16)band->screen[i] - band->dx[i];
                    quad->y2 = (band->screen[i] >> 16) - band->dy[i];
                    quad->x3 = (u16)band->screen[i + 1] - band->dx[i + 1];
                    quad->y3 = (band->screen[i + 1] >> 16) - band->dy[i + 1];
                }
            } else if (side == 1) {
                quad->x0 = (u16)band->screen[i] + band->dx[i];
                quad->y0 = (band->screen[i] >> 16) + band->dy[i];
                quad->x1 = (u16)band->screen[i + 1] + band->dx[i + 1];
                quad->y1 = (band->screen[i + 1] >> 16) + band->dy[i + 1];
                quad->x2 = (u16)band->screen[i] - band->dx[i];
                quad->y2 = (band->screen[i] >> 16) - band->dy[i];
                quad->x3 = (u16)band->screen[i + 1] - band->dx[i + 1];
                quad->y3 = (band->screen[i + 1] >> 16) - band->dy[i + 1];
            } else {
                quad->x0 = (u16)band->screen[i] - band->dx[i];
                quad->y0 = (band->screen[i] >> 16) - band->dy[i];
                quad->x1 = (u16)band->screen[i + 1] - band->dx[i + 1];
                quad->y1 = (band->screen[i + 1] >> 16) - band->dy[i + 1];
                quad->x2 = (u16)band->screen[i] + band->dx[i];
                quad->y2 = (band->screen[i] >> 16) + band->dy[i];
                quad->x3 = (u16)band->screen[i + 1] + band->dx[i + 1];
                quad->y3 = (band->screen[i + 1] >> 16) + band->dy[i + 1];
            }
            if (band->otz[i] >= 0 && band->flag[i] >= 0) {
                GsSortPoly(quad, ot, band->otz[i]);
            }
            if (i == 0) {
                quad++;
            }
        }
    }
}
