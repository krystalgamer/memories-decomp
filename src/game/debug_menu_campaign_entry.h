#ifndef MEMORIES_DECOMP_DEBUG_MENU_CAMPAIGN_ENTRY_H
#define MEMORIES_DECOMP_DEBUG_MENU_CAMPAIGN_ENTRY_H

#include "../types.h"

/* Entry 2 of the frontend step table gDebugMenu_apfnPrimaryPageSteps (frontend_step_tables.c).
 * Switches between message-preview and campaign-index editors, preserving
 * the dialog-choice cleanup before returning to the editor. */
void DebugMenu_UpdateCampaignEntry(void);

#endif
