#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_API_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_API_H

#include "../types.h"

s32 func_8004002C(void);
void *func_800400AC(s32 index, s32 key);
void *func_80042B40(s32 value);
void func_8004036C(void *object);
void func_800404CC(
    void *object, s32 x, s32 y, s32 field_67, s32 field_68,
    s32 field_69, s32 color, s32 texture
);

#endif
