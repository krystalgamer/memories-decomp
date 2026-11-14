#include "../../types.h"
#include "module_state.h"

/* Main-menu screen state at 0x80184558-0x80185CD4, defined in address order.
 * Each name is its own variable; the D_ entries cover bytes no consumer has
 * been seen to name. */
struct DisplayObject *gMainMenu_pTitleLogo = 0;
struct DisplayObject *gMainMenu_pCopyrightNotice = 0;
struct DisplayObject *gMainMenu_pStartPrompt = 0;
u8 *D_80184564 = 0;
u8 *gMain_apMenuEntries[11] = {0};
u8 gMain_bMenuID = 0;
u8 gMainMenu_bBackRequested = 0;
u8 gMainMenu_bTransitionMode = 0;
u8 gMainMenu_bShadeLevel = 0;
s8 gMainMenu_nFadeDirection = 0;
u8 gMainMenu_bTransitionActive = 0;
u8 gMainMenu_bSaveWritePending = 0;
u8 gMainMenu_bLoadPending = 0;
u8 gMainMenu_bTradeLoadPending = 0;
u8 gMainMenu_bDuelLoadPending = 0;
u8 D_8018459E = 4;
u8 D_8018459F = 0x24;

DisplayObject *gTwoPlayerDuel_pBackground = 0;
DisplayObject *gTwoPlayerDuel_pSetupPanel = 0;
DisplayObject *D_801845A8 = 0;
DisplayObject *D_801845AC = 0;
DisplayObject *gTwoPlayerDuel_apSetupCursors[3] = {0};
u8 gTwoPlayerDuel_abSetupChoices[3] = {0};
u8 D_801845BF = 0;
ValueSetupEntry gTwoPlayerDuel_aSetupEntries[2] = {{0}};
u8 *gTwoPlayerDuel_pSetupToggle = 0;

MainMenuWidget *gTrade_pBackground = 0;
MainMenuWidget *gTrade_pStatsModeDisplay = 0;
u8 *D_801845E4 = 0;
u8 *D_801845E8 = 0;
MainMenuSlot gTrade_aCardSlots[2] = {{0}};
CardCountEntry gTrade_aWorkingCards[2][CARD_COUNT] = {{{0}}};
MainMenuPair gTrade_aListScroll[4] = {{0}};
u16 gTrade_awOffers[2][11] = {{0}};
u8 gTrade_abReady[2] = {0};
/* gTrade_abCursorRow[2]. GCC word-aligns byte arrays, so the linker binds the
 * name here (main_menu_linker_symbols.txt). */
u8 D_80185CCA = 0;
u8 D_80185CCB = 0;
u8 gTrade_abSortMode[2] = {0};
u8 gTrade_bFocusPane = 0;
u8 gTrade_bDecisionMenuActive = 0;
u8 gTrade_bCommitRequested = 0;
u8 gTrade_bWritePending = 0;
u16 D_80185CD2 = 0;
