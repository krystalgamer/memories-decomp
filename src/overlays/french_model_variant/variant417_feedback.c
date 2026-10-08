#include "../../types.h"
#include "variant417_feedback.h"

void func_8013E0E4(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Feedback417SizeRecord *record = (Feedback417SizeRecord *)(context + 0xA98);
    Feedback417State *state = (Feedback417State *)context;
    Feedback417 *group = state->groups;
    POLY_GT4 *quad = &state->quad;
    GsOT *ot;
    s32 i, j;
    s32 width, radius;
    s32 size, depth;
    s32 active;
    s32 yaw, pitch;
    s32 angle;
    s32 z;
    u32 page;

    ot = func_80058F10();
    active = GsGetActiveBuff();
    yaw = ratan2(state->direction.vz, state->direction.vx) + 2048;
    pitch = ratan2(state->direction.vy, state->direction.vz) + 2048;
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw += 1024;
    }
    width = record->size / 128;
    radius = record->size * 100 / 8192;
    for (i = 0; i < 5; i++, group++) {
        if (group->progress > 0) {
            size = rsin(group->progress) * 4096 >> 12;
            if (state->phase >= 5) {
                z = -width;
                for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                    setVector(&group->points[1][j], rcos(angle) * (256 + radius) >> 12,
                              rsin(angle) * (256 + radius) >> 12, z);
                    setVector(&group->points[2][j], rcos(angle) * (284 + radius) >> 12,
                              rsin(angle) * (284 + radius) >> 12, z);
                }
            }
            rotation.vx = -pitch;
            rotation.vy = yaw;
            rotation.vz = 0;
            matrix.t[0] = state->origin.vx +
                          state->direction.vx * (group->progress - 1024) / 2048;
            matrix.t[1] = state->origin.vy +
                          state->direction.vy * (group->progress - 1024) / 2048;
            matrix.t[2] = state->origin.vz +
                          state->direction.vz * (group->progress - 1024) / 2048;
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
                RotTransPers4(&group->points[0][j], &group->points[0][j + 1],
                              &group->points[2][j], &group->points[2][j + 1],
                              (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                              (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                              &interpolation, &flag);
                if (quad->x0 < 160) {
                    if (active == 0) {
                        page = GetTPage(2, 1, 320, 0);
                    } else {
                        page = GetTPage(2, 1, 0, 0);
                    }
                    SetPolyGT4(quad);
                    quad->tpage = page;
                    setUV4(quad, quad->x0, quad->y0, quad->x1, quad->y1,
                           quad->x2, quad->y2, quad->x3, quad->y3);
                } else {
                    if (active == 0) {
                        page = GetTPage(2, 1, 448, 0);
                    } else {
                        page = GetTPage(2, 1, 128, 0);
                    }
                    SetPolyGT4(quad);
                    quad->tpage = page;
                    setUV4(quad, quad->x0 - 128, quad->y0, quad->x1 - 128, quad->y1,
                           quad->x2 - 128, quad->y2, quad->x3 - 128, quad->y3);
                }
                depth = RotTransPers4(&group->points[0][j], &group->points[0][j + 1],
                                     &group->points[1][j], &group->points[1][j + 1],
                                     (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                     (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                     &interpolation, &flag);
                SetSemiTrans(quad, 1);
                SetShadeTex(quad, 0);
                setRGB0(quad, group->inner.r, group->inner.g, group->inner.b);
                setRGB1(quad, group->inner.r, group->inner.g, group->inner.b);
                setRGB2(quad, group->outer.r, group->outer.g, group->outer.b);
                setRGB3(quad, group->outer.r, group->outer.g, group->outer.b);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (group->progress < 2048) {
            group->progress += state->step * 48;
            if (group->progress >= 2048) {
                group->progress -= 2048;
                if (state->phase == 4) {
                    group->cycles++;
                    if (group->cycles >= 2) {
                        state->phase = 5;
                    }
                }
            }
        }
    }
}
