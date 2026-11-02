#ifndef MEMORIES_DECOMP_FRONTEND_DEBUG_CONSTANTS_H
#define MEMORIES_DECOMP_FRONTEND_DEBUG_CONSTANTS_H

#include "../types.h"

extern u16 gDebugMenu_wMessageId;
extern u16 gDebugMenu_wCampaignScene;
/* The four nibble masks the value editor applies per digit.
 * DebugMenu_UpdateValueEditor (debug_menu_editor_entries.c) copies all
 * eight bytes with a Bytes8 assignment; the unsized declaration is what
 * gives that read cc1psx's %hi/%lo pair rather than a gp-relative one. */
extern u16 gDebugMenu_awNibbleMasks[];

#endif
