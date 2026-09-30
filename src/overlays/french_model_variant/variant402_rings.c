#include "../../types.h"
#include "variant402_rings.h"

void func_8013C69C(u8 *context)
{
    Family402RingView *work = (Family402RingView *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Family402Ring *ring = work->rings;
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
            matrix.t[0] = work->transform.t[0];
            matrix.t[1] = work->transform.t[1];
            matrix.t[2] = work->transform.t[2];
        } else {
            matrix.t[0] = work->transform.t[0] + work->velocity.vx * work->interpolation / 1024;
            matrix.t[1] = work->transform.t[1] + work->velocity.vy * work->interpolation / 1024;
            matrix.t[2] = work->transform.t[2] + work->velocity.vz * work->interpolation / 1024;
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
            setRGB0(quad, 0, 64, 192);
            setRGB1(quad, 0, 64, 192);
            setRGB2(quad, 0, 64, 192);
            setRGB3(quad, 192, 192, 192);
            if (depth > 0) {
                if (depth < 2048) {
                    GsSortPoly(quad, ot, depth);
                }
            }
        }
        if (work->selected_part + 1 == work->config->part_count) {
            if (i == 0) {
                if (work->phase == 0 && ring->scale < 4096) {
                    ring->scale = (work->elapsed << 12) / work->config->duration;
                    if (ring->scale >= 4096) {
                        ring->scale = 4096;
                        work->phase = 1;
                    }
                }
                if (work->phase == 5 && ring->scale > 0) {
                    ring->scale -= work->step * 32;
                    if (ring->scale <= 0) {
                        ring->scale = 0;
                        work->phase = 6;
                    }
                }
            } else if (work->phase <= 0) {
                ring->scale = 0;
            } else if (work->phase == 1) {
                ring->scale = 4096;
            } else if (work->phase == 2 || work->phase == 3) {
                if (ring->scale < 8192) {
                    ring->scale += work->step * 512;
                    if (ring->scale >= 8192) {
                        ring->scale = 8192;
                        work->phase = 4;
                    }
                }
            } else if (work->phase == 5 && ring->scale > 0) {
                ring->scale -= work->step * 64;
                if (ring->scale <= 0) {
                    ring->scale = 0;
                }
            }
        }
    }
}
