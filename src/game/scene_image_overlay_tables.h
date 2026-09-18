#ifndef MEMORIES_DECOMP_SCENE_IMAGE_OVERLAY_TABLES_H
#define MEMORIES_DECOMP_SCENE_IMAGE_OVERLAY_TABLES_H

#include "../types.h"

/* The two overlay tables ScriptImage_RebuildObjects reads when it rebuilds a
 * scene-script
   image record. Declared here so the source that defines them and the one
   that reads them cannot drift apart. */
/* Ids 0x100-0x1FF: which of the two overlays to create, and the value the
 * second one's record takes at +0x2C. */
typedef struct {
    u8 mask;
    u8 value;
} ScriptImageOverlayPair;

/* Ids from 0x200: the same mask (bit 7 also puts the second overlay in
 * screen space), an x and y for each overlay, then the +0x2C value. */
typedef struct {
    u8 mask;
    u8 x1;
    u8 y1;
    u8 x2;
    u8 y2;
    u8 value;
} ScriptImageOverlayPlacement;

/* Thirteen placements fill 78 of the table's 80 bytes; the last two are
 * retail padding before the script command table that follows. */
typedef struct {
    ScriptImageOverlayPlacement rows[13];
    u8 pad_4E[2];
} ScriptImageOverlayPlacementTable;

extern ScriptImageOverlayPair D_80090BA8[];
extern ScriptImageOverlayPlacementTable D_80090C00;

#endif
