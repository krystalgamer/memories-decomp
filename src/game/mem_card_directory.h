#ifndef MEMORIES_DECOMP_MEM_CARD_DIRECTORY_H
#define MEMORIES_DECOMP_MEM_CARD_DIRECTORY_H

#include "../types.h"

/* Walks a run of memory card directory entries.
 *
 * Both take the first entry and the number of entries, and step by
 * MEM_CARD_DIRECTORY_ENTRY_SIZE. func_80044544 returns the blocks still free
 * after charging every entry's byte count at 0x18 to whole blocks;
 * func_80044598 returns the index of the entry whose name matches, or -1.
 *
 * func_80044598's first parameter is the name to look for, not a number: the
 * body hands it straight to strcmp. */
s32 func_80044544(u8 *entry, s32 count);
s32 func_80044598(u8 *name, u8 *entry, s32 count);

#endif
