#include "../types.h"
#include "mem_card_directory.h"

/* Declared void, which is how it matched, but the index MemCard_FindEntry
 * returns is still in $v0 on the way out and the one caller, func_8003DC1C,
 * branches on its sign. */
void MemCard_FindLoadedEntry(u8 *name)
{
    MemCard_FindEntry(name, D_8009B444, D_8009B440);
}
