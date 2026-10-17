#ifndef MEMORIES_DECOMP_LIBRARY_UPDATE_GRID_SCROLL_H
#define MEMORIES_DECOMP_LIBRARY_UPDATE_GRID_SCROLL_H

#include "../types.h"
#include "../ygo_types.h"

/* Keeps the vertical scroll following a record's y, and pins the horizontal
 * scroll to zero.
 *
 * It reads one halfword, at +0xA, three times: once signed to get the
 * distance from the current gGraphics_sViewportY, then twice unsigned to
 * rebuild the viewport when that distance leaves the 0x40..0xB0 band. The
 * band is a dead zone, so the viewport only moves when the record has left
 * it, and it is then placed 0x40 above or 0xB0 below the record.
 *
 * The record is the library motion state. Library_InitializeScreen passes gLibrary_State and
 * writes that same halfword itself on the line before the call --
 * `((LibraryMotionState *)r)->y = y` -- so the offset is that record's y,
 * which ygo_types.h's LibraryMotionState names at exactly 0xA.
 * Library_UpdateGridCursor is the second caller and passes &state->motion
 * after handling cursor movement.
 *
 * Both callers supply the asserted LibraryMotionState prefix. The record
 * is read-only; its signed first read and unsigned follow-up reads remain
 * distinct around the viewport store. */
void Library_UpdateGridScroll(const LibraryMotionState *record);

#endif
