#include "../../types.h"
#include "variant373_layers.h"

void func_8013C0B0(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light_matrix;
    GsCOORDINATE2 coordinate;
    /* Retail reserves 24 bytes before the two color locals. */
    u8 unknown_stack[24];
    CVECTOR dark;
    CVECTOR light;
    PSXLONG interpolation;
    PSXLONG flag;
    GsOT *ot;
    s32 i;
    Family373LayerView *work;
    Family373Layer *layer;
    POLY_GT4 *quad;
    s32 j;
    s32 fade;
    s32 depth;
    s32 amount;
    s32 all_done;
    s32 texture_v, angle, byte_offset;

    work = (Family373LayerView *)context;
    ot = func_80058F10();
    all_done = 1;
    GsGetActiveBuff();
    i = 0;
    layer = work->layers;
    ratan2(work->delta[2], work->delta[0]);
    ratan2(work->delta[1], work->delta[2]);
    do {
        if (layer->scale > 0) {
            rsin(layer->scale);
            if (work->phase < 2) {
                setVector(&rotation, 0, 0, 0);
                matrix.t[0] = work->origin[0];
                matrix.t[1] = work->origin[1] - layer->scale / 12;
                matrix.t[2] = work->origin[2];
                if (layer->scale < 1024 || layer->scale > 5120) {
                    amount = 1024;
                } else {
                    amount = 1024 - ((rsin(layer->scale) * (work->scale / 2) >> 12)
                                     - work->scale / 2);
                }
                setVector(&scale, amount, amount, amount);
            } else {
                rotation.vx = layer->rotation.vx;
                rotation.vy = layer->rotation.vy;
                rotation.vz = layer->rotation.vz;
                matrix.t[0] = work->target.vx;
                matrix.t[1] = work->target.vy;
                matrix.t[2] = work->target.vz;
                scale.vx = layer->scale;
                scale.vy = layer->scale;
                scale.vz = layer->scale;
            }
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light_matrix);
            GsSetLsMatrix(&light_matrix);
            if (layer->scale < 5120) {
                light.r = 192;
                light.g = 192;
                light.b = 192;
                dark.r = 192;
                dark.g = 128;
                dark.b = 64;
            } else {
                fade = layer->scale - 5120;
                light.r = 192 - fade * 192 / 1024;
                light.g = 192 - fade * 192 / 1024;
                light.b = 192 - fade * 192 / 1024;
                dark.r = 192 - fade * 192 / 1024;
                dark.g = 128 - fade / 8;
                dark.b = 64 - fade / 16;
            }
            for (j = 0, quad = work->quads, texture_v = 0, byte_offset = 0; j < 16;
                 j++, quad++, byte_offset = j * 8, texture_v = -j * 8) {
                setUV4(quad, 0, texture_v - 1, 0, texture_v - 16,
                       31, texture_v - 1, 31, texture_v - 16);
                depth = RotTransPers4(
                    &layer->points[j],
                    (SVECTOR *)((u8 *)layer + (byte_offset + 8)),
                    (SVECTOR *)((u8 *)layer + (byte_offset + 0x88)),
                    (SVECTOR *)((u8 *)layer + (byte_offset + 0x90)),
                    (PSXLONG *)&quad->x0, (PSXLONG *)&quad->x1,
                    (PSXLONG *)&quad->x2, (PSXLONG *)&quad->x3,
                    &interpolation, &flag);
                setRGB0(quad, light.r, light.g, light.b);
                setRGB1(quad, light.r, light.g, light.b);
                setRGB2(quad, dark.r, dark.g, dark.b);
                setRGB3(quad, dark.r, dark.g, dark.b);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
        if (work->phase < 2) {
            if (layer->scale < 6144) {
                layer->scale += work->step * 192;
                if (layer->scale >= 6144) {
                    if (work->config->start < work->elapsed) {
                        layer->scale = 6144;
                        layer->done = 1;
                    } else {
                        layer->scale -= 6144;
                    }
                }
            }
            all_done *= layer->done;
            if (i + 1 == 5 && all_done == 1 && work->phase == 1) {
                work->phase = 2;
            }
        } else if (work->phase == 2) {
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                setVector(&layer->points[j],
                          rcos(angle) * 384U / 4096, 0,
                          rsin(angle) * 384U / 4096);
                setVector(&layer->points[j + 17],
                          rcos(angle) * 576U / 4096, 16,
                          rsin(angle) * 576U / 4096);
                setVector(&layer->points[j + 34],
                          rcos(angle) * 768U / 4096, 16,
                          rsin(angle) * 768U / 4096);
            }
            layer->scale = -i * 6144 / 5;
        } else if (layer->scale < 6144) {
            layer->scale += work->step * 128;
            if (layer->scale >= 6144) {
                layer->scale -= 6144;
                layer->rotation.vx += 384;
                layer->rotation.vz += 800;
                if (work->phase >= 5) {
                    layer->scale = 6144;
                }
            }
        }
        i++;
        layer++;
    } while (i < 5);
}
