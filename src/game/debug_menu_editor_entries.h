#ifndef MEMORIES_DECOMP_DEBUG_MENU_EDITOR_ENTRIES_H
#define MEMORIES_DECOMP_DEBUG_MENU_EDITOR_ENTRIES_H

#include "../types.h"

/* The primary debug page's two value-editor entries, adjacent in the image at
 * 0x800307B8 and 0x80030998. Both drive func_80030294's digit editor and
 * return through DebugMenu_RestorePrimaryDisplayObject when it cancels. */

/* Entry 5 of gDebugMenu_apfnPrimaryPageSteps (frontend_step_tables.c), the
 * "Sound" label: edits and plays sound IDs. */
void DebugMenu_UpdateSoundEntry(void);

/* Entry 2 of gDebugMenu_apfnPrimaryPageSteps (frontend_step_tables.c), the
 * "Campaign" label.
 * Switches between message-preview and campaign-index editors, preserving
 * the dialog-choice cleanup before returning to the editor. */
void DebugMenu_UpdateCampaignEntry(void);

#endif
