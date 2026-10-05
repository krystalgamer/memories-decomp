#include "../../types.h"
#include "variant446_strip.h"

void func_8013C2F8(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    GsOT *ot;
    Strip446State *state = (Strip446State *)context;
    Strip446 *strip;
    POLY_GT4 *polygon;
    s16 i, j;
    s16 radius;
    s32 angle;

    ot = func_80058F10();
    angle = ratan2(state->projected.vy, state->projected.vx);
    polygon = &state->polygon;
    angle += 2048;
    if (state->phase > 0) {
        if ((state->frame & 1) == 0) {
            radius = state->timing->width * state->radius / 1024;
        } else {
            radius = state->timing->width * state->radius / 1024 + state->radius / 128;
        }
        strip = state->strips;
        for (i = 0; i < 1; i++, strip++) {
            for (j = 0; j < 5; j++) {
                setVector(&strip->upper[j], (rcos(1024) * radius) >> 12,
                          (rsin(1024) * radius) >> 12, 0);
                setVector(&strip->center[j], 0, 0, 0);
                setVector(&strip->lower[j], (rcos(3072) * radius) >> 12,
                          (rsin(3072) * radius) >> 12, 0);
                rotation.vx = 0;
                rotation.vy = 0;
                rotation.vz = angle;
                matrix.t[0] = state->origin[0] + state->direction[0] * (state->progress * j / 4) / 1024;
                matrix.t[1] = state->origin[1] + state->direction[1] * (state->progress * j / 4) / 1024;
                matrix.t[2] = state->origin[2] + state->direction[2] * (state->progress * j / 4) / 1024;
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
                strip->depth[j] = RotTransPers3(&strip->upper[j], &strip->center[j],
                                               &strip->lower[j],
                                               &strip->projected[0][j],
                                               &strip->projected[1][j],
                                               &strip->projected[2][j],
                                               &interpolation, &flag);
            }
        }
        strip = state->strips;
        for (i = 0; i < 1; i++, strip++) {
            for (j = 0; j < 4; j++) {
                polygon->x0 = strip->projected[0][j];
                polygon->y0 = strip->projected[0][j] >> 16;
                polygon->x1 = strip->projected[0][j+1];
                polygon->y1 = strip->projected[0][j+1] >> 16;
                polygon->x2 = strip->projected[1][j];
                polygon->y2 = strip->projected[1][j] >> 16;
                polygon->x3 = strip->projected[1][j+1];
                polygon->y3 = strip->projected[1][j+1] >> 16;
                polygon->r0 = 0;
                polygon->g0 = 32;
                polygon->b0 = 192;
                polygon->r1 = 0;
                polygon->g1 = 32;
                polygon->b1 = 192;
                polygon->r2 = 192;
                polygon->g2 = 192;
                polygon->b2 = 192;
                polygon->r3 = 192;
                polygon->g3 = 192;
                polygon->b3 = 192;
                if (strip->depth[j] > 0) {
                    GsSortPoly(polygon, ot, (u16)strip->depth[j+1]);
                }
                polygon->x0 = strip->projected[2][j];
                polygon->y0 = strip->projected[2][j] >> 16;
                polygon->x1 = strip->projected[2][j+1];
                polygon->y1 = strip->projected[2][j+1] >> 16;
                polygon->x2 = strip->projected[1][j];
                polygon->y2 = strip->projected[1][j] >> 16;
                polygon->x3 = strip->projected[1][j+1];
                polygon->y3 = strip->projected[1][j+1] >> 16;
                polygon->r0 = 0;
                polygon->g0 = 32;
                polygon->b0 = 192;
                polygon->r1 = 0;
                polygon->g1 = 32;
                polygon->b1 = 192;
                polygon->r2 = 192;
                polygon->g2 = 192;
                polygon->b2 = 192;
                polygon->r3 = 192;
                polygon->g3 = 192;
                polygon->b3 = 192;
                if (strip->depth[j] > 0) {
                    GsSortPoly(polygon, ot, (u16)strip->depth[j+1]);
                }
            }
        }
        if (state->iteration + 1 == state->timing->iterations) {
            if (state->phase == 1) {
                if (state->progress <= 1024) {
                    state->progress = ((state->time-state->timing->start)*1024) /
                                      (state->timing->expanded-state->timing->start);
                    if (state->progress >= 1024) {
                        state->progress = 1024;
                        state->phase = 2;
                    }
                }
            } else if (state->timing->fade <= state->time) {
                if (state->radius > 0) {
                    state->radius = 1024 - ((state->time-state->timing->fade)*1024) /
                                          (state->timing->end-state->timing->fade);
                    if (state->radius <= 0) {
                        state->radius = 0;
                    }
                }
            }
        }
    }
}
