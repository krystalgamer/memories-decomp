#include "../../types.h"
#include "e539_h356_entry.h"

void FrenchModelVariant373_DrawSphere(EntryState373 *context)
{
    EntryState373 *work = context;
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    ModelVariantMesh *sphere;
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
    sphere = &work->sphere;
    if (work->slot == 0) {
        setVector(&rotation, work->rotation, 0, 0);
    } else {
        setVector(&rotation, -work->rotation, 0, 0);
    }
    matrix.t[0] = work->matrix.t[0] + work->aim.direction.vx * work->translation / 1024;
    y = work->matrix.t[1] + work->aim.direction.vy * work->translation / 1024 - 256;
    matrix.t[1] = y + work->translation / 4;
    matrix.t[2] = work->matrix.t[2] + work->aim.direction.vz * work->translation / 1024;
    scale.vx = work->sphere_scale;
    scale.vy = work->sphere_scale;
    scale.vz = work->sphere_scale;
    RotMatrix(&rotation, &matrix);
    ScaleMatrix(&matrix, &scale);
    coordinate.coord = matrix;
    coordinate.super = 0;
    coordinate.flg = 0;
    GsGetLs(&coordinate, &light);
    GsSetLsMatrix(&light);
    if (work->phase == 5) {
        work->sphere_brightness = 1024 - (work->sphere_scale - 8192) / 8;
        for (i = 0; i < 9; i++) {
            sphere->colors[i].r = i * 255 / 8 * work->sphere_brightness / 1024;
            sphere->colors[i].g = work->sphere_brightness / 16;
            sphere->colors[i].b = (255 - i * 255 / 8) * work->sphere_brightness / 1024;
        }
    } else if (work->phase >= 6) {
        for (i = 0; i < 9; i++) {
            sphere->colors[i].r = work->sphere_brightness / 8;
            sphere->colors[i].g = work->sphere_brightness / 8;
            sphere->colors[i].b = work->sphere_brightness / 8;
        }
    }
    for (i = 0, quad = work->packets.sphere, texture_v = work->texture_offset;
         i < 8; i++, quad++, texture_v = -i * 16 + work->texture_offset) {
        setUV4(quad, 32, texture_v + 127, 95, texture_v + 127,
               32, texture_v + 111, 95, texture_v + 111);
        setRGB0(quad, sphere->colors[i].r, sphere->colors[i].g, sphere->colors[i].b);
        setRGB1(quad, sphere->colors[i].r, sphere->colors[i].g, sphere->colors[i].b);
        setRGB2(quad, sphere->colors[i + 1].r, sphere->colors[i + 1].g, sphere->colors[i + 1].b);
        setRGB3(quad, sphere->colors[i + 1].r, sphere->colors[i + 1].g, sphere->colors[i + 1].b);
        for (j = 0; j < 16; j++) {
            depth = RotTransPers4(
                &sphere->rows[i][j], &sphere->rows[i][j + 1],
                &sphere->rows[i + 1][j], &sphere->rows[i + 1][j + 1],
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
    if (work->clock.frame > work->config->sphere_start && work->sphere_scale < 4096) {
        work->sphere_scale = ((work->clock.frame - work->config->sphere_start) << 12)
                      / (work->config->scale_end - work->config->sphere_start);
        if (work->sphere_scale >= 4096) {
            work->sphere_scale = 4096;
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
        if (work->sphere_scale < 8192) {
            work->sphere_scale += work->clock.step * 512;
            if (work->sphere_scale >= 8192) {
                work->sphere_scale = 8192;
                work->phase = 4;
            }
        }
    } else if (work->phase == 4) {
        work->sphere_scale = (rcos(work->oscillation) * 2048 >> 12) + 6144;
        work->oscillation += work->clock.step << 6;
        if (work->config->sparks_start < work->clock.frame) {
            work->phase = 5;
        }
    } else if (work->phase == 5) {
        work->sphere_scale += work->clock.step * 256;
        if (work->sphere_scale >= 16384) {
            work->phase = 6;
            work->sphere_brightness = 1024;
        }
    } else if (work->phase == 6) {
        work->sphere_scale += work->clock.step * 256;
        if (work->sphere_brightness > 0) {
            work->sphere_brightness -= work->clock.step * 32;
            if (work->sphere_brightness <= 0) {
                work->sphere_brightness = 0;
                work->phase = 7;
            }
        }
    }
    if (work->clock.frame > work->config->translation_start) {
        work->rotation += work->clock.step * 64;
    }
}
