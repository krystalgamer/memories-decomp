#ifndef MEMORIES_DECOMP_TEXT_RENDER_STATE_H
#define MEMORIES_DECOMP_TEXT_RENDER_STATE_H

#include "../types.h"
#include "sorted_entry.h"

/* The two text-render state setters. Model_SetFogDrawFlags publishes the render flags
 * word and resets gModel_dwFogBaseColor to 0x808080;
 * SortedEntry_BeginCollection installs the sorted entry list and raises bit 2
 * of the same flags word.
 *
 * SortedEntry_BeginCollection takes the list itself: both globals it writes are
 * SortedEntry *, so the parameter was a u32 only to be cast twice on the way
 * in. Nothing in C calls it, so the typed declaration costs no call site.
 *
 * Model_SetFogDrawFlags's parameter stays u32. It is a flags word, and the two files
 * that reached it spelled it s32; the value is a bit pattern either way, so
 * that difference is only a spelling. */
void Model_SetFogDrawFlags(u32 value);

/* Keep the frozen overworld initializer's spelling. */
#define func_80035668 Model_SetFogDrawFlags
void SortedEntry_BeginCollection(SortedEntry *list);

#endif
