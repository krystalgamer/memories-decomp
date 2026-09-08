#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_API_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_API_H

#include "../types.h"

s32 func_8004002C(void);
void *func_800400AC(s32 index, s32 key);
void func_800404CC(
    void *object, s32 x, s32 y, s32 field_67, s32 field_68,
    s32 field_69, s32 color, s32 texture
);

#endif
