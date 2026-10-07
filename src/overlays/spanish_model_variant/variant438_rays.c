#include "../../types.h"
#include "variant438_rays.h"
#include "../../game/gpu_packets.h"

void func_8013D3E8(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Rays438State *state = (Rays438State *)context;
    Rays438 *ray = state->rays;
    Rays438Pulse *pulse = &state->pulse;
    POLY_G3 *triangle = &state->triangle;
    GsOT *ot;
    s16 i, j;
    s16 angle, radius, width;
    s32 axis_angle, tilt;
    s32 flags;
    s32 dx, dy;

    ot = func_80058F10();
    axis_angle = ratan2(state->direction_b[2], state->direction_b[0]) + 3072;
    ratan2(state->direction_b[1], state->direction_b[0]);
    tilt = ratan2(state->direction[2], state->direction[1]) + 1024;
    ratan2(state->direction[2], state->direction[0]);
    if (state->mirror == 1) {
        axis_angle = -axis_angle;
    }
    width = 16;
    flags = state->frame;
    for (i = 0, angle = state->angle; i < 8; i++, angle = state->angle + i * 512, ray++) {
        for (j = 0; j < 2; j++) {
            radius = 150;
            if (j == 0) {
                radius = 40;
            }
            (ray->points + j)->vx = rcos(angle) * radius >> 12;
            (ray->points + j)->vy = rsin(angle) * radius >> 12;
            (ray->points + j)->vz = j * ((flags & 1) * 32 + 160);
            (ray->offset_points + j)->vx = ray->points[j].vx + (rcos(axis_angle) * width >> 12);
            (ray->offset_points + j)->vy = ray->points[j].vy;
            (ray->offset_points + j)->vz = ray->points[j].vz + (rsin(axis_angle) * width >> 12);
        }
    }
    rotation.vx = tilt;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = state->origin[0] + state->direction[0] * state->progress / 1024;
    matrix.t[1] = state->origin[1] + state->direction[1] * state->progress / 1024;
    matrix.t[2] = state->origin[2] + state->direction[2] * state->progress / 1024;
    scale.vx = pulse->size;
    scale.vy = pulse->size;
    scale.vz = pulse->size;
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    ray = state->rays;
    for (i = 0; i < 8; i++, ray++) {
        for (j = 0; j < 2; j++) {
            if (j == 0) {
                ray->depth[j] = RotTransPers4(
                    &ray->points[j], &ray->points[j+1], &ray->points[j], &ray->points[j+1],
                    &ray->screen[j], &ray->screen[j+1], &ray->screen[j], &ray->screen[j+1],
                    &interpolation, &flag);
                dx = (s16)ray->screen[j+1] - (s16)ray->screen[j];
                dy = (ray->screen[j+1] >> 16) - (ray->screen[j] >> 16);
            } else {
                ray->depth[j] = RotTransPers4(
                    &ray->points[j-1], &ray->points[j], &ray->points[j-1], &ray->points[j],
                    &ray->screen[j-1], &ray->screen[j], &ray->screen[j-1], &ray->screen[j],
                    &interpolation, &flag);
                dx = (s16)ray->screen[j] - (s16)ray->screen[j-1];
                dy = (ray->screen[j] >> 16) - (ray->screen[j-1] >> 16);
            }
            RotTransPers(&ray->offset_points[j], &ray->offset_screen[j], &interpolation, &flag);
            ray->angles[j] = ratan2(dy, dx) + 3072;
            ray->projected_width[j] = (s16)ray->offset_screen[j] - (s16)ray->screen[j];
            ray->width_x[j] = rcos(ray->angles[j]) * ray->projected_width[j] >> 12;
            ray->width_y[j] = rsin(ray->angles[j]) * ray->projected_width[j] >> 12;
        }
    }
    ray = state->rays;
    for (i = 0; i < 8; i++, ray++) {
        for (j = 0; j < 1; j++) {
            setXY3(triangle,
                   ray->screen[j] + ray->width_x[j], (ray->screen[j] >> 16) + ray->width_y[j],
                   ray->screen[j+1], ray->screen[j+1] >> 16,
                   ray->screen[j] - ray->width_x[j], (ray->screen[j] >> 16) - ray->width_y[j]);
            setRGB0(triangle, 128, 128, 128);
            setRGB1(triangle, 255, 0, 255);
            setRGB2(triangle, 128, 128, 128);
            if (ray->depth[j] > 0) {
                func_8005B260((u32 *)triangle, ot, (u16)ray->depth[j], 1);
            }
        }
    }
    if (state->index + 1 == state->timing->count) {
        state->angle += state->step * 32;
    }
}
