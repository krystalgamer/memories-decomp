#include "../../types.h"
#include "variant402_bands.h"

#include "variant402_entry.h"

void FrenchModelVariant402_DrawBands(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Variant402EntryState *work;
    ModelVariantCurtain *band;
    GsOT *ot;
    POLY_GT4 *quad;
    s32 i, j;
    s32 direction;
    s32 angle;
    s32 bearing;
    s32 yaw;
    s32 outer_radius;
    s32 outer_depth;
    s32 size;
    s32 depth;
    s32 z;
    s16 fade;
    u8 r0, g0, b0, r1, g1, b1;

    work = (Variant402EntryState *)context;
    band = work->bands;
    ot = Graphics_GetDefaultOrderingTable();
    direction = -1;
    bearing = ratan2(work->aim.direction.vz, work->aim.direction.vx);
    yaw = bearing + 2048;
    ratan2(work->aim.direction.vy, work->aim.direction.vz);
    quad = &work->band_packet;
    if (work->slot == 0) {
        direction = 1;
    }
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw = bearing + 3072;
    }
    outer_radius = (work->clock.frame_count & 1) * 64 + 512;
    outer_depth = (work->clock.frame_count & 1) * 64 + 192;
    for (i = 0; i < 2; i++, band++) {
        if (band->scale > 0) {
            size = band->scale;
            z = outer_depth * direction;
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                band->a[j].vx = (u32)rcos(angle) >> 4;
                band->a[j].vy = (u32)rsin(angle) >> 4;
                band->a[j].vz = 0;
                setVector(&band->b[j], rcos(angle) * outer_radius >> 12,
                          rsin(angle) * outer_radius >> 12, z);
            }
            rotation.vx = 0;
            rotation.vy = yaw;
            rotation.vz = 0;
            matrix.t[0] = work->aim.target.vx;
            matrix.t[1] = work->aim.target.vy;
            matrix.t[2] = work->aim.target.vz;
            scale.vx = size;
            scale.vy = size;
            scale.vz = size;
            RotMatrix(&rotation, &matrix);
            ScaleMatrix(&matrix, &scale);
            coordinate.coord = matrix;
            coordinate.super = 0;
            coordinate.flg = 0;
            GsGetLs(&coordinate, &light);
            GsSetLsMatrix(&light);
            if (band->scale < 2048) {
                r0 = work->inner.r;
                g0 = work->inner.g;
                b0 = work->inner.b;
                r1 = work->outer.r;
                g1 = work->outer.g;
                b1 = work->outer.b;
            } else {
                fade = 4096 - band->scale;
                r0 = work->inner.r * fade / 2048;
                g0 = work->inner.g * fade / 2048;
                b0 = work->inner.b * fade / 2048;
                r1 = work->outer.r * fade / 2048;
                g1 = work->outer.g * fade / 2048;
                b1 = work->outer.b * fade / 2048;
            }
            for (j = 0; j < 16; j++) {
                depth = RotTransPers4(&band->a[j], &band->a[j + 1],
                                     &band->b[j], &band->b[j + 1],
                                     (long *)&quad->x0, (long *)&quad->x1,
                                     (long *)&quad->x2, (long *)&quad->x3,
                                     &interpolation, &flag);
                setRGB0(quad, r0, g0, b0);
                setRGB1(quad, r0, g0, b0);
                setRGB2(quad, r1, g1, b1);
                setRGB3(quad, r1, g1, b1);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, depth & 0xFFFF);
                }
            }
        }
        if (band->scale < 4096) {
            band->scale += work->clock.step * 256;
            if (band->scale >= 4096) {
                band->scale = 4096;
                band->count++;
                if (i + 1 == 2 && work->phase == 2) {
                    work->phase = 3;
                }
            }
        }
    }
}
