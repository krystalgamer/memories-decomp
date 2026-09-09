#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_LIST_RENDERERS_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_LIST_RENDERERS_H

#include "../types.h"

/* Two more entries of D_80090FB0, the pair that builds packets in the
 * scratchpad rather than only running callbacks. func_80040DD8 takes the list
 * at D_800EFE38[4] and is 8 wide; func_80041068 takes D_800EFE38[5] and is 12
 * wide by 0x3C high. Both are reached only through that table. */
void func_80040DD8(void);
void func_80041068(void);

#endif
