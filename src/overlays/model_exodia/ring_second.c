#include "../../types.h"
#include "ring_second.h"

void func_8017B9B0(u8 *context)
{
    ExodiaSecondRingState *work;
    SVECTOR rotation;
    VECTOR position;
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

    work = (ExodiaSecondRingState *)context;
    ring = work->rings;
    ot = func_80058F10();
    quad = &work->quad;
    for (i = 0; i < 2; i++, ring++) {
        flicker = 0;
        if (work->frame_count & 1) {
            flicker = ring->scale / 8;
        }
        if (i == 0) {
            if (work->phase < 2) {
                if (ring->scale < 4096) {
                    ring->scale = ((work->frame - work->timing->start) << 12) /
                                  (work->timing->ramp_end - work->timing->start);
                    if (ring->scale >= 4096) {
                        ring->scale = 4096;
                        work->phase = 1;
                    }
                } else {
                    ring->scale = 4096;
                }
            } else if (work->timing->second_start <= work->frame) {
                ring->scale = 6144;
            }
        } else {
            if (work->timing->second_start <= work->frame && work->phase >= 2) {
                ring->scale = 4096;
            } else {
                ring->scale = 0;
            }
        }
        setVector(&rotation, 0, 0, 0);
        if (i == 0) {
            local.t[0] = work->origin.vx;
            local.t[1] = work->origin.vy;
            local.t[2] = work->origin.vz;
        } else {
            local.t[0] = work->origin.vx + work->direction.vx * work->distance / 1024;
            local.t[1] = work->origin.vy + work->direction.vy * work->distance / 1024;
            local.t[2] = work->origin.vz + work->direction.vz * work->distance / 1024;
        }
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
        for (j = 0; j < 4; j++) {
            RotTransPers4(&ring->points[j], &ring->points[j + 4],
                          &ring->points[j + 8], &ring->points[j + 12],
                          (long *)&quad->x0, (long *)&quad->x1,
                          (long *)&quad->x2, (long *)&quad->x3,
                          (long *)&interpolation, (long *)&flag);
            setRGB0(quad, ring->outer.r, ring->outer.g, ring->outer.b);
            setRGB1(quad, ring->outer.r, ring->outer.g, ring->outer.b);
            setRGB2(quad, ring->outer.r, ring->outer.g, ring->outer.b);
            setRGB3(quad, ring->inner.r, ring->inner.g, ring->inner.b);
            flag = 0;
            GsSortPoly(quad, ot, 0);
        }
    }
}
