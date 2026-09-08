#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_LIFECYCLE_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_LIFECYCLE_H

#include "../types.h"

/* Fields func_80042B98 and func_80042BC0 read and write while stepping a
 * display object towards its target state. Other translation units describe
 * the same memory through their own narrower views. */
typedef struct {
    u8 pad_00[0x0C];
    u8 value_0C;
    u8 value_0D;
    u8 value_0E;
    u8 pad_0F[0x51];
    s16 step;
    u8 pad_62[0x0A];
    u8 flags;
} DisplayObjectLifecycle;

s32 func_80042B98(DisplayObjectLifecycle *object);
void func_80042BC0(DisplayObjectLifecycle *object);

#endif
