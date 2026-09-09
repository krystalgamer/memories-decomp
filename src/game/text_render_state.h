#ifndef MEMORIES_DECOMP_TEXT_RENDER_STATE_H
#define MEMORIES_DECOMP_TEXT_RENDER_STATE_H

#include "../types.h"

/* The two text-render state setters. func_80035668 publishes the render flags
 * word and resets the colour to 0x808080; func_80035680 installs the sorted
 * entry list and raises bit 2 of the same flags word.
 *
 * The parameter is u32 in both definitions -- func_80035680 casts it straight to
 * a SortedEntry * -- and the two files that reached func_80035668 spelled it
 * s32. The value is a bit pattern either way, so the difference is only a
 * spelling. */
void func_80035668(u32 value);
void func_80035680(u32 value);

#endif
