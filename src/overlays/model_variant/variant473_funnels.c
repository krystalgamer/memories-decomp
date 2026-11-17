#include "../../types.h"

#include "../model_variant/variant473_entry.h"

#define FIRST_OBJECT(object, state) \
    do { \
        (object) = (state)->meshes; \
    } while (0)

/* Five funnels of seventeen-point rings at the object positions: an inner ring
 * of a fixed radius and an outer ring 160 plus the flicker wider, set back by 256
 * plus the flicker, drawn as sixteen POLY_GT4 quads scaled by each object's size.
 * Past 0x1000 the colours fade with the size; the rings spin by step * 80. */
void SpanishModelVariant473_DrawFunnels(EntryState473 *context)
{
    SVECTOR rotation;
    VECTOR scale;
    MATRIX matrix;
    MATRIX light;
    GsCOORDINATE2 coordinate;
    long interpolation;
    long flag;
    EntryState473 *state = context;
    ModelVariantSheet *sheet;
    ModelVariantFunnel *funnel;
    ModelVariantGridSheet *object;
    POLY_GT4 *quad;
    GsOT *ot;
    s32 i, k;
    s32 turn, yaw, pitch;
    s32 bias;
    s32 radius;
    s32 reach;
    s32 depth;
    s32 size;
    s32 angle;
    u8 r, g, b, outer_r, outer_g, outer_b;

    sheet = state->sheets;
    FIRST_OBJECT(object, state);
    ot = Graphics_GetDefaultOrderingTable();
    funnel = state->funnels;
    turn = ratan2(state->direction.vz, state->direction.vx);
    yaw = turn + 0x800;
    pitch = ratan2(state->direction.vy, state->direction.vz);
    quad = &state->funnel_packet;
    pitch += 0x800;
    if (yaw < 0x800) {
        yaw = -yaw + 0x400;
    } else {
        yaw = turn + 0xC00;
    }
    bias = (((s32)state->clock.frame_count & 1) << 5) * sheet[1].size / 4096;
    radius = rsin(1300) * 256 >> 12;
    for (i = 0; i < 5; i++, funnel++, object++) {
        if (state->size >= 0) {
            reach = radius;
            reach += 160 + bias;
            size = object->size;
            angle = state->spin;
            for (k = 0; k < 17; k++, angle = state->spin + k * 0x100) {
                setVector(&funnel->inner[k], rcos(angle) * radius >> 12, 0, rsin(angle) * radius >> 12);
                setVector(&funnel->outer[k], rcos(angle) * reach >> 12, -(bias + 256),
                          rsin(angle) * reach >> 12);
            }
            rotation.vx = 0;
            rotation.vy = 0;
            rotation.vz = 0;
            matrix.t[0] = state->origins[i].vx;
            matrix.t[1] = state->origins[i].vy - (rcos(1300) * 256 >> 12);
            matrix.t[2] = state->origins[i].vz;
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
            if (object->size < 4096) {
                r = state->inner.r;
                g = state->inner.g;
                b = state->inner.b;
                outer_r = state->outer.r;
                outer_g = state->outer.g;
                outer_b = state->outer.b;
            } else {
                r = state->inner.r * (8192 - object->size) / 4096;
                g = state->inner.g * (8192 - object->size) / 4096;
                b = state->inner.b * (8192 - object->size) / 4096;
                outer_r = state->outer.r * (8192 - object->size) / 4096;
                outer_g = state->outer.g * (8192 - object->size) / 4096;
                outer_b = state->outer.b * (8192 - object->size) / 4096;
            }
            setRGB0(quad, r, g, b);
            setRGB1(quad, r, g, b);
            setRGB2(quad, outer_r, outer_g, outer_b);
            setRGB3(quad, outer_r, outer_g, outer_b);
            for (k = 0; k < 16; k++) {
                depth = RotTransPers4(&funnel->inner[k], &funnel->inner[k + 1],
                                     &funnel->outer[k], &funnel->outer[k + 1],
                                     (long *)&quad->x0, (long *)&quad->x1,
                                     (long *)&quad->x2, (long *)&quad->x3,
                                     &interpolation, &flag);
                if (depth >= 0 && flag >= 0) {
                    GsSortPoly(quad, ot, (u16)depth);
                }
            }
        }
    }
    state->spin += state->clock.step * 80;
}
