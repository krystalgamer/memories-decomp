#ifndef MEMORIES_DECOMP_CREDITS_RESIDENT_GLYPHS_H
#define MEMORIES_DECOMP_CREDITS_RESIDENT_GLYPHS_H

#include "../types.h"

/* Replacement bitmaps for the Shift-JIS codes 0xE056 ([0]) and 0x9B92 ([1]),
   fifteen two-byte rows each like the ROM-font glyphs. Resident read-only
   data in the North American and Japanese executables, read only by the
   credits overlay. */
extern const u8 gCredits_abReplacementGlyphs[2][15][2];

#endif
