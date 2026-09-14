#include "../types.h"
#include "frontend_step_tables.h"
#include "debug_menu_mapped_mode.h"
#include "debug_menu_exit.h"
#include "debug_menu_two_player_entry.h"
#include "frontend_scene_states.h"
#include "debug_menu_campaign_entry.h"
#include "debug_menu_bust_up_entry.h"
#include "../unmatched.h"

/* Initialized data at 0x80090D7C: the two step tables DebugMenu_Update
 * dispatches through.
 *
 * It picks between them on gDebugMenu_bPage and indexes the winner with
 * `D_8009B2EB & FRONTEND_STEP_INDEX_MASK`. Both are written here rather than
 * resolved out of the
 * blob at 0x80090BA8 because every entry is a function this tree already
 * names. All twenty-three are matching C; DebugMenu_UpdateCampaignEntry is
 * compiled separately and the next eight states share frontend_scene_states.c.
 *
 * The two are adjacent and that is load bearing. gDebugMenu_apfnAlternatePageSteps holds only two
 * entries while the index mask permits thirty-two, so an index above 1 runs
 * straight into gDebugMenu_apfnPrimaryPageSteps -- whose first entry is the same DebugMenu_ResetEntryState
 * that gDebugMenu_apfnAlternatePageSteps starts with. Whether that is deliberate reuse or simply an
 * index the game never produces is not established here, so the two arrays
 * are defined separately, in this order, exactly as the image has them.
 *
 * What this does NOT claim: the tables are attributed to the frontend
 * because their entries live in frontend_scene_states.c,
 * debug_menu_mapped_mode.c and
 * their neighbours in the 0x80030000 range, not because anything in the tree
 * names them. The state byte they index, D_8009B2EB, is also written by the
 * memory card paths, so this is not evidence that the tables are
 * frontend-only. */

void (*gDebugMenu_apfnAlternatePageSteps[])(void) = {
    DebugMenu_ResetEntryState,
    DebugMenu_UpdateTwoPlayerDuelEntry,
};

void (*gDebugMenu_apfnPrimaryPageSteps[])(void) = {
    DebugMenu_ResetEntryState,
    DebugMenu_EnterMappedMode,       /* 3D */
    DebugMenu_UpdateCampaignEntry,   /* Campaign */
    DebugMenu_EnterDuel,             /* DUEL */
    DebugMenu_EnterMappedMode,       /* Detail */
    DebugMenu_UpdateSoundEntry,      /* Sound */
    DebugMenu_UpdateBustUpEntry,     /* BustUp */
    DebugMenu_EnterMappedMode,       /* 3D MAP */
    DebugMenu_EnterDeckEditor,       /* DeckEdit */
    DebugMenu_EnterMappedMode,       /* FreeDUEL */
    DebugMenu_UpdateTitleEntry,      /* TITLE */
    DebugMenu_EnterMappedMode,       /* NAME */
    DebugMenu_EnterMappedMode,       /* Password */
    DebugMenu_UpdateMovieEntry,      /* MOVIE */
    DebugMenu_UpdateLoadEntry,       /* Load */
    DebugMenu_UpdateSaveEntry,       /* Save */
    DebugMenu_UpdateTradeEntry,      /* Trade */
    DebugMenu_EnterMappedMode,       /* Option */
    DebugMenu_EnterMappedMode,       /* LOSE */
    DebugMenu_EnterMappedMode,       /* HIRATA's */
    DebugMenu_Exit,                  /* EXIT */
};
