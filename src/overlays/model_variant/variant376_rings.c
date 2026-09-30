#include "../../types.h"
#include "variant376_rings.h"

/* Draws one ring, or two when the single flag is clear, as four POLY_GT4
 * quads each, and grows or shrinks each ring's scale through the phases. */
void func_8013C6D8(u8 *context)
{
    ModelVariant376State *work = (ModelVariant376State *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    s32 interpolation;
    s32 flag;
    ModelVariant337Ring *ring = work->rings;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    s32 count;
    s32 pulse;
    s32 depth;

    ot = func_80058F10();
    count = 1;
    quad = &work->quad;
    if (work->single == 0) {
        count = 2;
    }
    for (i = 0; i < count; i++, ring++) {
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
            matrix.t[0] = work->target.vx;
            matrix.t[1] = work->target.vy;
            matrix.t[2] = work->target.vz;
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
        if (i == 0) {
            if (work->state == 0) {
                if (ring->scale < 4096) {
                    ring->scale = (work->elapsed - work->config->start) * 4096 /
                        (work->config->end - work->config->start);
                    if (ring->scale >= 4096) {
                        ring->scale = 4096;
                        work->state = 1;
                    }
                }
            } else if (work->elapsed >= work->config->fade_start && ring->scale > 0) {
                ring->scale = 4096 -
                    (work->elapsed - work->config->fade_start) * 4096 /
                    (work->config->fade_end - work->config->fade_start);
                if (ring->scale <= 0) {
                    ring->scale = 0;
                    if (work->state == 3) {
                        work->state = 4;
                    }
                }
            }
        } else if (work->state == 2) {
            if (ring->scale < 0x4000) {
                ring->scale += work->step * 2048;
                if (ring->scale >= 0x4000) {
                    ring->scale = 0x4000;
                    work->state = 3;
                }
            }
        } else if (work->state == 4) {
            if (ring->scale > 0) {
                ring->scale -= work->step * 256;
                if (ring->scale <= 0) {
                    ring->scale = 0;
                }
            }
        }
    }
}
