#include "../../types.h"
#include "variant441_tube.h"

void func_8013C73C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Tube441State *state = (Tube441State *)context;
    Tube441 *tube;
    Tube441Pulse *pulse;
    POLY_G4 *quad = &state->quad;
    GsOT *ot;
    s16 i, j, k;
    s16 angle, theta, radius;
    s32 axis_angle;
    s32 depth;

    ot = func_80058F10();
    ratan2(state->direction_b[2], state->direction_b[0]);
    ratan2(state->direction_b[1], state->direction_b[0]);
    axis_angle = ratan2(state->direction[2], state->direction[1]) + 3072;
    ratan2(state->projected.vy, state->projected.vx);
    if (state->phase >= 3) {
        pulse = &state->pulse;
        radius = (state->width / 16) * pulse->size / 8192;
    } else {
        radius = state->width / 16;
    }
    tube = state->tubes;
    for (i = 0; i < 1; i++, tube++) {
        j = 0;
        angle = state->angle;
        for (; j < 9; j++, angle += 512) {
            for (k = 0, theta = angle; k < 9; k++, theta = angle + k * 512) {
                (tube->points[j] + k)->vx = state->direction[0] * state->progress / 1024 * j / 8 +
                                       (rcos(theta) * radius >> 12);
                (tube->points[j] + k)->vy = state->direction[1] * state->progress / 1024 * j / 8 +
                                       (rsin(theta) * (rcos(axis_angle) * radius >> 12) >> 12);
                (tube->points[j] + k)->vz = state->direction[2] * state->progress / 1024 * j / 8 +
                                       (rsin(theta) * (rsin(axis_angle) * radius >> 12) >> 12);
            }
        }
    }
    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = state->origin[0];
    matrix.t[1] = state->origin[1];
    matrix.t[2] = state->origin[2];
    scale.vx = 4096;
    scale.vy = 4096;
    scale.vz = 4096;
    RotMatrix(&rotation, &matrix);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    tube = state->tubes;
    for (i = 0; i < 1; i++, tube++) {
        for (j = 0; j < 8; j++) {
            for (k = 0; k < 8; k++) {
                depth = RotTransPers4(&tube->points[j][k], &tube->points[j][k+1],
                                     &tube->points[j+1][k], &tube->points[j+1][k+1],
                                     (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                     (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                     &interpolation, &flag);
                setRGB0(quad, tube->colors[k].r, tube->colors[k].g, tube->colors[k].b);
                setRGB1(quad, tube->colors[k+1].r, tube->colors[k+1].g, tube->colors[k+1].b);
                setRGB2(quad, tube->colors[k].r, tube->colors[k].g, tube->colors[k].b);
                setRGB3(quad, tube->colors[k+1].r, tube->colors[k+1].g, tube->colors[k+1].b);
                if (depth >= 0 && flag >= 0) {
                    func_8005B260((u32 *)quad, ot, (u16)depth, 1);
                }
            }
        }
    }
    if (state->phase == 1 && state->progress < 1024) {
        if (state->time >= state->timing->expansion_start) {
            state->progress = (state->time - state->timing->expansion_start) * 1024 /
                              (state->timing->expansion_end - state->timing->expansion_start);
            if (state->progress >= 1024) {
                state->progress = 1024;
                state->phase = 2;
            }
        }
    }
    if (state->width > 0) {
        if (state->time >= state->timing->fade_start) {
            state->width = 1024 - (state->time - state->timing->fade_start) * 1024 /
                                 (state->timing->fade_end - state->timing->fade_start);
        }
        if (state->width <= 0) {
            state->width = 0;
            if (state->phase == 3) {
                state->phase = 4;
            }
        }
    }
    state->angle -= state->step * 128;
}
