#include "../../types.h"
#include "variant441_entry_pal.h"

void SpanishModelVariant441_DrawTubularMesh(Variant441EntryState *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant441EntryState *state = context;
    ModelVariantTube *tube;
    ModelVariantSheet *pulse;
    POLY_G4 *quad = &state->packets.quad;
    GsOT *ot;
    s16 i, j, k;
    s16 angle, theta, radius;
    s32 axis_angle;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    ratan2(state->aim.view_delta.vz, state->aim.view_delta.vx);
    ratan2(state->aim.view_delta.vy, state->aim.view_delta.vx);
    axis_angle = ratan2(state->aim.direction.vz, state->aim.direction.vy) + 3072;
    ratan2(state->aim.screen_delta.vy, state->aim.screen_delta.vx);
    if (state->phase >= 3) {
        pulse = &state->sheets[1];
        radius = (state->width / 16) * pulse->size / 8192;
    } else {
        radius = state->width / 16;
    }
    tube = state->tubes;
    for (i = 0; i < 1; i++, tube++) {
        j = 0;
        angle = state->tube_angle;
        for (; j < 9; j++, angle += 512) {
            for (k = 0, theta = angle; k < 9; k++, theta = angle + k * 512) {
                (tube->points[j] + k)->vx = state->aim.direction.vx * state->tube_progress / 1024 * j / 8 +
                                       (rcos(theta) * radius >> 12);
                (tube->points[j] + k)->vy = state->aim.direction.vy * state->tube_progress / 1024 * j / 8 +
                                       (rsin(theta) * (rcos(axis_angle) * radius >> 12) >> 12);
                (tube->points[j] + k)->vz = state->aim.direction.vz * state->tube_progress / 1024 * j / 8 +
                                       (rsin(theta) * (rsin(axis_angle) * radius >> 12) >> 12);
            }
        }
    }
    rotation.vx = 0;
    rotation.vy = 0;
    rotation.vz = 0;
    matrix.t[0] = state->matrix.t[0];
    matrix.t[1] = state->matrix.t[1];
    matrix.t[2] = state->matrix.t[2];
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
                                     (long *)&quad->x0, (long *)&quad->x1,
                                     (long *)&quad->x2, (long *)&quad->x3,
                                     &interpolation, &flag);
                setRGB0(quad, tube->colors[k].r, tube->colors[k].g, tube->colors[k].b);
                setRGB1(quad, tube->colors[k+1].r, tube->colors[k+1].g, tube->colors[k+1].b);
                setRGB2(quad, tube->colors[k].r, tube->colors[k].g, tube->colors[k].b);
                setRGB3(quad, tube->colors[k+1].r, tube->colors[k+1].g, tube->colors[k+1].b);
                if (depth >= 0 && flag >= 0) {
                    Graphics_SubmitDrawModePacket((u32 *)quad, ot, (u16)depth, 1);
                }
            }
        }
    }
    if (state->phase == 1 && state->tube_progress < 1024) {
        if (state->frame >= state->config->expansion_start) {
            state->tube_progress = (state->frame - state->config->expansion_start) * 1024 /
                              (state->config->expansion_end - state->config->expansion_start);
            if (state->tube_progress >= 1024) {
                state->tube_progress = 1024;
                state->phase = 2;
            }
        }
    }
    if (state->width > 0) {
        if (state->frame >= state->config->fade_start) {
            state->width = 1024 - (state->frame - state->config->fade_start) * 1024 /
                                 (state->config->fade_end - state->config->fade_start);
        }
        if (state->width <= 0) {
            state->width = 0;
            if (state->phase == 3) {
                state->phase = 4;
            }
        }
    }
    state->tube_angle -= state->step * 128;
}
