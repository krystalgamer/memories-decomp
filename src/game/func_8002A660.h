#ifndef MEMORIES_DECOMP_FUNC_8002A660_H
#define MEMORIES_DECOMP_FUNC_8002A660_H

#include "../types.h"

/* Keeps the vertical scroll following a record's y, and pins the horizontal
 * scroll to zero.
 *
 * It reads one halfword, at +0xA, three times: once signed to get the
 * distance from the current gGraphics_sViewportY, then twice unsigned to
 * rebuild the viewport when that distance leaves the 0x40..0xB0 band. The
 * band is a dead zone, so the viewport only moves when the record has left
 * it, and it is then placed 0x40 above or 0xB0 below the record.
 *
 * The record is the library motion state. Its one caller, func_8002BFCC,
 * passes D_800EA1E8 and writes that same halfword itself on the line before
 * the call -- `*(s16 *)(r + 0xA) = y` -- so the offset is that record's y,
 * which func_8002A3CC.h's LibraryMotionState names at exactly 0xA.
 *
 * The parameter stays u8 * rather than becoming LibraryMotionState *. That
 * caller reaches the same record through func_8002BAB4.h's `u8 D_800EA1E8[]`
 * byte view, and the typed view lives in func_8002A3CC.h; a file that
 * included both would have two declarations of the symbol. The two views are
 * kept apart on purpose, so this prototype takes the one its caller can
 * supply and records the identification here instead. */
void func_8002A660(u8 *record);

#endif
