#include "../../types.h"
#include "variant438_warp.h"

void func_8013C614(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Warp438State *state = (Warp438State *)context;
    s32 depth = 0;
    Warp438Ring *ring = state->rings;
    POLY_GT4 *poly = &state->polygon;
    GsOT *ot;
    s32 i, j;
    s32 buffer;
    s32 yaw, pitch;
    u32 tpage;
    u8 r0, g0, b0, r1, g1, b1;

    ot = func_80058F10();
    buffer = GsGetActiveBuff();
    yaw = ratan2(state->direction[2], state->direction[0]) + 2048;
    pitch = ratan2(state->direction[1], state->direction[2]) + 2048;
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw += 1024;
    }
    for (i = 0; i < 5; i++, ring++) {
        if (ring->size > 0) {
            rsin(ring->size);
            rotation.vz = 0;
            rotation.vx = -pitch;
            rotation.vy = yaw;
            matrix.t[0] = state->origin[0] + state->direction[0] * ring->size / 2048;
            matrix.t[1] = state->origin[1] + state->direction[1] * ring->size / 2048;
            matrix.t[2] = state->origin[2] + state->direction[2] * ring->size / 2048;
            scale.vx = 4096;
            scale.vy = 4096;
            scale.vz = 4096;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            for (j = 0; j < 16; j++) {
                r0 = ring->inner_color.r;
                g0 = ring->inner_color.g;
                b0 = ring->inner_color.b;
                r1 = ring->outer_color.r;
                g1 = ring->outer_color.g;
                b1 = ring->outer_color.b;
                RotTransPers4(&ring->outer[j], &ring->outer[j + 1], &ring->source[j], &ring->source[j + 1],
                              (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2,
                              (PSXLONG *)&poly->x3, &interpolation, &flag);
                if (poly->x0 < 160) {
                    if (buffer == 0) {
                        tpage = GetTPage(2, 1, 320, 0);
                    } else {
                        tpage = GetTPage(2, 1, 0, 0);
                    }
                    SetPolyGT4(poly);
                    poly->u0 = poly->x0;
                    poly->v0 = poly->y0;
                    poly->u1 = poly->x1;
                    poly->v1 = poly->y1;
                    poly->u2 = poly->x2;
                    poly->v2 = poly->y2;
                    poly->u3 = poly->x3;
                    poly->v3 = poly->y3;
                    poly->tpage = tpage;
                } else {
                    if (buffer == 0) {
                        tpage = GetTPage(2, 1, 448, 0);
                    } else {
                        tpage = GetTPage(2, 1, 128, 0);
                    }
                    SetPolyGT4(poly);
                    poly->u0 = poly->x0 - 128;
                    poly->v0 = poly->y0;
                    poly->u1 = poly->x1 - 128;
                    poly->v1 = poly->y1;
                    poly->u2 = poly->x2 - 128;
                    poly->v2 = poly->y2;
                    poly->u3 = poly->x3 - 128;
                    poly->v3 = poly->y3;
                    poly->tpage = tpage;
                }
                depth = RotTransPers4(&ring->outer[j], &ring->outer[j + 1], &ring->inner[j], &ring->inner[j + 1],
                                      (PSXLONG *)&poly->x0, (PSXLONG *)&poly->x1, (PSXLONG *)&poly->x2,
                                      (PSXLONG *)&poly->x3, &interpolation, &flag);
                SetSemiTrans(poly, 1);
                SetShadeTex(poly, 0);
                setRGB0(poly, r0, g0, b0);
                setRGB1(poly, r0, g0, b0);
                setRGB2(poly, r1, g1, b1);
                setRGB3(poly, r1, g1, b1);
                if (ring->size < 2048 && depth >= 0 && flag >= 0) {
                    GsSortPoly(poly, ot, depth);
                }
            }
        }
        if (ring->size < 2048) {
            ring->size += state->step << 6;
            if (ring->size >= 2048) {
                if (state->phase < 3) {
                    ring->size -= 2048;
                } else {
                    ring->size = 2048;
                }
            }
        }
    }
}
