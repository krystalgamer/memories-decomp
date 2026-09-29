#include "../../types.h"
#include "ring.h"

void func_8013B860(u8 *context)
{
    ExodiaRingState *work;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX local;
    MATRIX screen;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    GsOT *ot;
    s32 i, j;
    s32 flicker;
    ExodiaRing *ring;
    POLY_GT4 *quad;

    work = (ExodiaRingState *)context;
    ring = work->rings;
    ot = func_80058F10();
    quad = &work->quad;
    for (i = 0; i < 1; i++, ring++) {
        flicker = 0;
        if (work->frame_count & 1) {
            flicker = ring->scale / 8;
        }
        setVector(&rotation, 0, 0, 0);
        local.t[0] = work->origin.vx;
        local.t[1] = work->origin.vy;
        local.t[2] = work->origin.vz;
        scale.vx = ring->scale + flicker;
        scale.vy = ring->scale + flicker;
        scale.vz = ring->scale + flicker;
        RotMatrix(&rotation, &local);
        coordinate.coord = local;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &screen);
        GsSetLsMatrix(&screen);
        ReadRotMatrix(&screen);
        RotMatrix(&rotation, &screen);
        ScaleMatrix(&screen, &scale);
        SetRotMatrix(&screen);
        setRGB0(quad, ring->outer.r, ring->outer.g, ring->outer.b);
        setRGB1(quad, ring->outer.r, ring->outer.g, ring->outer.b);
        setRGB2(quad, ring->outer.r, ring->outer.g, ring->outer.b);
        setRGB3(quad, ring->inner.r, ring->inner.g, ring->inner.b);
        for (j = 0; j < 4; j++) {
            RotTransPers4(&ring->points[j], &ring->points[j + 4],
                          &ring->points[j + 8], &ring->points[j + 12],
                          (long *)&quad->x0, (long *)&quad->x1,
                          (long *)&quad->x2, (long *)&quad->x3,
                          (long *)&interpolation, (long *)&flag);
            flag = 0;
            GsSortPoly(quad, ot, 0);
        }
        if (ring->scale < 4096 && work->grown == 0) {
            ring->scale = ((work->frame - work->timing->start) << 12) /
                          (work->timing->ramp_end - work->timing->start);
            if (ring->scale >= 4096) {
                ring->scale = 4096;
                work->grown = 1;
            }
        } else if (work->frame < work->timing->growth_end) {
            ring->scale += work->step << 9;
            if (ring->scale >= 8192) {
                ring->scale = 8192;
            }
        } else {
            ring->scale = 8192 - ((work->frame - work->timing->growth_end) << 13) /
                          (work->timing->shrink_end - work->timing->growth_end);
            if (ring->scale <= 0) {
                ring->scale = 0;
            }
        }
    }
}
