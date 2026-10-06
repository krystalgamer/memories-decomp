#include "../../types.h"
#include "variant438_bands.h"
#include "../../game/gpu_packets.h"

void func_8013E638(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag[1][9];
    Bands438State *state;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *quad;
    ModelVariantBand *band;
    s16 i;
    s16 j;
    s16 radius;

    state = (Bands438State *)context;
    ot = func_80058F10();
    quad = &state->quad;
    angle = ratan2(state->projected.vy, state->projected.vx) + 2048;
    if (!(state->frame & 1)) {
        radius = state->size * state->timing->radius / 4096;
    } else {
        radius = state->size * (state->timing->radius * 18 / 16) / 4096;
    }
    band = state->bands;
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 9; j++) {
            setVector(&band->a[j], rcos(1024) * radius >> 12, rsin(1024) * radius >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(3072) * radius >> 12, rsin(3072) * radius >> 12, 0);
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = angle;
            if (j == 0) {
                matrix.t[0] = state->origin[0];
                matrix.t[1] = state->origin[1];
                matrix.t[2] = state->origin[2];
            } else {
                matrix.t[0] = state->origin[0] + state->direction[0] * state->progress / 1024 * j / 8;
                matrix.t[1] = state->origin[1] + state->direction[1] * state->progress / 1024 * j / 8;
                matrix.t[2] = state->origin[2] + state->direction[2] * state->progress / 1024 * j / 8;
            }
            scale.vx = 4096;
            scale.vy = 4096;
            scale.vz = 4096;
            RotMatrix(&rotation, &matrix);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            ReadRotMatrix(&light);
            RotMatrix(&rotation, &light);
            ScaleMatrix(&light, &scale);
            SetRotMatrix(&light);
            band->otz[j] = RotTransPers3(&band->a[j], &band->b[j], &band->c[j],
                                         &band->sa[j], &band->sb[j], &band->sc[j],
                                         &interpolation, &flag[i][j]);
        }
    }
    band = state->bands;
    for (i = 0; i < 1; i++, band++) {
        for (j = 0; j < 8; j++) {
            quad->x0 = band->sa[j];
            quad->y0 = band->sa[j] >> 16;
            quad->x1 = band->sa[j + 1];
            quad->y1 = band->sa[j + 1] >> 16;
            quad->x2 = band->sb[j];
            quad->y2 = band->sb[j] >> 16;
            quad->x3 = band->sb[j + 1];
            quad->y3 = band->sb[j + 1] >> 16;
            quad->r0 = band->ca[j][0];
            quad->g0 = band->ca[j][1];
            quad->b0 = band->ca[j][2];
            quad->r1 = band->ca[j + 1][0];
            quad->g1 = band->ca[j + 1][1];
            quad->b1 = band->ca[j + 1][2];
            quad->r2 = band->cb[j][0];
            quad->g2 = band->cb[j][1];
            quad->b2 = band->cb[j][2];
            quad->r3 = band->cb[j + 1][0];
            quad->g3 = band->cb[j + 1][1];
            quad->b3 = band->cb[j + 1][2];
            if (band->otz[j] >= 0 && flag[i][j] >= 0) {
                GsSortPoly(quad, ot, band->otz[j]);
            }
            quad->x0 = band->sc[j];
            quad->y0 = band->sc[j] >> 16;
            quad->x1 = band->sc[j + 1];
            quad->y1 = band->sc[j + 1] >> 16;
            quad->x2 = band->sb[j];
            quad->y2 = band->sb[j] >> 16;
            quad->x3 = band->sb[j + 1];
            quad->y3 = band->sb[j + 1] >> 16;
            quad->r0 = band->ca[j][0];
            quad->g0 = band->ca[j][1];
            quad->b0 = band->ca[j][2];
            quad->r1 = band->ca[j + 1][0];
            quad->g1 = band->ca[j + 1][1];
            quad->b1 = band->ca[j + 1][2];
            quad->r2 = band->cb[j][0];
            quad->g2 = band->cb[j][1];
            quad->b2 = band->cb[j][2];
            quad->r3 = band->cb[j + 1][0];
            quad->g3 = band->cb[j + 1][1];
            quad->b3 = band->cb[j + 1][2];
            if (band->otz[j] >= 0 && flag[i][j] >= 0) {
                GsSortPoly(quad, ot, band->otz[j]);
            }
        }
    }
    if (state->phase == 1) {
        if (state->progress < 1025) {
            state->progress = ((state->time - state->timing->grow_start) << 10) /
                              (state->timing->grow_end - state->timing->grow_start);
            if (state->progress >= 1024) {
                state->progress = 1024;
                state->phase = 2;
            }
        }
    } else if (state->phase < 3 && state->size > 0) {
        if (state->time >= state->timing->fade_start) {
            state->size = 4096 - ((state->time - state->timing->fade_start) << 12) /
                                 (state->timing->fade_end - state->timing->fade_start);
            if (state->size <= 0) {
                state->size = 0;
                state->phase = 3;
            }
        }
    }
}
