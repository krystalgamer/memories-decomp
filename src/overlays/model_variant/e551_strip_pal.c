#include "../../types.h"
#include "e551_entry_pal.h"

void SpanishModelVariant385_DrawStationStrip(Variant385EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    Variant385EntryState *state = context;
    Variant385EntryTube *strip;
    POLY_GT4 *polygon;
    GsOT *ot;
    s16 i, j;
    s16 extent;
    s32 angle;

    ot = Graphics_GetDefaultOrderingTable();
    angle = ratan2(state->aim.screen_delta.vy, state->aim.screen_delta.vx);
    polygon = &state->packets.textured[0];
    angle += 2048;
    if (state->phase > 0) {
        if ((state->clock.frame_count & 1) == 0) {
            extent = state->config->width * state->radius / 1024;
        } else {
            extent = state->config->width * state->radius / 1024 + state->radius / 128;
        }
        strip = state->tubes;
        for (i = 0; i < 1; i++, strip++) {
            for (j = 0; j < 3; j++) {
                setVector(&strip->upper[j], rcos(1024) * extent >> 12,
                          rsin(1024) * extent >> 12, 0);
                setVector(&strip->center[j], 0, 0, 0);
                setVector(&strip->lower[j], rcos(3072) * extent >> 12,
                          rsin(3072) * extent >> 12, 0);
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = angle;
                matrix.t[0] = state->matrix.t[0] +
                    state->aim.direction.vx * (state->progress * j / 2) / 1024;
                matrix.t[1] = state->matrix.t[1] +
                    state->aim.direction.vy * (state->progress * j / 2) / 1024;
                matrix.t[2] = state->matrix.t[2] +
                    state->aim.direction.vz * (state->progress * j / 2) / 1024;
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
                strip->depth[j] = RotTransPers3(
                    &strip->upper[j], &strip->center[j], &strip->lower[j],
                    &strip->projected[0][j], &strip->projected[1][j], &strip->projected[2][j],
                    &interpolation, &strip->flags[j]);
            }
        }
        strip = state->tubes;
        for (i = 0; i < 1; i++, strip++) {
            for (j = 0; j < 2; j++) {
                setXY4(polygon,
                       strip->projected[0][j], strip->projected[0][j] >> 16,
                       strip->projected[0][j+1], strip->projected[0][j+1] >> 16,
                       strip->projected[1][j], strip->projected[1][j] >> 16,
                       strip->projected[1][j+1], strip->projected[1][j+1] >> 16);
                setRGB0(polygon, 0, 32, 192);
                setRGB1(polygon, 0, 32, 192);
                setRGB2(polygon, 192, 192, 192);
                setRGB3(polygon, 192, 192, 192);
                if (strip->depth[j] < 0) {
                    strip->depth[j] = 0;
                }
                strip->flags[j] = 0;
                if (strip->depth[j] >= 0 && strip->flags[j] >= 0) {
                    GsSortPoly(polygon, ot, (u16)strip->depth[j+1]);
                }
                setXY4(polygon,
                       strip->projected[2][j], strip->projected[2][j] >> 16,
                       strip->projected[2][j+1], strip->projected[2][j+1] >> 16,
                       strip->projected[1][j], strip->projected[1][j] >> 16,
                       strip->projected[1][j+1], strip->projected[1][j+1] >> 16);
                setRGB0(polygon, 0, 32, 192);
                setRGB1(polygon, 0, 32, 192);
                setRGB2(polygon, 192, 192, 192);
                setRGB3(polygon, 192, 192, 192);
                if (strip->depth[j] < 0) {
                    strip->depth[j] = 0;
                }
                strip->flags[j] = 0;
                if (strip->depth[j] >= 0 && strip->flags[j] >= 0) {
                    GsSortPoly(polygon, ot, (u16)strip->depth[j+1]);
                }
            }
        }
        if (state->index + 1 == state->config->count) {
            if (state->phase == 1) {
                if (state->progress <= 1024) {
                    state->progress = (state->clock.frame - state->config->grow_end) * 1024 /
                                      (state->config->expansion_end - state->config->grow_end);
                    if (state->progress >= 1024) {
                        state->progress = 1024;
                        state->phase = 2;
                    }
                }
            } else if (state->config->fade_start <= state->clock.frame && state->radius > 0) {
                state->radius = 1024 - (state->clock.frame - state->config->fade_start) * 1024 /
                                      (state->config->fade_end - state->config->fade_start);
                if (state->radius <= 0) {
                    state->radius = 0;
                }
            }
        }
    }
}
