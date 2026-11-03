#ifndef MEMORIES_DECOMP_MAIN_SERVICES_H
#define MEMORIES_DECOMP_MAIN_SERVICES_H

#include "../types.h"
#include "../psyq/setjmp.h"

/* The per-frame callback registry the resident service pump walks.
 *
 * There are four slots, not one. Main_RunFrameServices calls every non-null
 * slot in order once per frame, and Main_ClearFrameServiceCallbacks clears
 * all four. The size is not a guess: c_symbols.ld places gMain_apFrameServiceCallbacks at
 * 0x800E9DB0 and the next symbol, gMain_RecoveryJmpBuf, sixteen bytes later, which is
 * exactly four pointers.
 *
 * The main_menu overlay previously declared slot 0 as a bare
 * `void (*)(void)` -- and, where it only cleared the slot, as an `s32`. Both
 * spellings write the right address, because slot 0 sits at
 * the base, but they hide that three more slots follow and that something else
 * may own them. Reaching slot 0 by index says what the code is really doing.
 *
 * This is a registry of independent slots, NOT a chain: no slot is called with
 * arguments, none returns a value, and nothing enforces an order beyond the
 * index. Slot 3 is stored by Duel_InitScene (src/game/duel_init_scene.c:155,
 * `gMain_apFrameServiceCallbacks[3] = Duel_DrawFieldCards;`; Duel_InitScene.s:207-208), Library_InitializeScreen
 * (library_runtime.c, `gMain_apFrameServiceCallbacks[3] = Library_DrawCardGrid;`) and
 * CampaignMap_SetLocation
 * (src/overlays/overworld/set_location.c:56,
 * `gMain_apFrameServiceCallbacks[3] = CampaignMap_UpdateView;`), and by Library_UpdateCardView
 * (src/game/library_runtime.c), which clears it when the card view opens and
 * restores Library_DrawCardGrid when it closes.
 * Slot 2 is stored only by Library_UpdateCardView, which installs Library_DrawWireframeGlobe for
 * the model view and clears it on the way out; no other listing or C unit
 * names it.
 * Slot 1 is stored by the main_menu overlay, by index:
 * MainMenu_InitTradeScreen (src/overlays/main_menu/trade_update.c,
 * `gMain_apFrameServiceCallbacks[1] = MainMenu_DrawTradeOffersAndHighlights;`) and
 * MainMenu_ReleaseTradeDisplayHandles
 * (src/overlays/main_menu/trade_offers.c:145, `gMain_apFrameServiceCallbacks[1] = 0;`). Before
 * that the overlay reached it as D_800E9DB4, with a private declaration in
 * each unit. Every slot is reached as an element of
 * gMain_apFrameServiceCallbacks; no slot has a symbol of its own. */
extern void (*gMain_apFrameServiceCallbacks[4])(void);

/* The recovery point the registry's comment above already places at
 * 0x800E9DC0. Main_Init arms it with setjmp once the boot sequence is up,
 * and two functions jump back into it: Main_RunGameOver passes 1 from the
 * game-over path and DebugMenu_Exit passes 2, so the value distinguishes
 * which unwound. All three spell
 * it jmp_buf and include psyq/setjmp.h, which this header now includes so
 * the declaration stands on its own. */
extern jmp_buf gMain_RecoveryJmpBuf;

/* The single extra callback the pump runs after the four slots, and that
 * Main_ClearFrameServiceCallbacks clears alongside them. */
extern void (*gMain_pfnExtraFrameService)(void);

/* The signed per-frame watchdog owned and initialized by main_services.c. */
extern s32 gMain_nServiceResyncCountdown;

/* The pending frontend-menu request. Main_RunMenu hands the pair to the
   main_menu overlay as MainMenu_InitFrontendMenu(D_8009B268, gMain_bRequestedMenuID),
   whose parameters are (unused, menu): gMain_bRequestedMenuID
   (main_menu_request.h) is the menu id --
   Main_ApplyMenuSelection stores its selection argument, Script_OpSavePrompt and
   Script_OpReturnToMenu
   store 5, DebugMenu_UpdateTitleEntry round-trips it through gDebug_nSceneOrSoundID,
   Main_RunGameOver stores 0 -- and D_8009B268 is stored 1 beside every
   request and 0 in three of Main_ApplyMenuSelection's arms. Both are bytes, read lbu.
   Main_ApplyMenuSelection and Main_RunMenu (main_mode_runners.c) reach them through $gp;
   script_op_save_prompt.c, script_command_runner.c and frontend_scene_states.c
   address them with %hi/%lo, outside small data, and define the .data arms. */
#ifdef D_8009B268_IN_DATA
extern u8 D_8009B268 __attribute__((section(".data")));
#else
extern u8 D_8009B268;
#endif

#include "main_menu_request.h"

struct GraphicsFrameBuffer;
void Main_InitHardware(struct GraphicsFrameBuffer *base);
void Graphics_AdjustScreenOffset(void);

/* The pump itself, and the call that empties the registry. Main_RunFrameServices
   walks the four slots once per frame; Main_ClearFrameServiceCallbacks clears
   all four. Both callers already spelled the pump this way. */
void Main_RunFrameServices(void);
void Main_ClearFrameServiceCallbacks(void);

#endif
