#ifndef MEMORIES_DECOMP_MEM_CARD_DIRECTORY_H
#define MEMORIES_DECOMP_MEM_CARD_DIRECTORY_H

#include "../types.h"

/* Walks a run of memory card directory entries.
 *
 * Both take the first entry and the number of entries, and step by
 * MEM_CARD_DIRECTORY_ENTRY_SIZE. MemCard_CalcFreeBlocks returns the blocks
 * still free after charging every entry's byte count at 0x18 to whole blocks;
 * MemCard_FindEntry returns the index of the entry whose name matches, or -1.
 *
 * MemCard_FindEntry's first parameter is the name to look for, not a number:
 * the body hands it straight to strcmp. */
s32 MemCard_CalcFreeBlocks(u8 *entry, s32 count);
s32 MemCard_FindEntry(u8 *name, u8 *entry, s32 count);

/* Current directory-entry buffer and the number of records loaded into it. */
extern u8 *D_8009B444;
extern s32 D_8009B440;

#endif
