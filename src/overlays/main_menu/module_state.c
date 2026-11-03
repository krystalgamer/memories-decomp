#include "../../types.h"
#include "module_state.h"

/* Main-menu screen state at 0x80184558-0x80185CD4, defined in address order.
 * Each name is its own variable; the D_ entries cover bytes no consumer has
 * been seen to name. */
#define MAIN_MENU_STATE __attribute__((section(".data")))

struct DisplayObject *D_80184558 MAIN_MENU_STATE = 0;
struct DisplayObject *D_8018455C MAIN_MENU_STATE = 0;
struct DisplayObject *gMainMenu_pStartPrompt MAIN_MENU_STATE = 0;
u8 *D_80184564 MAIN_MENU_STATE = 0;
u8 *gMain_apMenuEntries[11] MAIN_MENU_STATE = {0};
u8 gMain_bMenuID MAIN_MENU_STATE = 0;
u8 gMainMenu_bBackRequested MAIN_MENU_STATE = 0;
u8 gMainMenu_bTransitionMode MAIN_MENU_STATE = 0;
u8 gMainMenu_bShadeLevel MAIN_MENU_STATE = 0;
s8 gMainMenu_nFadeDirection MAIN_MENU_STATE = 0;
u8 gMainMenu_bTransitionActive MAIN_MENU_STATE = 0;
u8 gMainMenu_bSaveWritePending MAIN_MENU_STATE = 0;
u8 gMainMenu_bLoadPending MAIN_MENU_STATE = 0;
u8 gMainMenu_bTradeLoadPending MAIN_MENU_STATE = 0;
u8 gMainMenu_bDuelLoadPending MAIN_MENU_STATE = 0;
u8 D_8018459E MAIN_MENU_STATE = 4;
u8 D_8018459F MAIN_MENU_STATE = 0x24;

DisplayObject *D_801845A0 MAIN_MENU_STATE = 0;
DisplayObject *D_801845A4 MAIN_MENU_STATE = 0;
DisplayObject *D_801845A8 MAIN_MENU_STATE = 0;
DisplayObject *D_801845AC MAIN_MENU_STATE = 0;
DisplayObject *gMainMenu_apValueSetupCursors[3] MAIN_MENU_STATE = {0};
u8 gMainMenu_abValueSetupChoices[3] MAIN_MENU_STATE = {0};
u8 D_801845BF MAIN_MENU_STATE = 0;
ValueSetupEntry gMainMenu_aValueSetupEntries[2] MAIN_MENU_STATE = {{0}};
u8 *gMainMenu_pValueSetupToggle MAIN_MENU_STATE = 0;

MainMenuWidget *D_801845DC MAIN_MENU_STATE = 0;
MainMenuWidget *gTrade_pStatsModeDisplay MAIN_MENU_STATE = 0;
u8 *D_801845E4 MAIN_MENU_STATE = 0;
u8 *D_801845E8 MAIN_MENU_STATE = 0;
MainMenuSlot gTrade_aCardSlots[2] MAIN_MENU_STATE = {{0}};
CardCountEntry gTrade_aWorkingCards[2][CARD_COUNT] MAIN_MENU_STATE = {{{0}}};
MainMenuPair gTrade_aListScroll[4] MAIN_MENU_STATE = {{0}};
u16 gTrade_awOffers[2][11] MAIN_MENU_STATE = {{0}};
u8 gTrade_abReady[2] MAIN_MENU_STATE = {0};
/* gTrade_abCursorRow[2]. GCC word-aligns byte arrays, so the linker binds the
 * name here (main_menu_linker_symbols.txt). */
u8 D_80185CCA MAIN_MENU_STATE = 0;
u8 D_80185CCB MAIN_MENU_STATE = 0;
u8 gTrade_abSortMode[2] MAIN_MENU_STATE = {0};
u8 gTrade_bFocusPane MAIN_MENU_STATE = 0;
u8 gTrade_bDecisionMenuActive MAIN_MENU_STATE = 0;
u8 gTrade_bCommitRequested MAIN_MENU_STATE = 0;
u8 gTrade_bWritePending MAIN_MENU_STATE = 0;
u16 D_80185CD2 MAIN_MENU_STATE = 0;

#undef MAIN_MENU_STATE
