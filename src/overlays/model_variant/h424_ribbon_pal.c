#include "../../types.h"
#include "h424_entry_pal.h"

void SpanishModelVariant441_DrawRibbon(Variant441EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant441EntryState *state = context;
    ModelVariantShortRibbon *ribbon = state->ribbons;
    POLY_GT4 *quad = &state->packets.textured[0];
    GsOT *ot;
    s16 i, j;
    s32 angle;
    s16 radius;

    ot = Graphics_GetDefaultOrderingTable();
    angle = ratan2(state->aim.screen_delta.vy, state->aim.screen_delta.vx) + 2048;
    if (state->phase > 0) {
        radius = state->ribbon_width / 32;
        for (i = 0; i < 1; i++, ribbon++) {
            for (j = 0; j < 2; j++) {
                (ribbon->points[0] + j)->vx = rcos(1024) * radius >> 12;
                (ribbon->points[0] + j)->vy = rsin(1024) * radius >> 12;
                (ribbon->points[0] + j)->vz = 0;
                (ribbon->points[1] + j)->vx = 0;
                (ribbon->points[1] + j)->vy = 0;
                (ribbon->points[1] + j)->vz = 0;
                (ribbon->points[2] + j)->vx = rcos(3072) * radius >> 12;
                (ribbon->points[2] + j)->vy = rsin(3072) * radius >> 12;
                (ribbon->points[2] + j)->vz = 0;
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = angle;
                if (j == 0) {
                    matrix.t[0] = state->matrix.t[0];
                    matrix.t[1] = state->matrix.t[1];
                    matrix.t[2] = state->matrix.t[2];
                } else {
                    matrix.t[0] = state->matrix.t[0] + state->aim.direction.vx * state->progress / 1024;
                    matrix.t[1] = state->matrix.t[1] + state->aim.direction.vy * state->progress / 1024;
                    matrix.t[2] = state->matrix.t[2] + state->aim.direction.vz * state->progress / 1024;
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
                ribbon->depth[j] = RotTransPers3(
                    &ribbon->points[0][j], &ribbon->points[1][j], &ribbon->points[2][j],
                    &ribbon->projected[0][j],
                    &ribbon->projected[1][j],
                    &ribbon->projected[2][j], &interpolation, &flag);
            }
        }
        ribbon = state->ribbons;
        for (i = 0; i < 1; i++, ribbon++) {
            for (j = 0; j < 1; j++) {
                setXY4(quad,
                       ribbon->projected[0][j], ribbon->projected[0][j] >> 16,
                       ribbon->projected[0][j+1], ribbon->projected[0][j+1] >> 16,
                       ribbon->projected[1][j], ribbon->projected[1][j] >> 16,
                       ribbon->projected[1][j+1], ribbon->projected[1][j+1] >> 16);
                setRGB0(quad, 0, 0, 128);
                setRGB1(quad, 0, 0, 128);
                setRGB2(quad, 192, 192, 192);
                setRGB3(quad, 192, 192, 192);
                if (ribbon->depth[j] > 0) {
                    GsSortPoly(quad, ot, (u16)ribbon->depth[j]);
                }
                setXY4(quad,
                       ribbon->projected[2][j], ribbon->projected[2][j] >> 16,
                       ribbon->projected[2][j+1], ribbon->projected[2][j+1] >> 16,
                       ribbon->projected[1][j], ribbon->projected[1][j] >> 16,
                       ribbon->projected[1][j+1], ribbon->projected[1][j+1] >> 16);
                setRGB0(quad, 0, 0, 128);
                setRGB1(quad, 0, 0, 128);
                setRGB2(quad, 192, 192, 192);
                setRGB3(quad, 192, 192, 192);
                if (ribbon->depth[j] > 0) {
                    GsSortPoly(quad, ot, (u16)ribbon->depth[j]);
                }
            }
        }
        if (state->index + 1 == state->config->count) {
            if (state->phase == 1 && state->progress <= 1024) {
                if (state->config->expansion_start <= state->frame) {
                    state->progress = (state->frame - state->config->expansion_start) * 1024 /
                                      (state->config->expansion_end - state->config->expansion_start);
                }
                if (state->progress >= 1024) {
                    state->progress = 1024;
                    state->phase = 2;
                }
            }
            if (state->ribbon_width > 0) {
                if (state->frame >= state->config->fade_start) {
                    state->ribbon_width = 1024 - (state->frame - state->config->fade_start) * 1024 /
                                         (state->config->fade_end - state->config->fade_start);
                }
                if (state->ribbon_width <= 0) {
                    state->ribbon_width = 0;
                    if (state->phase == 3) {
                        state->phase = 4;
                    }
                }
            }
        }
    }
}
