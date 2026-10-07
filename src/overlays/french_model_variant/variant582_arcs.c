#include "../../types.h"
#include "variant582_arcs.h"

void func_8013C560(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Arcs582State *state = (Arcs582State *)context;
    Arcs582Group *group = state->groups;
    GsGLINE *line = &state->line;
    GsOT *ot;
    s16 i, k, m;
    s16 base, angle;
    s32 near, far, radius;
    s32 count;
    s32 depth;

    ot = func_80058F10();
    ratan2(state->axis[2], state->axis[0]);
    ratan2(state->axis[1], state->axis[0]);
    count = 0;
    ratan2(state->direction_a[1], state->direction_a[0]);
    i = 0;
    ratan2(state->projected.vy, state->projected.vx);
    for (base = 0; i < 4; i++, group++, base += 300) {
        for (k = 0; k < 2; k++) {
            near = 0;
            if (group->near_progress[k] >= 0) {
                near = 1024;
                if (group->near_progress[k] < near) {
                    near = group->near_progress[k];
                }
            }
            far = 0;
            if (group->far_progress[k] >= 0) {
                far = 1024;
                if (group->far_progress[k] < far) {
                    far = group->far_progress[k];
                }
            }
            radius = (k + 1) * 192;
            for (m = 0, angle = base; m < 5; m++, angle = base + m * 4096 / 5) {
                setVector(&group->near_points[k][m],
                          state->direction[0] * near / 1024 + (rcos(angle) * radius >> 12),
                          state->direction[1] * near / 1024,
                          state->direction[2] * near / 1024 + (rsin(angle) * radius >> 12));
                setVector(&group->far_points[k][m],
                          state->direction[0] * far / 1024 + (rcos(angle) * radius >> 12),
                          state->direction[1] * far / 1024,
                          state->direction[2] * far / 1024 + (rsin(angle) * radius >> 12));
            }
            if (group->far_progress[k] < 1024) {
                group->near_progress[k] += state->step * 48;
                group->far_progress[k] += state->step * 48;
                if (group->far_progress[k] >= 1024) {
                    if (state->timing->limit <= state->time) {
                        group->near_progress[k] = 1024;
                        group->far_progress[k] = 1024;
                        group->done[k] = 1;
                    } else {
                        group->near_progress[k] = group->far_progress[k] - 1024;
                        group->far_progress[k] -= 1408;
                        if (state->phase == 1) {
                            state->phase = 2;
                        }
                    }
                }
            }
            count += group->done[k];
        }
    }
    if (count == 8 && state->phase == 2) {
        state->phase = 3;
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
    ScaleMatrix(&matrix, &scale);
    group = state->groups;
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    for (i = 0; i < 4; i++, group++) {
        for (k = 0; k < 2; k++) {
            if (group->far_progress[k] < 1024) {
                for (m = 0; m < 5; m++) {
                    line->attribute = 0x50000000;
                    depth = RotTransPers3(&group->near_points[k][m], &group->far_points[k][m],
                                         &group->near_points[k][m],
                                         (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                                         (PSXLONG *)&line->x0, &interpolation, &flag);
                    setRGB0(line, 160, 0, 160);
                    setRGB1(line, 8, 0, 8);
                    if (depth >= 0 && flag >= 0) {
                        GsSortGLine(line, ot, (u16)depth);
                    }
                }
            }
        }
    }
}
