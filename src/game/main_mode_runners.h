#ifndef MEMORIES_DECOMP_MAIN_MODE_RUNNERS_H
#define MEMORIES_DECOMP_MAIN_MODE_RUNNERS_H
#include "../types.h"
#include "../ygo_types.h"

void Main_ResetFrontendRuntime(void);

/* The frontend loop's phase byte, and the gate on the reset above.
 *
 * Bit 0 chooses the phase: clear runs the attract path, set runs the menu.
 * Bit 0x80 is the "already reset for this phase" latch -- both arms of
 * Main_RunFrontendLoop open with `if ((f & 0x80) == 0)`, set the bit, and call
 * Main_ResetFrontendRuntime immediately, so the reset happens once per phase.
 * Main_RunBootSequence clears the whole byte when it sets the frontend up.
 *
 * Main_RunFrontendLoop also tests bit 0x40, and nothing sets it: notes/global-usage
 * lists Main_RunBootSequence and Main_RunFrontendLoop as the only accessors of this
 * address
 * in the image, assembly included, and between them they only ever store 0, 1
 * or `f | 0x80`. That arm is recorded here as an observation, not a claim
 * about intent. */
extern u8 gMain_bFrontendPhase;

extern u8 gMain_bRequestedMenuID;

extern u8 gMain_bReturnMode;

extern u8 gMain_bActiveModeState;

#define CREDITS_SECRET_NUMBER_COUNT 5

/* The five eight-digit secret numbers the credits display, each stored as a
   high/low four-digit pair. Main_RunCredits picks one by the save's game id
   modulo five. src/game/main_mode_runners.c owns them at 0x80090B50. */
extern u16 gCredits_awSecretNumbers[CREDITS_SECRET_NUMBER_COUNT][2];

/* Resets the frontend runtime once, before Main_Loop begins dispatching
 * modes. It is not itself a mode runner: main_modes.h owns those, including
 * Main_RunDebugMenu. */
void Main_PrepareFrontendLoop(void);

void Main_Loop(void);

typedef enum {
    MAIN_MENU_SELECTION_NEW_GAME = 0,
    MAIN_MENU_SELECTION_LOAD = 1,
    MAIN_MENU_SELECTION_TWO_PLAYER_DUEL = 2,
    MAIN_MENU_SELECTION_TRADE = 3,
    MAIN_MENU_SELECTION_OPTIONS = 4,
    MAIN_MENU_SELECTION_CAMPAIGN = 5,
    MAIN_MENU_SELECTION_FREE_DUEL = 6,
    MAIN_MENU_SELECTION_BUILD_DECK = 7,
    MAIN_MENU_SELECTION_LIBRARY = 8,
    MAIN_MENU_SELECTION_PASSWORD = 9,
    MAIN_MENU_SELECTION_SAVE = 10
} MainMenuSelection;

void Main_ApplyMenuSelection(MainMenuSelection selection);
#endif
