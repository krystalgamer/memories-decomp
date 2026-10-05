#include "../../types.h"
#include "variant441_framebuffer_rings.h"

void func_8013E060(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    FramebufferRings441State *state = (FramebufferRings441State *)context;
    FramebufferRings441 *group = state->groups;
    POLY_GT4 *quad = &state->quad;
    GsOT *ot;
    s32 i, j;
    s32 complete;
    s32 buffer;
    s32 angle_y, angle_x;
    s32 size;
    s32 depth;
    u32 tpage;
    u8 red, green, blue;

    ot = func_80058F10();
    complete = 1;
    buffer = GsGetActiveBuff();
    angle_y = ratan2(state->direction[2], state->direction[0]) + 2048;
    angle_x = ratan2(state->direction[1], state->direction[2]) + 2048;
    if (angle_y < 2048) {
        angle_y = -angle_y + 1024;
    } else {
        angle_y += 1024;
    }
    for (i = 0; i < 5; i++, group++) {
        if (group->progress > 0) {
            size = rsin(group->progress) * 4096 >> 12;
            rotation.vx = -angle_x;
            rotation.vy = angle_y;
            rotation.vz = 0;
            matrix.t[0] = state->target.vx +
                          state->direction[0] * (group->progress-1024) / 2048;
            matrix.t[1] = state->target.vy +
                          state->direction[1] * (group->progress-1024) / 2048;
            matrix.t[2] = state->target.vz +
                          state->direction[2] * (group->progress-1024) / 2048;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            for (j = 0; j < 16; j++) {
                RotTransPers4(&group->points[0][j], &group->points[0][j+1],
                              &group->points[2][j], &group->points[2][j+1],
                              (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                              (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                              &interpolation, &flag);
                if (quad->x0 < 160) {
                    if (buffer == 0) {
                        tpage = GetTPage(2, 1, 320, 0);
                    } else {
                        tpage = GetTPage(2, 1, 0, 0);
                    }
                    SetPolyGT4(quad);
                    quad->tpage = tpage;
                    setUV4(quad, quad->x0, quad->y0, quad->x1, quad->y1,
                           quad->x2, quad->y2, quad->x3, quad->y3);
                } else {
                    if (buffer == 0) {
                        tpage = GetTPage(2, 1, 448, 0);
                    } else {
                        tpage = GetTPage(2, 1, 128, 0);
                    }
                    SetPolyGT4(quad);
                    quad->tpage = tpage;
                    setUV4(quad, quad->x0-128, quad->y0, quad->x1-128, quad->y1,
                           quad->x2-128, quad->y2, quad->x3-128, quad->y3);
                }
                depth = RotTransPers4(&group->points[0][j], &group->points[0][j+1],
                                     &group->points[1][j], &group->points[1][j+1],
                                     (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                     (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                     &interpolation, &flag);
                SetSemiTrans(quad, 0);
                SetShadeTex(quad, 0);
                if (state->frame % 8 == 0) {
                    red = 255;
                    green = 0;
                    blue = 0;
                } else if (state->frame % 8 == 1) {
                    red = 255;
                    green = 128;
                    blue = 0;
                } else if (state->frame % 8 == 2) {
                    red = 255;
                    green = 255;
                    blue = 0;
                } else if (state->frame % 8 == 3) {
                    red = 0;
                    green = 255;
                    blue = 0;
                } else if (state->frame % 8 == 4) {
                    red = 0;
                    green = 255;
                    blue = 255;
                } else if (state->frame % 8 == 5) {
                    red = 0;
                    green = 0;
                    blue = 255;
                } else if (state->frame % 8 == 6) {
                    red = 255;
                    green = 0;
                    blue = 255;
                } else {
                    red = 255;
                    green = 0;
                    blue = 128;
                }
                setRGB0(quad, 255, 255, 255);
                setRGB1(quad, 255, 255, 255);
                setRGB2(quad, red, green, blue);
                setRGB3(quad, red, green, blue);
                if (depth > 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (group->progress < 2048) {
            complete = 0;
            group->progress += state->step * 32;
            if (group->progress >= 2048) {
                if (state->phase >= 5) {
                    group->progress = 2048;
                } else {
                    group->progress -= 2048;
                }
            }
        }
        if (i+1 == 5 && complete == 1 && state->phase == 5) {
            state->phase = 6;
        }
    }
}
