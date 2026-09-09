#ifndef MEMORIES_DECOMP_FUNC_80016778_H
#define MEMORIES_DECOMP_FUNC_80016778_H

#include "../types.h"

/* The per-frame update callback the duel card display and icon setup install
   in a display object's +0x10 slot. It takes two arguments, not none: the
   object, and a word whose sign bit it stores at +0x69. Both installers had
   declared it `void (void)`, which is what that slot's struct view says, so
   assigning it needs a cast at each site. */
void func_80016778(void *object, u32 value);

#endif
