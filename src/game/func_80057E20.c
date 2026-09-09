#include "../types.h"
#include "func_80057E20.h"
#include "model.h"

/* Copies the 8-byte {x,y,z} vector (+trailing pad word) at record[idx]+0xDC8
   into *out, then clamps each axis up to a per-record minimum threshold
   (record+0xCFF/0xD00/0xD01, each a byte multiplied by 16; 0 means "no
   clamp"). Finally sets out->max to the largest of {x (if positive), y, z}. */
void func_80057E20(s32 idx, ModelEffectAdjustment *out) {
    ModelSlot *rec = &D_800F2C40[idx];
    u8 *thresh = rec->field_CF8.field_00;

    *out = *(ModelEffectAdjustment *)rec->field_DC8;

    if (thresh[7] != 0) {
        s32 t = thresh[7] << 4;
        if (out->x < t) {
            out->x = t;
        }
    }
    if (thresh[8] != 0) {
        s32 t = thresh[8] << 4;
        if (out->y < t) {
            out->y = t;
        }
    }
    if (thresh[9] != 0) {
        s32 t = thresh[9] << 4;
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
