#include "../../types.h"
#include "variant376_rings.h"

/* Draws one ring, or two when the single flag is clear, as four POLY_GT4
 * quads each, and grows or shrinks each ring's scale through the phases. */
void ModelVariant376_DrawRings(u8 *context)
{
    ModelVariant376State *work = (ModelVariant376State *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    ModelVariantSheet *ring = work->rings;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    s32 count;
    s32 pulse;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    count = 1;
    quad = &work->quad;
    if (work->single == 0) {
        count = 2;
    }
    for (i = 0; i < count; i++, ring++) {
        pulse = 0;
        if (work->frame & 1) {
            pulse = ring->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        if (i == 0) {
            matrix.t[0] = work->transform.t[0];
            matrix.t[1] = work->transform.t[1];
            matrix.t[2] = work->transform.t[2];
        } else {
            matrix.t[0] = work->target.vx;
            matrix.t[1] = work->target.vy;
            matrix.t[2] = work->target.vz;
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
                &ring->v0[j], &ring->v1[j],
                &ring->v2[j], &ring->v3[j],
                (long *)&quad->x0, (long *)&quad->x1,
                (long *)&quad->x2, (long *)&quad->x3,
                (long *)&interpolation, (long *)&flag);
            quad->r0 = ring->inner[0];
            quad->g0 = ring->inner[1];
            quad->b0 = ring->inner[2];
            quad->r1 = ring->inner[0];
            quad->g1 = ring->inner[1];
            quad->b1 = ring->inner[2];
            quad->r2 = ring->inner[0];
            quad->g2 = ring->inner[1];
            quad->b2 = ring->inner[2];
            quad->r3 = ring->outer[0];
            quad->g3 = ring->outer[1];
            quad->b3 = ring->outer[2];
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, depth);
            }
        }
        if (i == 0) {
            if (work->state == 0) {
                if (ring->size < 4096) {
                    ring->size = (work->elapsed - work->config->start) * 4096 /
                        (work->config->end - work->config->start);
                    if (ring->size >= 4096) {
                        ring->size = 4096;
                        work->state = 1;
                    }
                }
            } else if (work->elapsed >= work->config->fade_start && ring->size > 0) {
                ring->size = 4096 -
                    (work->elapsed - work->config->fade_start) * 4096 /
                    (work->config->fade_end - work->config->fade_start);
                if (ring->size <= 0) {
                    ring->size = 0;
                    if (work->state == 3) {
                        work->state = 4;
                    }
                }
            }
        } else if (work->state == 2) {
            if (ring->size < 0x4000) {
                ring->size += work->step * 2048;
                if (ring->size >= 0x4000) {
                    ring->size = 0x4000;
                    work->state = 3;
                }
            }
        } else if (work->state == 4) {
            if (ring->size > 0) {
                ring->size -= work->step * 256;
                if (ring->size <= 0) {
                    ring->size = 0;
                }
            }
        }
    }
}
