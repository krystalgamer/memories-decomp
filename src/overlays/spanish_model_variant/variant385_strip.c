#include "../../types.h"
#include "variant385_strip.h"

void func_8013C478(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    Strip385State *state = (Strip385State *)context;
    Strip385 *strip;
    POLY_GT4 *polygon;
    GsOT *ot;
    s16 i, j;
    s16 extent;
    s32 angle;

    ot = func_80058F10();
    angle = ratan2(state->projected.vy, state->projected.vx);
    polygon = &state->polygon;
    angle += 2048;
    if (state->phase > 0) {
        if ((state->frame & 1) == 0) {
            extent = state->timing->width * state->radius / 1024;
        } else {
            extent = state->timing->width * state->radius / 1024 + state->radius / 128;
        }
        strip = state->strips;
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
                matrix.t[0] = state->origin[0] +
                    state->direction[0] * (state->progress * j / 2) / 1024;
                matrix.t[1] = state->origin[1] +
                    state->direction[1] * (state->progress * j / 2) / 1024;
                matrix.t[2] = state->origin[2] +
                    state->direction[2] * (state->progress * j / 2) / 1024;
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
        strip = state->strips;
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
        if (state->iteration + 1 == state->timing->iterations) {
            if (state->phase == 1) {
                if (state->progress <= 1024) {
                    state->progress = (state->time - state->timing->start) * 1024 /
                                      (state->timing->expanded - state->timing->start);
                    if (state->progress >= 1024) {
                        state->progress = 1024;
                        state->phase = 2;
                    }
                }
            } else if (state->timing->fade <= state->time && state->radius > 0) {
                state->radius = 1024 - (state->time - state->timing->fade) * 1024 /
                                      (state->timing->end - state->timing->fade);
                if (state->radius <= 0) {
                    state->radius = 0;
                }
            }
        }
    }
}
