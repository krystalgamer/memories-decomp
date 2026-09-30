#include "../../types.h"
#include "variant324_draw.h"

/* Draws one header-432 ring at a base position moved by velocity * time /
 * 1024, and grows or shrinks its scale through the phases. */
void func_8013D2F8(u8 *context)
{
    ModelVariant324State *work = (ModelVariant324State *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    ModelVariant432Ring *ring = work->rings;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    s32 pulse;
    s32 depth;

    ot = func_80058F10();
    quad = &work->quad;
    for (i = 0; i < 1; i++, ring++) {
        pulse = 0;
        if (work->frame & 1) {
            pulse = ring->scale / 8;
        }
        rotation.vx = 0;
        rotation.vy = 0;
        rotation.vz = 0;
        matrix.t[0] = work->base.vx + work->velocity.vx * work->time / 1024;
        matrix.t[1] = work->base.vy + work->velocity.vy * work->time / 1024;
        matrix.t[2] = work->base.vz + work->velocity.vz * work->time / 1024;
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
            depth = RotTransPers4(
                &ring->points[0][j], &ring->points[1][j],
                &ring->points[2][j], &ring->points[3][j],
                (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                (PSXLONG *)&interpolation, (PSXLONG *)&flag);
            quad->r0 = ring->color[1].r;
            quad->g0 = ring->color[1].g;
            quad->b0 = ring->color[1].b;
            quad->r1 = ring->color[1].r;
            quad->g1 = ring->color[1].g;
            quad->b1 = ring->color[1].b;
            quad->r2 = ring->color[1].r;
            quad->g2 = ring->color[1].g;
            quad->b2 = ring->color[1].b;
            quad->r3 = ring->color[0].r;
            quad->g3 = ring->color[0].g;
            quad->b3 = ring->color[0].b;
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, depth);
            }
        }
        if (work->state == 1) {
            ring->scale = 512;
        } else if (work->state < 4) {
            if (ring->scale < 4096) {
                ring->scale += work->step * 512;
                if (ring->scale >= 4096) {
                    ring->scale = 4096;
                }
            }
        } else if (work->state == 4) {
            if (ring->scale > 0) {
                ring->scale -= work->step * 32;
                if (ring->scale <= 0) {
                    ring->scale = 0;
                    work->state = 5;
                }
            }
        }
    }
}
