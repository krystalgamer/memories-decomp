#include "../../types.h"
#include "h421_entry_pal.h"
#include "../../game/gpu_packets.h"

void SpanishModelVariant438_DrawAdvancingBand(SpanishVariant438EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag[1][9];
    SpanishVariant438EntryState *state;
    GsOT *ot;
    s32 angle;
    POLY_GT4 *quad;
    ModelVariantColorBand *band;
    s16 i;
    s16 j;
    s16 radius;

    state = context;
    ot = Graphics_GetDefaultOrderingTable();
    quad = &state->packets.textured[0];
    angle = ratan2(state->aim.screen_delta.vy, state->aim.screen_delta.vx) + 2048;
    if (!(state->clock.frame_count & 1)) {
        radius = state->scale * state->config->radius / 4096;
    } else {
        radius = state->scale * (state->config->radius * 18 / 16) / 4096;
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
                matrix.t[0] = state->matrix.t[0];
                matrix.t[1] = state->matrix.t[1];
                matrix.t[2] = state->matrix.t[2];
            } else {
                matrix.t[0] = state->matrix.t[0] + state->aim.direction.vx * state->progress / 1024 * j / 8;
                matrix.t[1] = state->matrix.t[1] + state->aim.direction.vy * state->progress / 1024 * j / 8;
                matrix.t[2] = state->matrix.t[2] + state->aim.direction.vz * state->progress / 1024 * j / 8;
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
    if (state->phase == 1) {
        if (state->progress < 1025) {
            state->progress = ((state->clock.frame - state->config->grow_end) << 10) /
                              (state->config->band_grow_end - state->config->grow_end);
            if (state->progress >= 1024) {
                state->progress = 1024;
                state->phase = 2;
            }
        }
    } else if (state->phase < 3 && state->scale > 0) {
        if (state->clock.frame >= state->config->fade_start) {
            state->scale = 4096 - ((state->clock.frame - state->config->fade_start) << 12) /
                                 (state->config->fade_end - state->config->fade_start);
            if (state->scale <= 0) {
                state->scale = 0;
                state->phase = 3;
            }
        }
    }
}
