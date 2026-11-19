#ifndef MEMORIES_DECOMP_MAIN_MENU_TRADE_HELPERS_H
#define MEMORIES_DECOMP_MAIN_MENU_TRADE_HELPERS_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/card_constants.h"
#include "../../game/file_staging_buffer.h"
#include "module_rodata.h"
#include "../../game/display_object.h"

/* One entry of gTrade_aCardSlots, the two Trade card-display slots.
   MainMenu_InitTradeScreen's `i < 2` loop stores the DisplayObject_AcquireSlot result,
   or 0, into `.object` and 0 into `.unk4`;
   MainMenu_DrawTradeOffersAndHighlights reads `[0].object` and
   `[1].object`; MainMenu_UpdateTradeScreen stores `->xy.h.y` through `[0].object` and `[1].object`.
   The two readers used to declare the symbol `u8 *[]`, read at [0] and
   [2], and as a private widget view, with entry 1's pointer declared on its own
   as D_801845F4 (+8); the 8-byte stride reconciled those views, and spelled
   through this type they build the same overlay (measured, one build). */
typedef struct {
    DisplayObject *object;
    s32 unk4;
} MainMenuSlot;

typedef char MainMenuSlot_size_must_be_8[
    sizeof(MainMenuSlot) == 8 ? 1 : -1
];

extern MainMenuSlot gTrade_aCardSlots[];

/* One side's Trade list scroll position, gTrade_aListScroll[side]. The README records
   the meaning: "[side][0] is the current scrolling top; [1] is its target".

   MainMenu_UpdateTradeScreen and MainMenu_RebuildTradeInventoryRows declare the same storage
   as `u16 [2]` and are the only sources that read it, so the members are u16
   here; MainMenu_InitTradeScreen only ever stores zero, and
   a halfword store is the same instruction either way. The read sites clamp
   the target at zero, so the value is never negative. */
typedef struct {
    u16 current;
    u16 target;
} MainMenuPair;

extern MainMenuPair gTrade_aListScroll[];

/* These values are live on disjoint paths. Sharing their carrier reproduces
   the retail s2 allocation without a hard-register binding. */
typedef union {
    s32 dirty;
    u8 *clearBase;
} MainMenuTradeDirtyCarrier;


/* The two Trade display handles, gTrade_pBackground and gTrade_pStatsModeDisplay -- the pair
 * README.md:177-178 lists as what MainMenu_ReleaseTradeDisplayHandles
 * releases and clears. MainMenu_InitTradeScreen stores a DisplayObject_AcquireSlot
 * result into each, ORs 0x28 into +8 and passes it to
 * DisplayObject_SetDepthOffset;
 * MainMenu_ReleaseTradeDisplayHandles passes each to DisplayObject_ReleaseIfPresent and
 * stores 0; MainMenu_UpdateTradeScreen reads gTrade_pStatsModeDisplay->frame and passes it to
 * DisplayObject_SetResourceVariant; MainMenu_RebuildTradeInventoryRows reads ->frame.
 * The +8 halfword is DisplayObject.flags and the byte read as the stats
 * mode is DisplayObject.resource_variant (+0x69), the variant
 * DisplayObject_SetResourceVariant stores. */
extern DisplayObject *gTrade_pBackground;
/* Resource variant minus four carries the two players' stats-mode bits. */
extern DisplayObject *gTrade_pStatsModeDisplay;

/* Resident callers must load the main-menu image before using these entries. */
void MainMenu_InitTradeScreen(void);
s32 MainMenu_UpdateTradeScreen(void);
void MainMenu_ReleaseTradeDisplayHandles(void);
void MainMenu_RefreshTradeInventory(s32 slot, s32 force);
void MainMenu_DrawTradeOffersAndHighlights(void);
void MainMenu_DrawThreeDigitNumber(s32 x, s32 y, s32 value);
void MainMenu_DrawCardTypeIcon(s32 x, s32 y, s32 cardID);
void MainMenu_ApplyTradeOfferInventoryDelta(s32 slot, s32 amount);
void MainMenu_AdjustTradeCardCount(s32 slot, s32 id, u32 amount);
void MainMenu_DrawTradeColumnOverlay(s32 column);
void MainMenu_RebuildTradeInventoryRows(s32 side);

/* Trade confirmation state. gTrade_bFocusPane keeps its accepted name and
 * three-position decision selection; volatile reads remain at their use sites.
 * Both gTrade_abReady bytes gate the final menu.
 *
 * gTrade_bDecisionMenuActive gates commit/resume/exit selection.
 * gTrade_bCommitRequested starts offer application and the save request.
 * gTrade_bWritePending gates polling and handling the asynchronous result.
 */
extern u8 gTrade_bFocusPane;
extern u8 gTrade_bDecisionMenuActive;
extern u8 gTrade_bCommitRequested;
extern u8 gTrade_bWritePending;

/* Per-side Trade interaction state. The offer table carries a count followed
 * by ten card ids. */
extern u16 gTrade_awOffers[2][11];
extern u8 gTrade_abReady[2];
extern u8 gTrade_abCursorRow[2];
extern u8 gTrade_abSortMode[2];

/* The per-side working card table, two rows of CARD_COUNT CardCountEntry
 * (ygo_types.h) records. Four functions in trade_screen.c reach it through
 * this header. The two-dimensional shape is the one that matched
 * MainMenu_RefreshTradeInventory (it stores
 * `[slot][i].id` and `.count`, then sorts `[slot]`; the module's functions.csv
 * row for 0x8018338C records why), and the other three reach the same
 * rows through it: MainMenu_AdjustTradeCardCount walks row 0 from
 * `gTrade_aWorkingCards[0]` with `slot * 2888` added,
 * MainMenu_RebuildTradeInventoryRows forms `side * 2888 + (s32)gTrade_aWorkingCards`,
 * and MainMenu_UpdateTradeScreen indexes `[0][...]` and `[1][...]` (row 1, +0xB48 =
 * CARD_COUNT * 4, once a separate label, gTrade_aSide1WorkingCards).
 * Two rows end at gTrade_aListScroll, +0x1690. */
extern CardCountEntry gTrade_aWorkingCards[][CARD_COUNT];

#endif
