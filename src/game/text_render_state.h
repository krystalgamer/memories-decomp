#ifndef MEMORIES_DECOMP_TEXT_RENDER_STATE_H
#define MEMORIES_DECOMP_TEXT_RENDER_STATE_H

#include "../types.h"
#include "sorted_entry.h"

/* The two text-render state setters. func_80035668 publishes the render flags
 * word and stores 0x808080 in the unverified companion global; func_80035680
 * installs the sorted entry list and raises bit 2 of the same flags word.
 *
 * func_80035680 takes the list itself: both globals it writes are
 * SortedEntry *, so the parameter was a u32 only to be cast twice on the way
 * in. Nothing in C calls it, so the typed declaration costs no call site.
 *
 * func_80035668's parameter stays u32. It is a flags word, and the two files
 * that reached it spelled it s32; the value is a bit pattern either way, so
 * that difference is only a spelling. */
void func_80035668(u32 value);
void func_80035680(SortedEntry *list);

#endif
