#include "../../types.h"
#include "variant373_entry.h"

void FrenchModelVariant373_DrawGrid(EntryState373 *context)
{
    EntryState373 *work = context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    ModelVariantDome *grid;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i;
    s32 j;
    s32 depth;
    s32 texture_v;
    s32 y;

    ot = Graphics_GetDefaultOrderingTable();
    ratan2(work->aim.direction.vz, work->aim.direction.vx);
    ratan2(work->aim.direction.vy, work->aim.direction.vz);
    grid = &work->grid;
    if (work->slot == 0) {
        setVector(&rotation, work->rotation, 0, 0);
    } else {
        setVector(&rotation, -work->rotation, 0, 0);
    }
    matrix.t[0] = work->matrix.t[0] + work->aim.direction.vx * work->translation / 1024;
    y = work->matrix.t[1] + work->aim.direction.vy * work->translation / 1024 - 256;
    matrix.t[1] = y + work->translation / 4;
    matrix.t[2] = work->matrix.t[2] + work->aim.direction.vz * work->translation / 1024;
    scale.vx = work->grid_scale;
    scale.vy = work->grid_scale;
    scale.vz = work->grid_scale;
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    if (work->phase == 5) {
        work->grid_brightness = 1024 - (work->grid_scale - 8192) / 8;
        for (i = 0; i < 9; i++) {
            grid->colors[i].r = i * 255 / 8 * work->grid_brightness / 1024;
            grid->colors[i].g = work->grid_brightness / 16;
            grid->colors[i].b = (255 - i * 255 / 8) * work->grid_brightness / 1024;
        }
    } else if (work->phase >= 6) {
        for (i = 0; i < 9; i++) {
            grid->colors[i].r = work->grid_brightness / 8;
            grid->colors[i].g = work->grid_brightness / 8;
            grid->colors[i].b = work->grid_brightness / 8;
        }
    }
    for (i = 0, quad = work->packets.grid, texture_v = work->texture_offset;
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
                (long *)&quad->x0, (long *)&quad->x1,
                (long *)&quad->x2, (long *)&quad->x3,
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
    if (work->clock.frame > work->config->grid_start && work->grid_scale < 4096) {
        work->grid_scale = ((work->clock.frame - work->config->grid_start) << 12)
                      / (work->config->scale_end - work->config->grid_start);
        if (work->grid_scale >= 4096) {
            work->grid_scale = 4096;
            work->phase = 1;
        }
    }
    if (work->clock.frame > work->config->translation_start
        && work->translation < 1024 && work->phase < 3) {
        work->translation = ((work->clock.frame - work->config->translation_start) << 10)
                            / (work->config->translation_end - work->config->translation_start);
        if (work->translation >= 1024) {
            work->translation = 1024;
            work->phase = 3;
        }
    }
    if (work->phase == 3) {
        if (work->grid_scale < 8192) {
            work->grid_scale += work->clock.step * 512;
            if (work->grid_scale >= 8192) {
                work->grid_scale = 8192;
                work->phase = 4;
            }
        }
    } else if (work->phase == 4) {
        work->grid_scale = (rcos(work->oscillation) * 2048 >> 12) + 6144;
        work->oscillation += work->clock.step << 6;
        if (work->config->points_start < work->clock.frame) {
            work->phase = 5;
        }
    } else if (work->phase == 5) {
        work->grid_scale += work->clock.step * 256;
        if (work->grid_scale >= 16384) {
            work->phase = 6;
            work->grid_brightness = 1024;
        }
    } else if (work->phase == 6) {
        work->grid_scale += work->clock.step * 256;
        if (work->grid_brightness > 0) {
            work->grid_brightness -= work->clock.step * 32;
            if (work->grid_brightness <= 0) {
                work->grid_brightness = 0;
                work->phase = 7;
            }
        }
    }
    if (work->clock.frame > work->config->translation_start) {
        work->rotation += work->clock.step * 64;
    }
}
