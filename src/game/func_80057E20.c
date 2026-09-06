#include "../types.h"
#include "model.h"

typedef struct {
    u16 x, y, z, w;
} Vec4s;

/* Copies the 8-byte {x,y,z} vector (+trailing pad word) at record[idx]+0xDC8
   into *out, then clamps each axis up to a per-record minimum threshold
   (record+0xCFF/0xD00/0xD01, each a byte multiplied by 16; 0 means "no
   clamp"). Finally sets out->w to the largest of {x (if positive), y, z}. */
void func_80057E20(s32 idx, Vec4s *out) {
    ModelSlot *rec = &D_800F2C40[idx];
    u8 *thresh = rec->field_CF8.field_00;

    *out = *(Vec4s *)rec->field_DC8;

    if (thresh[7] != 0) {
        s32 t = thresh[7] << 4;
        if ((s16)out->x < t) {
            out->x = t;
        }
    }
    if (thresh[8] != 0) {
        s32 t = thresh[8] << 4;
        if ((s16)out->y < t) {
            out->y = t;
        }
    }
    if (thresh[9] != 0) {
        s32 t = thresh[9] << 4;
        if ((s16)out->z < t) {
            out->z = t;
        }
    }

    out->w = 0;
    if ((s16)out->x > 0) {
        out->w = out->x;
    }
    if ((s16)out->w < (s16)out->y) {
        out->w = out->y;
    }
    if ((s16)out->w < (s16)out->z) {
        out->w = out->z;
    }
}
