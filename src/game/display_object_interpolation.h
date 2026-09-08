#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_INTERPOLATION_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_INTERPOLATION_H

#include "../types.h"

/* Fields the interpolation helpers read and write while easing an object
 * between its current pair at 0x36/0x38 and a target, leaving the result in
 * the pair at 0x30/0x32. Other translation units describe the same memory
 * through their own wider views. */
typedef struct {
    u8 pad_00[0x30];
    s16 out_x;
    s16 out_y;
    s16 unused;
    s16 x;
    s16 y;
} DisplayObjectPosition;

void func_8004318C(u8 *arg0, s32 arg1, s32 arg2, s32 arg3);
void func_80043230(
    DisplayObjectPosition *object,
    int target_x,
    int target_y,
    int phase);

#endif
