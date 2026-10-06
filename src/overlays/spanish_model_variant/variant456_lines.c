#include "../../types.h"
#include "variant456_lines.h"
#include "../../game/gpu_packets.h"

void func_8013E15C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Lines456State *state;
    GsOT *ot;
    Lines456 *group;
    GsGLINE *line;
    s16 i, j;
    u8 red, green, blue;
    Lines456Companion *companion;
    Lines456Companion *companion_base;
    Lines456Control *control;
    Lines456Control *control_base;
    s32 extra;
    s32 depth;
    s32 companion_index;
    s32 control_index;

    state = (Lines456State *)context;
    companion_index = 0;
    control_index = 0;
    group = state->groups;
    line = &state->line;
    ot = func_80058F10();
    ratan2(state->direction[2], state->direction[0]);
    ratan2(state->direction[1], state->direction[0]);
    ratan2(state->direction_a[1], state->direction_a[0]);
    ratan2(state->projected.vy, state->projected.vx);
    control_base = state->controls;
    companion_base = state->companions;
    for (i = 0; i < 2; i++, group++, companion_index++, control_index++) {
        control = control_base + control_index;
        companion = companion_base + companion_index;
        extra = 0;
        if (state->frame & 1) {
            extra = control->size / 4;
        }
        if (control->fading == 0) {
            red = group->color.r;
            green = group->color.g;
            blue = group->color.b;
        } else {
            red = group->color.r * control->intensity / 1024;
            green = group->color.g * control->intensity / 1024;
            blue = group->color.b * control->intensity / 1024;
            state->angle += state->step * 4;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = state->angle;
        matrix.t[0] = companion->origin_x;
        matrix.t[1] = companion->origin_y;
        matrix.t[2] = companion->origin_z;
        scale.vx = control->size + extra;
        scale.vy = control->size + extra;
        scale.vz = control->size + extra;
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
        for (j = 0; j < 5; j++) {
            line->attribute = 0x50000000;
            depth = RotTransPers4(&group->points[0], &group->points[j+1],
                                 &group->points[0], &group->points[j+1],
                                 (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                                 (PSXLONG *)&line->x0, (PSXLONG *)&line->x1,
                                 &interpolation, &flag);
            line->r0 = red;
            line->g0 = green;
            line->b0 = blue;
            line->r1 = 0;
            line->g1 = 0;
            line->b1 = 0;
            if (depth > 0 && companion->progress >= 1024) {
                GsSortGLine(line, ot, (u16)depth);
            }
        }
    }
}
