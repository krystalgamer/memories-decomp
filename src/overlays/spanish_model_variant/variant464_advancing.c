#include "../../types.h"
#include "variant464_advancing.h"

void func_8013C8EC(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG flags[5][2];
    PSXLONG interpolation;
    GsOT *ot;
    Advancing464State *state = (Advancing464State *)context;
    Advancing464 *group;
    POLY_GT4 *quad;
    s16 i, j;
    s32 angle;
    s16 radius;
    s32 progress;

    ot = func_80058F10();
    quad = &state->quad;
    angle = ratan2(state->projected.vy, state->projected.vx) + 2048;
    if ((state->frame & 1) == 0) {
        radius = state->width / 128;
    } else {
        radius = state->width * 40 / 4096;
    }
    group = state->groups;
    for (i = 0; i < state->timing->count; i++, group++) {
        for (j = 0; j < 2; j++) {
            progress = group->progress[j];
            if (progress < 0) {
                progress = 0;
            }
            (j + group->points)->vx = rcos(1024) * radius >> 12;
            (j + group->points)->vy = rsin(1024) * radius >> 12;
            (j + group->points)->vz = 0;
            (group->points + j + 2)->vx = 0;
            (group->points + j + 2)->vy = 0;
            (group->points + j + 2)->vz = 0;
            (group->points + j + 4)->vx = rcos(3072) * radius >> 12;
            (group->points + j + 4)->vy = rsin(3072) * radius >> 12;
            (group->points + j + 4)->vz = 0;
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = angle;
            matrix.t[0] = state->origin[0] + state->direction[0] * progress / 1024;
            matrix.t[1] = state->origin[1] + state->direction[1] * progress / 1024;
            matrix.t[2] = state->origin[2] + state->direction[2] * progress / 1024;
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
            group->depth[j] = RotTransPers3(&group->points[j], group->points + 2 + j,
                                           group->points + 4 + j, &group->projected[0][j],
                                           &group->projected[1][j], &group->projected[2][j],
                                           &interpolation, &flags[i][j]);
            if (group->progress[j] < 1024) {
                group->progress[j] += state->step * 64;
                if (group->progress[j] >= 1024) {
                    group->progress[j] = 1024;
                    group->active = 1;
                    if (state->phase == 1) {
                        state->phase = 2;
                    }
                }
            }
        }
    }
    group = state->groups;
    for (i = 0; i < state->timing->count; i++, group++) {
        for (j = 0; j < 1; j++) {
            quad->x0 = group->projected[0][j];
            quad->y0 = group->projected[0][j] >> 16;
            quad->x1 = group->projected[0][j + 1];
            quad->y1 = group->projected[0][j + 1] >> 16;
            quad->x2 = group->projected[2][j];
            quad->y2 = group->projected[2][j] >> 16;
            quad->x3 = group->projected[2][j + 1];
            quad->y3 = group->projected[2][j + 1] >> 16;
            setRGB0(quad, group->outer[j + 1].r, group->outer[j + 1].g, group->outer[j + 1].b);
            setRGB1(quad, group->outer[j + 1].r, group->outer[j + 1].g, group->outer[j + 1].b);
            setRGB2(quad, group->outer[j + 1].r, group->outer[j + 1].g, group->outer[j + 1].b);
            setRGB3(quad, group->outer[j + 1].r, group->outer[j + 1].g, group->outer[j + 1].b);
            if (group->depth[j] >= 0 && flags[i][j] >= 0) {
                GsSortPoly(quad, ot, (u16)group->depth[j]);
            }
        }
    }
}
