#include "../../types.h"
#include "variant411_tube.h"

void func_8013BE60(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Tube411State *state = (Tube411State *)context;
    Tube411 *tube;
    POLY_G4 *quad = &state->quad;
    GsOT *ot;
    s16 i, j, k;
    s16 angle, theta, radius;
    s32 size;
    s32 yaw;
    s32 axis_angle;
    s32 depth;
    u8 r0, g0, b0, r1, g1, b1;

    ot = func_80058F10();
    GsGetActiveBuff();
    ratan2(state->direction_b[2], state->direction_b[0]);
    ratan2(state->direction_b[1], state->direction_b[0]);
    axis_angle = ratan2(state->direction[2], state->direction[1]) + 3072;
    yaw = ratan2(state->projected.vy, state->projected.vx);
    size = state->width * 80 / 1024;
    if (state->width * 80 > 0) {
        yaw = -yaw + 1024;
    } else {
        yaw += 1024;
    }
    radius = size;
    tube = state->tubes;
    for (i = 0; i < 1; i++, tube++) {
        j = 0;
        angle = state->angle;
        for (; j < 9; j++, angle += 64) {
            for (k = 0, theta = angle; k < 9; k++, theta = angle + k * 512) {
                (tube->points[j] + k)->vx = state->direction[0] * state->progress / 1024 * j / 8 + (rcos(theta) * radius >> 12);
                (tube->points[j] + k)->vy = state->direction[1] * state->progress / 1024 * j / 8 + (rsin(theta) * (rcos(axis_angle) * radius >> 12) >> 12);
                (tube->points[j] + k)->vz = state->direction[2] * state->progress / 1024 * j / 8 + (rsin(theta) * (rsin(axis_angle) * radius >> 12) >> 12);
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
                if (state->phase >= 6) {
                    r0 = tube->colors[k].r * state->width / 1024;
                    g0 = tube->colors[k].g * state->width / 1024;
                    b0 = tube->colors[k].b * state->width / 1024;
                    r1 = tube->colors[k+1].r * state->width / 1024;
                    g1 = tube->colors[k+1].g * state->width / 1024;
                    b1 = tube->colors[k+1].b * state->width / 1024;
                    setRGB0(quad, r0, g0, b0);
                    setRGB1(quad, r1, g1, b1);
                    setRGB2(quad, r0, g0, b0);
                    setRGB3(quad, r1, g1, b1);
                } else {
                    setRGB0(quad, tube->colors[k].r, tube->colors[k].g, tube->colors[k].b);
                    setRGB1(quad, tube->colors[k+1].r, tube->colors[k+1].g, tube->colors[k+1].b);
                    setRGB2(quad, tube->colors[k].r, tube->colors[k].g, tube->colors[k].b);
                    setRGB3(quad, tube->colors[k+1].r, tube->colors[k+1].g, tube->colors[k+1].b);
                }
                flag = 0;
                if (depth >= 0) {
                    func_8005B260((u32 *)quad, ot, (u16)depth, 1);
                }
            }
        }
    }
    if (state->phase < 2 && state->progress <= 1024) {
        state->progress = (state->time - state->timing->expansion_start) * 1024 /
                          (state->timing->expansion_end - state->timing->expansion_start);
        if (state->progress >= 1024) {
            state->progress = 1024;
            state->phase = 2;
        }
    }
    if (state->time >= state->timing->fade_start && state->width > 0) {
        state->width = 1024 - (state->time - state->timing->fade_start) * 1024 /
                             (state->timing->fade_end - state->timing->fade_start);
        if (state->width <= 0) {
            state->width = 0;
            state->phase = 4;
        }
    }
    state->angle -= state->step * 32;
}
