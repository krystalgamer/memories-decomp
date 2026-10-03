#include "../../types.h"
#include "variant373_quads.h"

void func_8013DE68(u8 *context)
{
    Family373QuadView *work = (Family373QuadView *)context;
    SVECTOR rotation;
    /* Retail leaves 16 bytes between rotation and scale. */
    u8 unknown_stack[16];
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Family373QuadGroup *group = &work->group;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    s32 pulse;
    s32 depth;

    ot = func_80058F10();
    quad = &work->quad;
    for (i = 0; i < 1; i++, group++) {
        if (work->phase == 8) {
            group->color[0].r = work->brightness;
            group->color[0].g = work->brightness;
            group->color[0].b = work->brightness;
            group->color[1].r = 0;
            group->color[1].g = work->brightness * 128 / 255;
            group->color[1].b = work->brightness;
        }
        pulse = 0;
        if (work->flags & 1) {
            pulse = work->scale / 8;
        }
        setVector(&rotation, 0, 0, 0);
        matrix.t[0] = work->origin[0] + work->delta[0] * work->factor / 1024;
        matrix.t[1] = work->origin[1] + work->delta[1] * work->factor / 1024;
        matrix.t[2] = work->origin[2] + work->delta[2] * work->factor / 1024;
        scale.vx = work->scale + pulse;
        scale.vy = work->scale + pulse;
        scale.vz = work->scale + pulse;
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
                &group->points[0][j], &group->points[1][j],
                &group->points[2][j], &group->points[3][j],
                (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                &interpolation, &flag);
            quad->r0 = group->color[1].r;
            quad->g0 = group->color[1].g;
            quad->b0 = group->color[1].b;
            quad->r1 = group->color[1].r;
            quad->g1 = group->color[1].g;
            quad->b1 = group->color[1].b;
            quad->r2 = group->color[1].r;
            quad->g2 = group->color[1].g;
            quad->b2 = group->color[1].b;
            quad->r3 = group->color[0].r;
            quad->g3 = group->color[0].g;
            quad->b3 = group->color[0].b;
            if (depth > 0) {
                if (depth < 2048) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (work->index_242C + 1 == work->config->count_0C) {
            if (work->phase == 0) {
                if (work->scale < 4096) {
                    work->scale = (work->elapsed << 12) / work->config->duration_14;
                    if (work->scale >= 4096) {
                        work->scale = 4096;
                        work->phase = 1;
                    }
                }
            } else if (work->phase == 3) {
                if (work->factor <= 1024) {
                    work->factor += work->step * 48;
                    if (work->factor >= 1024) {
                        work->factor = 1024;
                        work->phase = 4;
                    }
                }
            } else if (work->phase == 4 || work->phase == 5) {
                if (work->scale < 8192) {
                    work->scale += work->step * 512;
                    if (work->scale >= 8192) {
                        work->scale = 8192;
                    }
                }
            } else if (work->phase == 6) {
                if (work->scale > 0) {
                    work->scale -= work->step * 32;
                    if (work->scale <= 64) {
                        work->scale = 64;
                        work->phase = 7;
                    }
                }
            } else if (work->phase == 7) {
                if (work->scale < 10240) {
                    work->scale += work->step * 2560;
                    if (work->scale >= 10240) {
                        work->scale = 10240;
                    }
                }
            } else if (work->phase == 8) {
                if (work->brightness > 0) {
                    work->brightness -= work->step * 8;
                    if (work->brightness <= 0) {
                        work->brightness = 0;
                        work->phase = 9;
                    }
                }
            }
        }
    }
}
