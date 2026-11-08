#include "../../types.h"
#include "variant324_draw.h"

/* Draws one header-432 ring at a base position moved by velocity * time /
 * 1024, and grows or shrinks its scale through the phases. */
void ModelVariant324_DrawMovingRing(u8 *context)
{
    ModelVariant324State *work = (ModelVariant324State *)context;
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
    s32 pulse;
    s32 depth;

    ot = Graphics_GetDefaultOrderingTable();
    quad = &work->quad;
    for (i = 0; i < 1; i++, ring++) {
        pulse = 0;
        if (work->frame & 1) {
            pulse = ring->size / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        matrix.t[0] = work->base.vx + work->velocity.vx * work->time / 1024;
        matrix.t[1] = work->base.vy + work->velocity.vy * work->time / 1024;
        matrix.t[2] = work->base.vz + work->velocity.vz * work->time / 1024;
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
        if (work->state == 1) {
            ring->size = 512;
        } else if (work->state < 4) {
            if (ring->size < 4096) {
                ring->size += work->step * 512;
                if (ring->size >= 4096) {
                    ring->size = 4096;
                }
            }
        } else if (work->state == 4) {
            if (ring->size > 0) {
                ring->size -= work->step * 32;
                if (ring->size <= 0) {
                    ring->size = 0;
                    work->state = 5;
                }
            }
        }
    }
}
