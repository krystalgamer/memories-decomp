#include "../../types.h"
#include "variant373_grid.h"

void func_8013C76C(u8 *context)
{
    Family373GridView *work = (Family373GridView *)context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    PSXLONG interpolation;
    PSXLONG flag;
    Family373Grid *grid;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i;
    s32 j;
    s32 depth;
    s32 texture_v;
    s32 y;

    ot = func_80058F10();
    ratan2(work->delta[2], work->delta[0]);
    ratan2(work->delta[1], work->delta[2]);
    grid = &work->grid;
    if (work->orientation == 0) {
        setVector(&rotation, work->rotation, 0, 0);
    } else {
        setVector(&rotation, -work->rotation, 0, 0);
    }
    matrix.t[0] = work->origin[0] + work->delta[0] * work->translation / 1024;
    y = work->origin[1] + work->delta[1] * work->translation / 1024 - 256;
    matrix.t[1] = y + work->translation / 4;
    matrix.t[2] = work->origin[2] + work->delta[2] * work->translation / 1024;
    scale.vx = work->scale;
    scale.vy = work->scale;
    scale.vz = work->scale;
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    if (work->phase == 5) {
        work->brightness = 1024 - (work->scale - 8192) / 8;
        for (i = 0; i < 9; i++) {
            grid->colors[i].r = i * 255 / 8 * work->brightness / 1024;
            grid->colors[i].g = work->brightness / 16;
            grid->colors[i].b = (255 - i * 255 / 8) * work->brightness / 1024;
        }
    } else if (work->phase >= 6) {
        for (i = 0; i < 9; i++) {
            grid->colors[i].r = work->brightness / 8;
            grid->colors[i].g = work->brightness / 8;
            grid->colors[i].b = work->brightness / 8;
        }
    }
    for (i = 0, quad = work->quads, texture_v = work->texture_offset;
         i < 8; i++, quad++, texture_v = -i * 16 + work->texture_offset) {
        setUV4(quad, 32, texture_v + 127, 95, texture_v + 127,
               32, texture_v + 111, 95, texture_v + 111);
        setRGB0(quad, grid->colors[i].r, grid->colors[i].g, grid->colors[i].b);
        setRGB1(quad, grid->colors[i].r, grid->colors[i].g, grid->colors[i].b);
        setRGB2(quad, grid->colors[i + 1].r, grid->colors[i + 1].g, grid->colors[i + 1].b);
        setRGB3(quad, grid->colors[i + 1].r, grid->colors[i + 1].g, grid->colors[i + 1].b);
        for (j = 0; j < 16; j++) {
            depth = RotTransPers4(
                &grid->points[i][j], &grid->points[i][j + 1],
                &grid->points[i + 1][j], &grid->points[i + 1][j + 1],
                (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                &interpolation, &flag);
            if (depth >= 0 && flag >= 0) {
                GsSortPoly(quad, ot, (u16)depth);
            }
        }
    }
    if (work->texture_offset <= 128) {
        work->texture_offset += 8;
        if (work->texture_offset >= 128) {
            work->texture_offset = 0;
        }
    }
    if (work->elapsed > work->config->scale_start && work->scale < 4096) {
        work->scale = ((work->elapsed - work->config->scale_start) << 12)
                      / (work->config->scale_end - work->config->scale_start);
        if (work->scale >= 4096) {
            work->scale = 4096;
            work->phase = 1;
        }
    }
    if (work->elapsed > work->config->translation_start
        && work->translation < 1024 && work->phase < 3) {
        work->translation = ((work->elapsed - work->config->translation_start) << 10)
                            / (work->config->translation_end - work->config->translation_start);
        if (work->translation >= 1024) {
            work->translation = 1024;
            work->phase = 3;
        }
    }
    if (work->phase == 3) {
        if (work->scale < 8192) {
            work->scale += work->step * 512;
            if (work->scale >= 8192) {
                work->scale = 8192;
                work->phase = 4;
            }
        }
    } else if (work->phase == 4) {
        work->scale = (rcos(work->oscillation) * 2048 >> 12) + 6144;
        work->oscillation += work->step << 6;
        if (work->config->expansion_start < work->elapsed) {
            work->phase = 5;
        }
    } else if (work->phase == 5) {
        work->scale += work->step * 256;
        if (work->scale >= 16384) {
            work->phase = 6;
            work->brightness = 1024;
        }
    } else if (work->phase == 6) {
        work->scale += work->step * 256;
        if (work->brightness > 0) {
            work->brightness -= work->step * 32;
            if (work->brightness <= 0) {
                work->brightness = 0;
                work->phase = 7;
            }
        }
    }
    if (work->elapsed > work->config->translation_start) {
        work->rotation += work->step * 64;
    }
}
