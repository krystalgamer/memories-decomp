#include "../../types.h"
#include "variant410_entry.h"

void FrenchModelVariant410_DrawRings(Variant410EntryState *context)
{
    Variant410EntryState *work = context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    ModelVariantPointSheet *ring = work->rings;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    s32 pulse;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    quad = &work->textured[1];
    for (i = 0; i < 2; i++, ring++) {
        pulse = 0;
        if (work->clock.frame_count & 1) {
            pulse = ring->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = work->matrix.t[0];
            matrix.t[1] = work->matrix.t[1];
            matrix.t[2] = work->matrix.t[2];
        } else {
            matrix.t[0] = work->aim.target.vx;
            matrix.t[1] = work->aim.target.vy;
            matrix.t[2] = work->aim.target.vz;
        }
        scale.vx = ring->size + pulse;
        scale.vy = ring->size + pulse;
        scale.vz = ring->size + pulse;
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
            depth = RotTransPers4(&ring->points[j], &ring->points[4 + j],
                                 &ring->points[8 + j], &ring->points[12 + j],
                                 (long *)&quad->x0, (long *)&quad->x1,
                                 (long *)&quad->x2, (long *)&quad->x3,
                                 &interpolation, &flag);
            setRGB0(quad, ring->inner.r, ring->inner.g, ring->inner.b);
            setRGB1(quad, ring->inner.r, ring->inner.g, ring->inner.b);
            setRGB2(quad, ring->inner.r, ring->inner.g, ring->inner.b);
            setRGB3(quad, ring->outer.r, ring->outer.g, ring->outer.b);
            if (depth > 0) {
                if (depth < 2048) {
                    GsSortPoly(quad, ot, depth);
                }
            }
        }
        if (i == 0) {
            if (work->tail.phase == 0 && ring->size < 4096) {
                ring->size = (work->clock.frame << 12) / work->config->end;
                if (ring->size >= 4096) {
                    ring->size = 4096;
                    work->tail.phase = 1;
                }
            }
            if (work->tail.phase == 3 && ring->size > 0) {
                ring->size -= work->clock.step * 32;
                if (ring->size <= 0) {
                    ring->size = 0;
                }
            }
        } else {
            if (work->tail.phase == 2 && ring->size < 16384) {
                ring->size += work->clock.step * 512;
                if (ring->size >= 16384) {
                    ring->size = 16384;
                    work->tail.phase = 3;
                }
            }
            if (work->tail.phase == 3 && ring->size > 0) {
                ring->size -= work->clock.step * 96;
                if (ring->size <= 0) {
                    ring->size = 0;
                    work->tail.phase = 4;
                }
            }
        }
    }
}
