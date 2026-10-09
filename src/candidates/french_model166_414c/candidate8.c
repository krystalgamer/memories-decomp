#include "../../types.h"
#include "../../overlays/model_variant/model_variant.h"

#include "candidate8.h"

void func_8013F14C(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Model166State *state = (Model166State *)context;
    Model166Ring *ring = state->rings;
    POLY_GT4 *quad = &state->quad;
    GsOT *ot;
    s32 i;
    s32 angle_y;
    s32 angle_x;
    s32 buffer;
    s32 j;
    s32 angle;
    s32 axial;
    s32 radius;
    s32 depth;
    u32 tpage;
    u8 red;
    u8 green;
    u8 blue;

    ot = func_80058F10();
    angle_y = ratan2(state->direction[2], state->direction[0]) + 2048;
    angle_x = ratan2(state->direction[1], state->direction[2]) + 2048;
    buffer = GsGetActiveBuff();
    if (angle_y < 2048) {
        angle_y = -angle_y + 1024;
    } else {
        angle_y += 1024;
    }
    for (i = 0; i < 3; i++, ring++) {
        if (ring->progress >= 2049) {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                axial = ((ring->progress - 2048) * 3 << 6) >> 11;
                radius = axial + 256;
                ring->rows[0][j].vx = rcos(angle) * radius >> 12;
                ring->rows[0][j].vy = rsin(angle) * radius >> 12;
                ring->rows[0][j].vz = -axial;
            }
        } else {
            for (j = 0, angle = 0; j < 17; j++, angle = j << 8) {
                ring->rows[0][j].vx = (u32)rcos(angle) >> 4;
                ring->rows[0][j].vy = (u32)rsin(angle) >> 4;
                ring->rows[0][j].vz = 0;
            }
        }
        if (ring->progress > 0) {
            rotation.vx = -angle_x;
            rotation.vy = angle_y;
            rotation.vz = 0;
            matrix.t[0] = state->translation[0];
            matrix.t[1] = state->translation[1];
            matrix.t[2] = state->translation[2];
            scale.vx = ring->progress;
            scale.vy = ring->progress;
            scale.vz = ring->progress;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            for (j = 0; j < 16; j++) {
                RotTransPers4(
                    &ring->rows[0][j], &ring->rows[0][j + 1],
                    &ring->rows[2][j], &ring->rows[2][j + 1],
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
                    setUV4(quad, quad->x0 - 128, quad->y0,
                           quad->x1 - 128, quad->y1,
                           quad->x2 - 128, quad->y2,
                           quad->x3 - 128, quad->y3);
                }
                depth = RotTransPers4(
                    &ring->rows[0][j], &ring->rows[0][j + 1],
                    &ring->rows[1][j], &ring->rows[1][j + 1],
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
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (ring->progress < 4096) {
            ring->progress += state->step * 80;
            if (ring->progress >= 4096) {
                if (state->phase < 4) {
                    ring->progress -= 4096;
                    ring->completed++;
                } else {
                    ring->progress = 4096;
                }
            }
        }
    }
}
