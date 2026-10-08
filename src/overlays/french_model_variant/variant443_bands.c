#include "../../types.h"
#include "variant443_bands.h"
#include "../../game/gpu_packets.h"

void func_8013EEF0(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag[3][2];
    Bands443State *state;
    GsOT *ot;
    POLY_GT4 *quad;
    Band443 *band;
    s16 i;
    s16 j;
    s16 radius;
    s32 angle;
    ModelVariantSheet *sheet;

    state = (Bands443State *)context;
    sheet = state->sheets;
    quad = &state->quad;
    ot = func_80058F10();
    if (!(state->frame & 1)) {
        radius = state->size * (sheet->size / 256) / 1024;
    } else {
        radius = state->size * (sheet->size * 24 / 4096) / 1024;
    }
    band = state->bands;
    for (i = 0; i < 3; i++, band++) {
        angle = ratan2(state->screen_y[i], state->screen_x[i]) + 2048;
        for (j = 0; j < 2; j++) {
            setVector(&band->a[j], rcos(1024) * radius >> 12, rsin(1024) * radius >> 12, 0);
            setVector(&band->b[j], 0, 0, 0);
            setVector(&band->c[j], rcos(3072) * radius >> 12, rsin(3072) * radius >> 12, 0);
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = angle;
            matrix.t[0] = state->origins[i].vx + state->directions[i].vx * j;
            matrix.t[1] = state->origins[i].vy + state->directions[i].vy * j;
            matrix.t[2] = state->origins[i].vz + state->directions[i].vz * j;
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
    for (i = 0; i < 3; i++, band++) {
        for (j = 0; j < 1; j++) {
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
    if (state->size < 1024 && state->phase == 1) {
        state->size = ((state->time - state->timing->grow_start) << 10) /
                      (state->timing->grow_end - state->timing->grow_start);
        if (state->size >= 1024) {
            state->size = 1024;
            state->phase = 2;
        }
    }
}
