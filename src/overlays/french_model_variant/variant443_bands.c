#include "../../types.h"
#include "variant443_entry.h"
#include "../../game/gpu_packets.h"

void FrenchModelVariant443_DrawBands(EntryState443 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag[3][2];
    EntryState443 *state;
    GsOT *ot;
    POLY_GT4 *quad;
    EntryShortBand443 *band;
    s16 i;
    s16 j;
    s16 radius;
    s32 angle;
    ModelVariantSheet *sheet;

    state = context;
    sheet = state->sheets;
    quad = &state->packets.textured[0];
    ot = Graphics_GetDefaultOrderingTable();
    if (!(state->clock.frame_count & 1)) {
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
            matrix.t[0] = state->positions[i].vx + state->paths[i].vx * j;
            matrix.t[1] = state->positions[i].vy + state->paths[i].vy * j;
            matrix.t[2] = state->positions[i].vz + state->paths[i].vz * j;
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
            quad->r0 = band->inner[j].r;
            quad->g0 = band->inner[j].g;
            quad->b0 = band->inner[j].b;
            quad->r1 = band->inner[j + 1].r;
            quad->g1 = band->inner[j + 1].g;
            quad->b1 = band->inner[j + 1].b;
            quad->r2 = band->outer[j].r;
            quad->g2 = band->outer[j].g;
            quad->b2 = band->outer[j].b;
            quad->r3 = band->outer[j + 1].r;
            quad->g3 = band->outer[j + 1].g;
            quad->b3 = band->outer[j + 1].b;
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
            quad->r0 = band->inner[j].r;
            quad->g0 = band->inner[j].g;
            quad->b0 = band->inner[j].b;
            quad->r1 = band->inner[j + 1].r;
            quad->g1 = band->inner[j + 1].g;
            quad->b1 = band->inner[j + 1].b;
            quad->r2 = band->outer[j].r;
            quad->g2 = band->outer[j].g;
            quad->b2 = band->outer[j].b;
            quad->r3 = band->outer[j + 1].r;
            quad->g3 = band->outer[j + 1].g;
            quad->b3 = band->outer[j + 1].b;
            if (band->otz[j] >= 0 && flag[i][j] >= 0) {
                GsSortPoly(quad, ot, band->otz[j]);
            }
        }
    }
    if (state->size < 1024 && state->phase == 1) {
        state->size = ((state->clock.frame - state->config->web_end) << 10) /
                      (state->config->spread_start - state->config->web_end);
        if (state->size >= 1024) {
            state->size = 1024;
            state->phase = 2;
        }
    }
}
