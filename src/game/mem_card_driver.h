#ifndef MEMORIES_DECOMP_MEM_CARD_DRIVER_H
#define MEMORIES_DECOMP_MEM_CARD_DRIVER_H
#include "../types.h"
#include "../psyq/libapi.h"
#include "mem_card.h"

/* Both take one card class's four result-event handles, in the order
 * MemCard_InitIOEvents registers them: I/O end, timeout, error, new card.
 * gMemCard_aIOEventHandles is the SwCARD set and gMemCard_aHwIOEventHandles
 * the HwCARD set. */
void MemCard_ClearIOEvents(long *handles);
s32 MemCard_WaitIOEvent(long *handles, s32 once);
void MemCard_Init(long val);
void MemCard_InitIOEvents(void);

/* Claims the single request slot the low-level card driver runs.
 *
 * Returns 0 while an earlier request is still pending (gMemCard_bRequest is
 * only negative when idle); otherwise records the channel and request code
 * in gMemCard_bChannel and gMemCard_bRequest, resets gMemCard_bRetries,
 * gMemCard_bLoadStep and gMemCard_bRequestStep, and marks gMemCard_nIOResult
 * pending. The matching controller MemCard_ProcessRequest dispatches on the code and puts
 * the slot back to -1 once it hands the caller a result:
 *
 *    1  MemCard_ReqCardInfo       _card_info only; reports the card's state
 *    2  MemCard_ReqLoadDirectory  info, clear and load; leaves the files listed
 *    3  MemCard_ReqReadFile       open O_RDONLY|O_NOWAIT, lseek, read
 *    4  MemCard_ReqWriteFile      open O_WRONLY|O_NOWAIT, lseek, write
 *    8  MemCard_ReqCreateFile     open O_CREAT with the block count
 *   11  MemCard_ReqReadSector     _card_read of one 128-byte sector
 *   12  MemCard_ReqWriteSector    _card_write of one 128-byte sector */
s32 MemCard_BeginRequest(s32 chan, s32 request);

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
typedef char MemCardDirectoryEntry_head_offset_must_match[
    (u32)&((struct DIRENTRY *)0)->head == 0x20 ? 1 : -1
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

/* Returns MemCard_FindEntry's index for name in the loaded directory. */
s32 MemCard_FindLoadedEntry(u8 *name);

/* Current directory-entry buffer and the number of records loaded into it. */
extern struct DIRENTRY *gMemCard_pDirEntries;
extern s32 gMemCard_nDirEntries;
extern s32 gMemCard_nFreeBlocks;
extern struct DIRENTRY gMemCard_aDirEntries[];

#endif
