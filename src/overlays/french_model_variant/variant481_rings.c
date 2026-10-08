#include "../../types.h"
#include "variant481_rings.h"

void func_8013C718(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Rings481State *state = (Rings481State *)context;
    Ring481Gauge *gauge;
    Ring481 *ring = state->rings;
    GsOT *ot;
    s32 i;
    s32 sweep;
    s32 angle;
    s32 tilt_a;
    s32 tilt_b;
    s16 bob;
    POLY_GT4 *quad;
    s32 pitch;
    s32 k;
    s32 turn;
    s32 fade;
    s32 depth;
    u8 r0, g0, b0, r1, g1, b1;

    ot = func_80058F10();
    angle = ratan2(state->direction[2], state->direction[0]) + 2048;
    pitch = ratan2(state->direction[1], state->direction[2]);
    gauge = &state->gauge;
    quad = &state->quad;
    pitch += 2048;
    if (angle < 2048) {
        angle = -angle + 1024;
    } else {
        angle = -angle + 1024;
    }
    bob = (state->frame & 1) ? 1024 : 0;
    rsin(2048);
    tilt_a = -pitch - 768;
    tilt_b = -pitch - 1280;
    for (i = 0, sweep = 2048; i < 2; i++, ring++, sweep += 2048) {
        if (ring->scale > 0) {
            for (k = 0, turn = state->spin; k < 17; k++, turn = state->spin + k * 256) {
                setVector(&ring->a[k], rcos(turn) * 64 >> 12, rsin(turn) * 64 >> 12, 0);
                setVector(&ring->b[k], rcos(turn) * 256 >> 12, rsin(turn) * 256 >> 12,
                          -(rcos(sweep) * 128 >> 12));
            }
            if (state->mode == 0) {
                setVector(&rotation, tilt_a, angle, 0);
                matrix.t[0] = state->position.vx;
                matrix.t[1] = state->position.vy - (rsin(sweep + 768) * 96 >> 12);
                matrix.t[2] = state->position.vz - (rcos(sweep) * 96 >> 12);
            } else {
                setVector(&rotation, tilt_b, angle, 0);
                matrix.t[0] = state->position.vx;
                matrix.t[1] = state->position.vy - (rsin(sweep + 768) * 96 >> 12);
                matrix.t[2] = state->position.vz - (rcos(sweep + 2048) * 96 >> 12);
            }
            setVector(&scale, ring->scale + bob, ring->scale + bob, ring->scale + bob);
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            if (ring->scale > 6144) {
                fade = 8192 - ring->scale;
                r0 = state->inner.r * fade / 2048;
                g0 = state->inner.g * fade / 2048;
                b0 = state->inner.b * fade / 2048;
                r1 = state->outer.r * fade / 2048;
                g1 = state->outer.g * fade / 2048;
                b1 = state->outer.b * fade / 2048;
            } else {
                r0 = state->inner.r;
                g0 = state->inner.g;
                b0 = state->inner.b;
                r1 = state->outer.r;
                g1 = state->outer.g;
                b1 = state->outer.b;
            }
            for (k = 0; k < 16; k++) {
                depth = RotTransPers4(&ring->a[k], &ring->a[k + 1], &ring->b[k], &ring->b[k + 1],
                                      (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                      (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3, &interpolation, &flag);
                setRGB0(quad, r0, g0, b0);
                setRGB1(quad, r0, g0, b0);
                setRGB2(quad, r1, g1, b1);
                setRGB3(quad, r1, g1, b1);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (i == 0) {
            if (ring->scale < 8192 && gauge->level > 900) {
                ring->scale += state->step * 64;
                if (ring->scale > 8192) {
                    ring->scale = 8192;
                }
            }
        } else {
            if (ring->scale < 8192 && gauge->level > 1100) {
                ring->scale += state->step * 64;
                if (ring->scale > 8192) {
                    ring->scale = 8192;
                }
            }
        }
    }
    state->spin += state->step * 80;
}
