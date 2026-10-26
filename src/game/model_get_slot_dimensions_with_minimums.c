#include "../types.h"
#include "model_get_slot_dimensions_with_minimums.h"
#include "model.h"

/* Copies the four cached halfwords at record[idx]+0xDC8 into *out, then
   clamps each axis up to the per-record minimum the slot's
   CF8 prefix carries for it: min_x, min_y and min_z, each a byte multiplied
   by 16, with 0 meaning "no clamp". Finally sets out->max to the largest of
   {x (if positive), y, z}. */
void Model_GetSlotDimensionsWithMinimums(s32 idx, ModelEffectAdjustment *out) {
    ModelSlot *rec = &gModel_aSlots[idx];
    ModelSlotCF8Prefix *cf8 = &rec->field_CF8.prefix;

    *out = *(ModelEffectAdjustment *)rec->cached_dimensions;

    if (cf8->thresholds.min_x != 0) {
        s32 t = cf8->thresholds.min_x << 4;
        if (out->x < t) {
            out->x = t;
        }
    }
    if (cf8->thresholds.min_y != 0) {
        s32 t = cf8->thresholds.min_y << 4;
        if (out->y < t) {
            out->y = t;
        }
    }
    if (cf8->thresholds.min_z != 0) {
        s32 t = cf8->thresholds.min_z << 4;
        if (out->z < t) {
            out->z = t;
        }
    }

    out->max = 0;
    if (out->x > 0) {
        out->max = out->x;
    }
    if (out->max < out->y) {
        out->max = out->y;
    }
    if (out->max < out->z) {
        out->max = out->z;
    }
}
