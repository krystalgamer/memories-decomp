#ifndef MEMORIES_DECOMP_FUNC_80043178_H
#define MEMORIES_DECOMP_FUNC_80043178_H

#include "../types.h"

/* The four fields func_80043178 touches. It copies the live pair at 0x30/0x32
 * into the saved pair at 0x36/0x38, so callers keep the previous values while
 * the live ones move on. Other translation units describe the same memory
 * through their own wider views. */
typedef struct {
    u8 pad_00[0x30];
    u16 field_30;
    u16 field_32;
    u8 pad_34[2];
    u16 field_36;
    u16 field_38;
} DisplayObjectSnapshot;

void func_80043178(DisplayObjectSnapshot *object);

#endif
