#include "../../types.h"
#include "variant405_pulses.h"
#include "../../game/gpu_packets.h"

void func_8013BD00(u8 *context)
{
    SVECTOR rotation;
    /* Unreferenced; occupies the untouched 16 bytes after the rotation. */
    VECTOR unused;
    s32 outer_r, outer_g, outer_b;
    s32 inner_r, inner_g, inner_b;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Pulse405State *state = (Pulse405State *)context;
    ModelVariantSheetSet *group = state->groups;
    Pulse405Primary *primary = state->primary;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    s32 pulse;
    s32 depth;

    ot = func_80058F10();
    quad = &state->quad;
    for (i = 0; i < state->count * 2; i++, group++) {
        if (!group->shown) {
            pulse = 0;
            if (state->frame & 1) {
                pulse = group->size / 8;
            }
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            if ((i & 1) == 0) {
                matrix.t[0] = state->origin[0];
                matrix.t[1] = state->origin[1];
                matrix.t[2] = state->origin[2];
            } else {
                matrix.t[0] = state->target.vx;
                matrix.t[1] = state->target.vy;
                matrix.t[2] = state->target.vz;
            }
            scale.vx = group->size + pulse;
            scale.vy = group->size + pulse;
            scale.vz = group->size + pulse;
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
            if ((i & 1) == 0) {
                if (group->size >= 2048) {
                    outer_r = group->outer[0] - group->outer[0] * (group->size - 2048) / 2048;
                    outer_g = group->outer[1] - group->outer[1] * (group->size - 2048) / 2048;
                    outer_b = group->outer[2] - group->outer[2] * (group->size - 2048) / 2048;
                    inner_r = group->inner[0] - group->inner[0] * (group->size - 2048) / 2048;
                    inner_g = group->inner[1] - group->inner[1] * (group->size - 2048) / 2048;
                    inner_b = group->inner[2] - group->inner[2] * (group->size - 2048) / 2048;
                } else {
                    outer_r = group->outer[0];
                    outer_g = group->outer[1];
                    outer_b = group->outer[2];
                    inner_r = group->inner[0];
                    inner_g = group->inner[1];
                    inner_b = group->inner[2];
                }
            } else {
                if (group->size < 4096) {
                    outer_r = group->outer[0];
                    outer_g = group->outer[1];
                    outer_b = group->outer[2];
                    inner_r = group->inner[0];
                    inner_g = group->inner[1];
                    inner_b = group->inner[2];
                } else {
                    outer_r = group->outer[0] - group->outer[0] * (group->size - 4096) / 4096;
                    outer_g = group->outer[1] - group->outer[1] * (group->size - 4096) / 4096;
                    outer_b = group->outer[2] - group->outer[2] * (group->size - 4096) / 4096;
                    inner_r = group->inner[0] - group->inner[0] * (group->size - 4096) / 4096;
                    inner_g = group->inner[1] - group->inner[1] * (group->size - 4096) / 4096;
                    inner_b = group->inner[2] - group->inner[2] * (group->size - 4096) / 4096;
                }
            }
            setRGB0(quad, inner_r, inner_g, inner_b);
            setRGB1(quad, inner_r, inner_g, inner_b);
            setRGB2(quad, inner_r, inner_g, inner_b);
            setRGB3(quad, outer_r, outer_g, outer_b);
            for (j = 0; j < 4; j++) {
                depth = RotTransPers4(&group->v0[j], &group->v1[j], &group->v2[j], &group->v3[j],
                                     (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                     (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                     &interpolation, &flag) * 8 / 10;
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if ((i & 1) == 0) {
            if (group->size < 4096) {
                group->size += state->step * 512;
                if (group->size >= 2048) {
                    primary->started = 1;
                }
                if (group->size >= 4096) {
                    group->size = 4096;
                    group->shown = 1;
                }
            }
        } else {
            if (primary->progress >= 1024 && group->size < 8192) {
                group->size += state->step * 512;
                if (group->size >= 8192) {
                    group->size = 8192;
                    group->shown = 1;
                }
            }
            primary++;
        }
    }
}
