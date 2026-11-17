#include "../../types.h"
#include "../model_variant/e570_entry.h"

/* The entry also passes its phase; the helper reads tail.phase instead. */
void SpanishModelVariant432_DrawSheets(Variant415EntryState *context, s32 phase)
{
    Variant415EntryState *work = context;
    SVECTOR rotation;
    /* The target reserves 16 unused bytes between rotation and scale. */
    u8 unknown_stack[16];
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    ModelVariantSheet *ring = work->sheets;
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
        matrix.t[0] = work->target.vx;
        matrix.t[1] = work->target.vy;
        matrix.t[2] = work->target.vz;
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
        if (work->tail.phase < 3) {
            if (ring->size < 8192) {
                ring->size += work->clock.step * 512;
                if (ring->size >= 8192) {
                    ring->size = 8192;
                }
            }
        } else if (work->tail.phase == 3) {
            if (ring->size > 0) {
                ring->size -= work->clock.step * 128;
                if (ring->size <= 0) {
                    ring->size = 0;
                    work->tail.phase = 4;
                }
            }
        }
    }
}
