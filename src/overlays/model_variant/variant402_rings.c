#include "../../types.h"
#include "variant402_rings.h"

void FrenchModelVariant402_DrawRings(Variant402EntryState *context)
{
    Variant402EntryState *work = context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant402EntryRing *ring = work->rings;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    s32 pulse;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    quad = &work->packets.textured[1];
    for (i = 0; i < 2; i++, ring++) {
        pulse = 0;
        if (work->clock.frame_count & 1) {
            pulse = ring->scale / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = work->matrix.t[0];
            matrix.t[1] = work->matrix.t[1];
            matrix.t[2] = work->matrix.t[2];
        } else {
            matrix.t[0] = work->matrix.t[0] + work->aim.direction.vx * work->displacement / 1024;
            matrix.t[1] = work->matrix.t[1] + work->aim.direction.vy * work->displacement / 1024;
            matrix.t[2] = work->matrix.t[2] + work->aim.direction.vz * work->displacement / 1024;
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
            depth = RotTransPers4(&ring->points[j], &ring->points[4 + j],
                                 &ring->points[8 + j], &ring->points[12 + j],
                                 (long *)&quad->x0, (long *)&quad->x1,
                                 (long *)&quad->x2, (long *)&quad->x3,
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
        if (work->part_index + 1 == work->config->part_count) {
            if (i == 0) {
                if (work->phase == 0 && ring->scale < 4096) {
                    ring->scale = (work->clock.frame << 12) / work->config->start;
                    if (ring->scale >= 4096) {
                        ring->scale = 4096;
                        work->phase = 1;
                    }
                }
                if (work->phase == 5 && ring->scale > 0) {
                    ring->scale -= work->clock.step * 32;
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
                    ring->scale += work->clock.step * 512;
                    if (ring->scale >= 8192) {
                        ring->scale = 8192;
                        work->phase = 4;
                    }
                }
            } else if (work->phase == 5 && ring->scale > 0) {
                ring->scale -= work->clock.step * 64;
                if (ring->scale <= 0) {
                    ring->scale = 0;
                }
            }
        }
    }
}
