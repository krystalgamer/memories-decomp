#include "../types.h"
#include "mem_card_directory.h"

/* The legacy updater branches on the directory index returned in $v0. */
s32 MemCard_FindLoadedEntry(u8 *name)
{
    return MemCard_FindEntry(name, gMemCard_pDirEntries, gMemCard_nDirEntries);
}
