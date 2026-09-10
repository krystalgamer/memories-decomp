#ifndef MEMORIES_DECOMP_MEM_CARD_DIRECTORY_H
#define MEMORIES_DECOMP_MEM_CARD_DIRECTORY_H

#include "../types.h"
#include "../psyq/libapi.h"
#include "mem_card.h"

/* The BIOS fills these same records through firstfile/nextfile and LIBMCRD.
 * Keep the SDK type rather than maintaining a second game-owned layout. */
typedef char MemCardDirectoryEntry_size_must_match[
    sizeof(struct DIRENTRY) == MEM_CARD_DIRECTORY_ENTRY_SIZE ? 1 : -1
];
typedef char MemCardDirectoryEntry_name_offset_must_match[
    (u32)&((struct DIRENTRY *)0)->name == 0 ? 1 : -1
];
typedef char MemCardDirectoryEntry_size_offset_must_match[
    (u32)&((struct DIRENTRY *)0)->size == 0x18 ? 1 : -1
];

/* Walks a run of memory card directory entries.
 *
 * Both take the first entry and the number of entries, and step by
 * MEM_CARD_DIRECTORY_ENTRY_SIZE. MemCard_CalcFreeBlocks returns the blocks
 * still free after charging every entry's byte count at 0x18 to whole blocks;
 * MemCard_FindEntry returns the index of the entry whose name matches, or -1.
 *
 * MemCard_FindEntry's first parameter is the name to look for, not a number:
 * the body hands it straight to strcmp. */
s32 MemCard_CalcFreeBlocks(struct DIRENTRY *entry, s32 count);
s32 MemCard_FindEntry(u8 *name, struct DIRENTRY *entry, s32 count);
s32 MemCard_FindFiles(s32 chan, const char *pattern, struct DIRENTRY *cursor,
                     s32 *out_count);
s32 MemCard_DoLoadDirectory(void);

/* Current directory-entry buffer and the number of records loaded into it. */
extern struct DIRENTRY *gMemCard_pDirEntries;
extern struct DIRENTRY gMemCard_aDirEntries[];
extern s32 gMemCard_nDirEntries;
extern s32 gMemCard_nFreeBlocks;
extern u8 D_8009AF7C[];

#endif
