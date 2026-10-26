#ifndef MEMORIES_DECOMP_MAIN_MENU_FRONTEND_H
#define MEMORIES_DECOMP_MAIN_MENU_FRONTEND_H

#include "../../types.h"
#include "display_resources.h"

struct DisplayObject;

typedef void (*MainMenuEntryEffectUpdate)(u8 *object);

/* Frontend screen state, all of it owned by the front-end functions in
 * frontend.c. The state symbols
 * themselves are not defined in C.
 *
 * What the uses show:
 *
 *   gMain_bMenuID          The selected entry, assigned as `menu % 11`, so
 *                          the wheel has eleven positions.
 *   gMain_apMenuEntries    One object pointer per entry, cleared to 0 for
 *                          the slots that are not live.
 *   D_80184558             The three standing objects: background, title and
 *   D_8018455C             prompt. Made by MainMenu_InitFrontendMenu and
 *   D_80184560             released by MainMenu_DestroyFrontendMenu.
 *   gMainMenu_bTransitionMode
 *                          The transition mode, taken straight from
 *                          MainMenu_StartFrontendEntryTransition's argument.
 *   gMainMenu_nFadeDirection
 *                          The fade direction, and the reason it is signed:
 *                          MainMenu_UpdateFrontendMenu assigns it 1 and -1,
 *                          tests `< 0` to decide which end of the fade it is
 *                          at, and shifts it left three into
 *                          gMainMenu_bShadeLevel as a step. See below.
 *   gMainMenu_bTransitionActive
 *                          Set to 1 when that transition starts, and tested
 *                          as the "transition running" flag.
 *   gMainMenu_bShadeLevel  A shade level, only ever 0 or 0x80.
 *   gMainMenu_bBackRequested
 *                          Raised when Cancel is pressed inside a submenu
 *                          (menu id 5 or above). Once the transition
 *                          settles the update tests it: with a menu id
 *                          below 5 it hides all entries and sets the fade
 *                          direction to -1; otherwise it starts the entry
 *                          transition and sets menu id 1. Both branches
 *                          then clear the flag. Init clears it too.
 *   gMainMenu_bSaveWritePending, gMainMenu_bLoadPending,
 *   gMainMenu_bTradeLoadPending, gMainMenu_bDuelLoadPending
 *                          Pending-request flags with the same shape: the
 *                          confirm handler raises one after starting a save
 *                          write (entry 10), load (1), Trade load (3) or
 *                          two-player duel load (2), and the updater polls
 *                          MemCardDialog_Poll, SaveData_PollLoad,
 *                          SaveData_UpdateTradeLoad or SaveData_UpdateDuelLoad
 *                          until it finishes, then clears it.
 *
 * gMainMenu_nFadeDirection used to sit outside this list because frontend_init.c spelled it
 * u8 and frontend_update.c spelled it s8, and a declaration its two declarers
 * disagree about spreads the wrong one. Coalescing those two sources into
 * frontend.c forced the question and the evidence settles it: the only writes
 * are 0, 1 and -1, and the only read that cares about sign is `< 0`. The u8
 * spelling was harmless rather than right, because the file that used it only
 * ever wrote 0.
 *
 * D_80184558/5C/60 are the three singleton display objects allocated by
 * MainMenu_InitFrontendMenu. Their matching and candidate consumers use the
 * shared DisplayObject fields rather than deriving the layout again.
 */
extern struct DisplayObject *G32 D_80184558;
extern struct DisplayObject *G32 D_8018455C;
extern struct DisplayObject *G32 D_80184560;
/* The blinking singleton gates Start input before the menu wheel. */
extern struct DisplayObject *G32 gMainMenu_pStartPrompt __asm__("D_80184560");
extern s8 gMainMenu_nFadeDirection;
extern u8 gMain_bMenuID;
extern u8 *G32 gMain_apMenuEntries[];
extern u8 gMainMenu_bBackRequested;
extern u8 gMainMenu_bTransitionMode;
extern u8 gMainMenu_bShadeLevel;
extern u8 gMainMenu_bTransitionActive;
extern u8 gMainMenu_bSaveWritePending;
extern u8 gMainMenu_bLoadPending;
extern u8 gMainMenu_bTradeLoadPending;
extern u8 gMainMenu_bDuelLoadPending;

/* Resident callers must load the main-menu image before using these entries. */
void MainMenu_InitFrontendMenu(s32 unused, s32 menu);
s32 MainMenu_UpdateFrontendMenu(void);
void MainMenu_DestroyFrontendMenu(void);
void MainMenu_StartFrontendEntryTransition(s32 mode);
void MainMenu_DrawFrontendBackground(void);
void MainMenu_SpawnFrontendEntryAfterimage(struct DisplayObject *entry);
void MainMenu_UpdateFrontendEntryAfterimage(struct DisplayObject *object);

#endif
