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
 * The record is the library motion state. func_8002BFCC passes D_800EA1E8 and
 * writes that same halfword itself on the line before the call --
 * `((LibraryMotionState *)r)->y = y` -- so the offset is that record's y,
 * which ygo_types.h's LibraryMotionState names at exactly 0xA.
 * Library_UpdateGridCursor is the second caller and passes its byte-view state
 * after handling cursor movement.
 *
 * The parameter stays u8 * rather than becoming LibraryMotionState *. That
 * first caller holds the record through library_runtime.h's
 * `u8 D_800EA1E8[]` byte view, while Library_UpdateGridCursor receives the
 * same state through its own u8 * dispatcher contract. The typed declaration
 * lives in func_8002A3CC.h; including it beside the byte global would create
 * conflicting declarations. The implementation therefore applies the
 * asserted LibraryMotionState layout internally and accesses its typed y
 * field. */
void func_8002A660(u8 *record);

#endif
