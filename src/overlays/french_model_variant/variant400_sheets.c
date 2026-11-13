#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "variant400_entry.h"

void FrenchModelVariant400_DrawSheets(u8 *context)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long projection;
    long flag;
    Variant400EntryNode *node;
    ModelVariantSheet *quad;
    GsOT *ot;
    Variant400EntryState *work;
    POLY_GT4 *poly;
    s32 i;
    s32 k;
    s32 bias;
    s32 depth;

    work = (Variant400EntryState *)context;
    node = work->nodes;
    quad = (ModelVariantSheet *)work->sheets;
    ot = Graphics_GetDefaultOrderingTable();
    poly = &work->packets.textured[1];
    for (i = 0; i < 7; i++, quad++) {
        bias = 0;
        if (work->frame_count & 1) {
            bias = quad->size / 8;
        }
        setVector(&rot, 0, 0, 0);
        if (i == 0) {
            matrix.t[0] = work->matrix.t[0];
            matrix.t[1] = work->matrix.t[1];
            matrix.t[2] = work->matrix.t[2];
        } else if (i < 6) {
            matrix.t[0] = node->position.vx;
            matrix.t[1] = node->position.vy;
            matrix.t[2] = node->position.vz;
            node++;
        } else {
            matrix.t[0] = work->target.vx;
            matrix.t[1] = work->target.vy;
            matrix.t[2] = work->target.vz;
        }
        setVector(&scale, quad->size + bias, quad->size + bias, quad->size + bias);
        RotMatrix(&rot, &matrix);
        ScaleMatrix(&matrix, &scale);
        coordinate.coord = matrix;
        coordinate.super = 0;
        coordinate.flg = 0;
        GsGetLs(&coordinate, &light);
        GsSetLsMatrix(&light);
        ReadRotMatrix(&light);
        RotMatrix(&rot, &light);
        ScaleMatrix(&light, &scale);
        SetRotMatrix(&light);
        for (k = 0; k < 4; k++) {
            depth = RotTransPers4(
                &quad->v0[k], &quad->v1[k], &quad->v2[k], &quad->v3[k],
                (long *)&poly->x0, (long *)&poly->x1,
                (long *)&poly->x2, (long *)&poly->x3,
                &projection, &flag);
            poly->r0 = quad->inner[0];
            poly->g0 = quad->inner[1];
            poly->b0 = quad->inner[2];
            poly->r1 = quad->inner[0];
            poly->g1 = quad->inner[1];
            poly->b1 = quad->inner[2];
            poly->r2 = quad->inner[0];
            poly->g2 = quad->inner[1];
            poly->b2 = quad->inner[2];
            poly->r3 = quad->outer[0];
            poly->g3 = quad->outer[1];
            poly->b3 = quad->outer[2];
            depth = depth * 8 / 10;
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(poly, ot, depth);
            }
        }
        if (i == 0) {
            if (work->tail.phase == 0) {
                if (quad->size < 4096) {
                    quad->size = ((work->frame
                        - work->config->start) << 12)
                        / (work->config->grow_end
                        - work->config->start);
                    if (quad->size >= 4096) {
                        quad->size = 4096;
                        work->tail.phase = 1;
                    }
                }
            } else if (work->tail.phase == 1 || work->tail.phase == 2) {
                if (quad->size >= 0) {
                    quad->size -= work->step * 64;
                    if (quad->size <= 0) {
                        quad->size = 0;
                        if (work->segment_start == 16) {
                            work->tail.phase = 3;
                        }
                    }
                }
            }
        } else if (i < 6) {
            if (work->tail.phase == 2 || work->tail.phase == 3) {
                if (quad->size <= 4096) {
                    quad->size += work->step * 64;
                    if (quad->size >= 4096) {
                        quad->size = 4096;
                        if (work->tail.phase == 3) {
                            work->tail.phase = 4;
                        }
                    }
                }
            } else if (work->tail.phase >= 5) {
                if (work->frame
                    > work->config->end
                    && quad->size > 0) {
                    quad->size -= work->step * 128;
                    if (quad->size <= 0) {
                        quad->size = 0;
                        if (work->tail.phase == 5) {
                            work->tail.phase = 6;
                        }
                    }
                }
            }
        } else {
            if (work->tail.phase == 5 || work->tail.phase == 6) {
                if (quad->size < 8192) {
                    quad->size += work->step * 512;
                    if (quad->size >= 8192) {
                        quad->size = 8192;
                    }
                }
            } else if (work->tail.phase == 7 && quad->size > 0) {
                quad->size -= work->step * 64;
                if (quad->size <= 0) {
                    quad->size = 0;
                    work->tail.phase = 8;
                }
            }
        }
    }
}
