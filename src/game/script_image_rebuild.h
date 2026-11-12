#ifndef MEMORIES_DECOMP_SCRIPT_IMAGE_REBUILD_H
#define MEMORIES_DECOMP_SCRIPT_IMAGE_REBUILD_H
#include "../types.h"
#include "script_image_objects.h"

#define SCRIPT_IMAGE_OVERLAY_FIRST_PRESENT 1
#define SCRIPT_IMAGE_OVERLAY_SECOND_PRESENT 2
#define SCRIPT_IMAGE_OVERLAY_SECOND_SCREEN_SPACE 0x80

/* The two overlay tables ScriptImage_RebuildObjects reads when it rebuilds a
 * scene-script
   image record. Declared here so the source that defines them and the one
   that reads them cannot drift apart. */
/* Ids 0x100-0x1FF: which overlays to create, and the callback-index byte
 * copied into the second overlay slot's control halfword at set +0x2C. */
typedef struct {
    u8 mask;
    u8 callback_index;
} ScriptImageOverlayPair;

/* Ids from 0x200: the same mask (bit 7 also puts the second overlay in
 * screen space), an x and y for each overlay, then the same callback index. */
typedef struct {
    u8 mask;
    u8 x1;
    u8 y1;
    u8 x2;
    u8 y2;
    u8 callback_index;
} ScriptImageOverlayPlacement;

/* Thirteen placements fill 78 of the table's 80 bytes; the last two are
 * retail padding before the script command table that follows. */
typedef struct {
    ScriptImageOverlayPlacement rows[13];
    u8 pad_4E[2];
} ScriptImageOverlayPlacementTable;

extern ScriptImageOverlayPair gScriptImage_OverlayPairs[];
extern ScriptImageOverlayPlacementTable gScriptImage_OverlayPlacements;

/* Rebuilds the display objects for one scene-script image record. `value` is
 * the packed image id, or negative to reuse the one already stored at the
 * record's +0x3C; either way that halfword ends up holding what was built.
 *
 * Ids from 0x200 up are the gScriptImage_OverlayPlacements table and get a full-screen object plus
 * up to two overlays positioned from the table row; below that the object
 * comes from ScriptImage_CreateObject and, from 0x100 up, gScriptImage_OverlayPairs's flag
 * byte decides
 * which of the two overlays exist. Under 0x100 the record's two overlay words
 * are just cleared.
 *
 * Both callers hold a wider scene-script allocation and pass its
 * ScriptImageObjectSet prefix. */
void ScriptImage_RebuildObjects(ScriptImageObjectSet *record, s32 image_id);
#endif
