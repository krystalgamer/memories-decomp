#include "../../types.h"
#include "variant402_bands.h"

/* The French 402 and 412 entries share this drawer; it reads the entry
 * state of whichever image builds it. */
#ifdef MODEL_VARIANT412_BANDS
#include "variant412_entry.h"
typedef Variant412EntryState Family402BandOwner;
#else
#include "variant402_entry.h"
typedef Variant402EntryState Family402BandOwner;
#endif

void FrenchModelVariant402_DrawBands(u8 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    Family402BandOwner *work;
    ModelVariantCurtain *band;
    GsOT *ot;
    Family402BandPacket *quad;
    s32 i, j;
#ifndef MODEL_VARIANT412_BANDS
    s32 direction;
#endif
    s32 angle;
    s32 bearing;
#ifdef MODEL_VARIANT412_BANDS
    s32 pulse;
#endif
    s32 yaw;
#ifndef MODEL_VARIANT412_BANDS
    s32 outer_radius;
    s32 outer_depth;
#endif
    s32 size;
    s32 depth;
#ifndef MODEL_VARIANT412_BANDS
    s32 z;
#endif
    s16 fade;
    u8 r0, g0, b0, r1, g1, b1;

    work = (Family402BandOwner *)context;
    band = work->bands;
    ot = Graphics_GetDefaultOrderingTable();
#ifndef MODEL_VARIANT412_BANDS
    direction = -1;
#endif
    bearing = ratan2(work->aim.direction.vz, work->aim.direction.vx);
    yaw = bearing + 2048;
    ratan2(work->aim.direction.vy, work->aim.direction.vz);
#ifdef MODEL_VARIANT412_BANDS
    quad = &work->band_sprite;
#else
    quad = &work->band_packet;
#endif
#ifndef MODEL_VARIANT412_BANDS
    if (work->slot == 0) {
        direction = 1;
    }
#endif
    if (yaw < 2048) {
        yaw = -yaw + 1024;
    } else {
        yaw = bearing + 3072;
    }
#ifdef MODEL_VARIANT412_BANDS
#ifdef MODEL_VARIANT412_BANDS
    pulse = (work->frame_count & 1) * 64;
#else
    pulse = (work->clock.frame_count & 1) * 64;
#endif
#else
#ifdef MODEL_VARIANT412_BANDS
    outer_radius = (work->frame_count & 1) * 64 + 512;
#else
    outer_radius = (work->clock.frame_count & 1) * 64 + 512;
#endif
#ifdef MODEL_VARIANT412_BANDS
    outer_depth = (work->frame_count & 1) * 64 + 192;
#else
    outer_depth = (work->clock.frame_count & 1) * 64 + 192;
#endif
#endif
    for (i = 0; i < 2; i++, band++) {
        if (band->scale > 0) {
            size = band->scale;
#ifndef MODEL_VARIANT412_BANDS
            z = outer_depth * direction;
#endif
            for (j = 0, angle = 0; j < 17; j++, angle = j * 256) {
                band->a[j].vx = (u32)rcos(angle) >> 4;
                band->a[j].vy = (u32)rsin(angle) >> 4;
                band->a[j].vz = 0;
#ifdef MODEL_VARIANT412_BANDS
                setVector(&band->b[j], rcos(angle) * (pulse + 512) >> 12,
                          rsin(angle) * (pulse + 512) >> 12, pulse + 192);
#else
                setVector(&band->b[j], rcos(angle) * outer_radius >> 12,
                          rsin(angle) * outer_radius >> 12, z);
#endif
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
#ifndef MODEL_VARIANT412_BANDS
                setRGB1(quad, r0, g0, b0);
                setRGB2(quad, r1, g1, b1);
                setRGB3(quad, r1, g1, b1);
#endif
#ifdef MODEL_VARIANT412_BANDS
                if (depth > 0) {
#else
                if (depth >= 0 && flag >= 0) {
#endif
                    GsSortPoly(quad, ot, depth & 0xFFFF);
                }
            }
        }
        if (band->scale < 4096) {
#ifdef MODEL_VARIANT412_BANDS
            band->scale += work->step * 256;
#else
            band->scale += work->clock.step * 256;
#endif
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
