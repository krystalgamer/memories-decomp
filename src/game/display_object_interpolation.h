#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_INTERPOLATION_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_INTERPOLATION_H

#include "../types.h"

/* Fields the interpolation helpers read and write while easing an object
 * between its current pair at 0x36/0x38 and a target, leaving the result in
 * the pair at 0x30/0x32. Other translation units describe the same memory
 * through their own wider views.
 *
 * The halfword at 0x34 is padding here in the same sense as everywhere else:
 * these two functions do not touch it. That is not evidence that the memory
 * is unused, and it must not be treated as free space. The canonical record
 * in display_object.h spans it with pad_34[8], and func_80043178.h's view of
 * the same object spans it with pad_34[2]. */
typedef struct {
    u8 pad_00[0x30];
    s16 out_x;
    s16 out_y;
    u8 pad_34[0x02];
    s16 x;
    s16 y;
} DisplayObjectPosition;

void func_8004318C(
    DisplayObjectPosition *object, s32 arg1, s32 arg2, s32 arg3);
void func_80043230(
    DisplayObjectPosition *object,
    int target_x,
    int target_y,
    int phase);

#endif
