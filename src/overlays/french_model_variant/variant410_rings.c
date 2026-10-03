#include "../../types.h"
#include "variant410_rings.h"

void func_8013C8B8(u8 *context)
{
    Family410RingView *work = (Family410RingView *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Family410Ring *ring = work->rings;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    s32 pulse;
    s32 depth;

    ot = func_80058F10();
    quad = &work->quad;
    for (i = 0; i < 2; i++, ring++) {
        pulse = 0;
        if (work->frame & 1) {
            pulse = ring->scale / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = work->origin[0];
            matrix.t[1] = work->origin[1];
            matrix.t[2] = work->origin[2];
        } else {
            matrix.t[0] = work->target.vx;
            matrix.t[1] = work->target.vy;
            matrix.t[2] = work->target.vz;
        }
        scale.vx = ring->scale + pulse;
        scale.vy = ring->scale + pulse;
        scale.vz = ring->scale + pulse;
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
        for (j = 0; j < 4; j++) {
            depth = RotTransPers4(&ring->points[0][j], &ring->points[1][j],
                                 &ring->points[2][j], &ring->points[3][j],
                                 (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                                 (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                                 &interpolation, &flag);
            setRGB0(quad, ring->outer.r, ring->outer.g, ring->outer.b);
            setRGB1(quad, ring->outer.r, ring->outer.g, ring->outer.b);
            setRGB2(quad, ring->outer.r, ring->outer.g, ring->outer.b);
            setRGB3(quad, ring->inner.r, ring->inner.g, ring->inner.b);
            if (depth > 0) {
                if (depth < 2048) {
                    GsSortPoly(quad, ot, depth);
                }
            }
        }
        if (i == 0) {
            if (work->phase == 0 && ring->scale < 4096) {
                ring->scale = (work->elapsed << 12) / work->config->duration;
                if (ring->scale >= 4096) {
                    ring->scale = 4096;
                    work->phase = 1;
                }
            }
            if (work->phase == 3 && ring->scale > 0) {
                ring->scale -= work->step * 32;
                if (ring->scale <= 0) {
                    ring->scale = 0;
                }
            }
        } else {
            if (work->phase == 2 && ring->scale < 16384) {
                ring->scale += work->step * 512;
                if (ring->scale >= 16384) {
                    ring->scale = 16384;
                    work->phase = 3;
                }
            }
            if (work->phase == 3 && ring->scale > 0) {
                ring->scale -= work->step * 96;
                if (ring->scale <= 0) {
                    ring->scale = 0;
                    work->phase = 4;
                }
            }
        }
    }
}
