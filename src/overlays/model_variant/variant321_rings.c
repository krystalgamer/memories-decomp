#include "../../types.h"
#include "../french_model_variant/variant338_entry.h"

/* Draws two rings of four POLY_GT4 quads each, and grows each ring's scale
 * and fades it out through the phases. */
void ModelVariant321_DrawRings(Variant338EntryState *context)
{
    Variant338EntryState *work = context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    ModelVariantPointSheet *ring = work->rings;
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
            depth = RotTransPers4(
                &ring->points[j], &ring->points[j + 4],
                &ring->points[j + 8], &ring->points[j + 12],
                (long *)&quad->x0, (long *)&quad->x1,
                (long *)&quad->x2, (long *)&quad->x3,
                (long *)&interpolation, (long *)&flag);
            quad->r0 = ring->inner.r;
            quad->g0 = ring->inner.g;
            quad->b0 = ring->inner.b;
            quad->r1 = ring->inner.r;
            quad->g1 = ring->inner.g;
            quad->b1 = ring->inner.b;
            quad->r2 = ring->inner.r;
            quad->g2 = ring->inner.g;
            quad->b2 = ring->inner.b;
            quad->r3 = ring->outer.r;
            quad->g3 = ring->outer.g;
            quad->b3 = ring->outer.b;
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, depth);
            }
        }
        if (i == 0) {
            if (work->phase == 0) {
                if (ring->size < 4096) {
                    ring->size = (work->clock.frame - work->config->start) * 4096 /
                        (work->config->end - work->config->start);
                    if (ring->size >= 4096) {
                        ring->size = 4096;
                        work->phase = 1;
                    }
                }
            } else if (work->phase == 3 && ring->size > 0) {
                if (work->clock.frame >= work->config->fade_start) {
                    ring->size = 4096 -
                        (work->clock.frame - work->config->fade_start) * 4096 /
                        (work->config->fade_end - work->config->fade_start);
                }
                if (ring->size <= 0) {
                    ring->size = 0;
                }
            }
        } else if (work->phase == 2) {
            if (ring->size < 0x3000) {
                ring->size += work->clock.step * 2048;
                if (ring->size >= 0x3000) {
                    ring->size = 0x3000;
                    work->phase = 3;
                }
            }
        } else if (work->phase == 3 && ring->size > 0) {
            if (work->clock.frame >= work->config->fade_start) {
                ring->size = 0x3000 -
                    (work->clock.frame - work->config->fade_start) * 0x3000 /
                    (work->config->fade_end - work->config->fade_start);
            }
            if (ring->size <= 0) {
                ring->size = 0;
                work->phase = 4;
            }
        }
    }
}
