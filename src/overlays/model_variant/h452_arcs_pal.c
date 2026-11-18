#include "../../types.h"
#include "h452_entry_pal.h"

void FrenchModelVariant582_DrawArcs(EntryState469 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    EntryState469 *state = context;
    Arcs582Group *group = state->arcs;
    GsGLINE *line = &state->line;
    GsOT *ot;
    s16 i, k, m;
    s16 base, angle;
    s32 near, far, radius;
    s32 count;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    ratan2(state->view_delta.vz, state->view_delta.vx);
    ratan2(state->view_delta.vy, state->view_delta.vx);
    count = 0;
    ratan2(state->direction.vz, state->direction.vy);
    i = 0;
    ratan2(state->screen_delta.vy, state->screen_delta.vx);
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
                          state->line_direction.vx * near / 1024 + (rcos(angle) * radius >> 12),
                          state->line_direction.vy * near / 1024,
                          state->line_direction.vz * near / 1024 + (rsin(angle) * radius >> 12));
                setVector(&group->far_points[k][m],
                          state->line_direction.vx * far / 1024 + (rcos(angle) * radius >> 12),
                          state->line_direction.vy * far / 1024,
                          state->line_direction.vz * far / 1024 + (rsin(angle) * radius >> 12));
            }
            if (group->far_progress[k] < 1024) {
                group->near_progress[k] += state->clock.step * 48;
                group->far_progress[k] += state->clock.step * 48;
                if (group->far_progress[k] >= 1024) {
                    if (state->config->arc_limit <= state->clock.frame) {
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
    matrix.t[0] = state->origin.vx;
    matrix.t[1] = state->origin.vy;
    matrix.t[2] = state->origin.vz;
    scale.vx = 4096;
    scale.vy = 4096;
    scale.vz = 4096;
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    group = state->arcs;
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
                                         (long *)&line->x0, (long *)&line->x1,
                                         (long *)&line->x0, &interpolation, &flag);
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
