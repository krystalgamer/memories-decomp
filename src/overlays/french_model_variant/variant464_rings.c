#include "../../types.h"
#include "variant464_rings.h"
#include "../../game/gpu_packets.h"

void func_8013C368(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Ring464State *state = (Ring464State *)context;
    Ring464 *ring = state->rings;
    POLY_GT4 *quad = &state->quad;
    GsOT *ot;
    s32 i, j;
    s32 total;
    s32 angle_y, angle_x;
    s32 buffer;
    s32 lift, radius;
    s32 angle;
    s32 size;
    s32 depth;
    u32 tpage;
    u8 full;

    ot = func_80058F10();
    total = 0;
    angle_y = ratan2(state->direction[2], state->direction[0]) + 2048;
    angle_x = ratan2(state->direction[1], state->direction[2]) + 2048;
    buffer = GsGetActiveBuff();
    if (angle_y < 2048) {
        angle_y = -angle_y + 1024;
    } else {
        angle_y += 1024;
    }
    for (i = 0; i < 3; i++, ring++) {
        if (ring->progress > 2048) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                lift = (ring->progress - 2048) / 16;
                radius = lift + 256;
                ring->points[0][j].vx = rcos(angle) * radius >> 12;
                ring->points[0][j].vy = rsin(angle) * radius >> 12;
                ring->points[0][j].vz = -lift;
            }
            size = ring->progress;
        } else {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                ring->points[0][j].vx = rcos(angle) * 256 >> 12;
                ring->points[0][j].vy = rsin(angle) * 256 >> 12;
                ring->points[0][j].vz = 0;
            }
            size = ring->progress;
        }
        if (size > 0) {
            rotation.vx = -angle_x;
            rotation.vy = angle_y;
            rotation.vz = 0;
            matrix.t[0] = state->origin[0];
            matrix.t[1] = state->origin[1];
            matrix.t[2] = state->origin[2];
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
            for (j = 0, full = 255; j < 16; j++) {
                RotTransPers4(&ring->points[0][j], &ring->points[0][j+1],
                              &ring->points[2][j], &ring->points[2][j+1],
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
                depth = RotTransPers4(&ring->points[0][j], &ring->points[0][j+1],
                                     &ring->points[1][j], &ring->points[1][j+1],
                                     (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                     (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                     &interpolation, &flag);
                SetSemiTrans(quad, 0);
                SetShadeTex(quad, 0);
                setRGB0(quad, full, full, full);
                setRGB1(quad, full, full, full);
                setRGB2(quad, 0, full, full);
                setRGB3(quad, 0, full, full);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (ring->progress < 4096) {
            ring->progress += state->step * 96;
            if (ring->progress >= 4096) {
                if (state->phase < 3) {
                    ring->progress -= 4096;
                    ring->state = 1;
                } else {
                    ring->progress = 4096;
                    ring->state = 2;
                }
            }
        }
        total += ring->state;
        if (total == 6 && state->phase == 3) {
            state->phase = 5;
        }
    }
}
