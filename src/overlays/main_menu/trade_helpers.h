#ifndef MEMORIES_DECOMP_MAIN_MENU_TRADE_HELPERS_H
#define MEMORIES_DECOMP_MAIN_MENU_TRADE_HELPERS_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/card_constants.h"
#include "module_rodata.h"

/* One entry of gTrade_aCardSlots, the two Trade card-display slots.
   MainMenu_InitTradeScreen's `i < 2` loop stores the DisplayObject_AcquireSlot result,
   or 0, into `.object` and 0 into `.unk4` (trade_update.c:54-65);
   MainMenu_DrawTradeOffersAndHighlights reads `[0].object` and
   `[1].object` (trade_offers.c:37-38); MainMenu_UpdateTradeScreen
   (trade_update.c) stores `->y` through `[0].object` and `[1].object`.
   The two readers used to declare the symbol `u8 *[]`, read at [0] and
   [2], and `MainMenuWidget *`, with entry 1's pointer declared on its own
   as D_801845F4 (+8); the 8-byte stride reconciled those views, and spelled
   through this type they build the same overlay (measured, one build). */
typedef struct {
    u8 *G32 object;
    s32 unk4;
} MainMenuSlot;

typedef char MainMenuSlot_size_must_be_8[
    sizeof(MainMenuSlot) == 8 ? 1 : -1
];

extern MainMenuSlot gTrade_aCardSlots[];

/* One side's Trade list scroll position, gTrade_aListScroll[side]. The README records
   the meaning: "[side][0] is the current scrolling top; [1] is its target".

   MainMenu_UpdateTradeScreen and rebuild_trade_inventory_rows.c declare the same storage
   as `u16 [2]` and are the only sources that read it, so the members are u16
   here; MainMenu_InitTradeScreen only ever stores zero, and
   a halfword store is the same instruction either way. The read sites clamp
   the target at zero, so the value is never negative. */
typedef struct {
    u16 current;
    u16 target;
} MainMenuPair;

#ifdef MAIN_MENU_TRADE_SCROLL_AS_WORDS
extern u16 gTrade_awListScrollWords[2][2] asm("gTrade_aListScroll");
#define gTrade_aListScroll gTrade_awListScrollWords
#else
extern MainMenuPair gTrade_aListScroll[];
#endif

/* One entry of D_801A8000, the per-side inventory row state. Only the leading
   display-object pointer is named; the rest is carried so the stride is
   right. */
typedef struct {
    u8 *G32 object;
#ifdef VERSION_EUROPE
    s32 pad[6];
#else
    s32 pad[5];
#endif
} MainMenuState;

/* These values are live on disjoint paths. Sharing their carrier reproduces
   the retail s2 allocation without a hard-register binding. */
typedef union {
    s32 dirty;
    u8 *G32 clearBase;
} MainMenuTradeDirtyCarrier;

/* The Trade updater and inventory updater's two-record view of the shared
   overlay staging base. */
#ifdef MAIN_MENU_TRADE_STATE_BUFFER
extern MainMenuState D_801A8000[];
extern MainMenuState gTrade_aInventoryRowSets[] __asm__("D_801A8000");
#endif

/* The main-menu view of a display-object pool record. Named for this overlay
   rather than shared with the resident DisplayObject: `y` at 0x32 falls
   inside that record's s32 at 0x30, which is the split display_object.h
   documents as the reason its own callers keep private copies. */
typedef struct {
    u8 pad0[8];
    u16 flags;
    u8 pad0A[0x28];
    s16 y;
    u8 pad34[0x35];
    u8 frame;
} MainMenuWidget;

/* The two Trade display handles, D_801845DC and D_801845E0 -- the pair
 * README.md:177-178 lists as what MainMenu_ReleaseTradeDisplayHandles
 * releases and clears. MainMenu_InitTradeScreen stores a DisplayObject_AcquireSlot
 * result into each (trade_update.c:37, :45), ORs 0x28 into +8
 * (:40, :48) and passes it to DisplayObject_SetDepthOffset (:41, :49);
 * MainMenu_ReleaseTradeDisplayHandles passes each to DisplayObject_ReleaseIfPresent and
 * stores 0 (trade_offers.c:141-144); MainMenu_UpdateTradeScreen
 * (trade_update.c) reads D_801845E0->frame and passes D_801845E0 to
 * DisplayObject_SetResourceVariant; MainMenu_RebuildTradeInventoryRows reads ->frame
 * (trade_screen_helpers.c:101). The units used to declare them `u8 *` and
 * `void *`, and D_801845E0 also `MainMenuWidget *` in the two units that
 * read `frame`. `frame` is a member of that view, and so is the +8 halfword
 * those two stores OR into, named `flags` as DisplayObjectConfig
 * (display_object_config.h) names it, so the widget view is the one kept
 * here; only the DisplayObject_SetDepthOffset sites cast to bytes. */
extern MainMenuWidget *G32 D_801845DC;
extern MainMenuWidget *G32 D_801845E0;
/* Resource variant minus four carries the two players' stats-mode bits. */
extern MainMenuWidget *G32 gTrade_pStatsModeDisplay __asm__("D_801845E0");

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
 * Both ready bytes gate the final menu. gTrade_bSide1Ready is the second byte
 * of gTrade_abReady, with its own symbol for direct accesses; array clears also
 * clear it. None of these declarations allocate separate state storage.
 *
 * gTrade_bDecisionMenuActive gates commit/resume/exit selection.
 * gTrade_bCommitRequested starts offer application and the save request.
 * gTrade_bWritePending gates polling and handling the asynchronous result.
 *
 * gTrade_bSide1CursorRow remains the separate second-byte cursor-row view. */
extern u8 gTrade_bSide1Ready;
extern u8 gTrade_bSide1CursorRow;
extern u8 gTrade_bFocusPane;
extern u8 gTrade_bDecisionMenuActive;
extern u8 gTrade_bCommitRequested;
extern u8 gTrade_bWritePending;

/* Per-side Trade interaction state. The offer table carries a count followed
 * by ten card ids. gTrade_abReady needs a scalar view only in the offer drawing
 * unit; the update paths index both players. */
extern u16 gTrade_awOffers[2][11];
#ifdef MAIN_MENU_TRADE_READY_AS_SCALAR
extern u8 gTrade_abReady;
#else
extern u8 gTrade_abReady[2];
#endif
extern u8 gTrade_abCursorRow[2];
extern u8 gTrade_abSortMode[2];

/* The per-side working card table, two rows of CARD_COUNT CardCountEntry
 * (ygo_types.h) records. Three sources reach it and all three already include
 * this header. The two-dimensional shape is the one that matched
 * MainMenu_RefreshTradeInventory (trade_inventory.c stores
 * `[slot][i].id` and `.count`, then sorts `[slot]`; the module's functions.csv
 * row for 0x8018338C records why), and the other two sources reach the same
 * rows through it: MainMenu_AdjustTradeCardCount walks row 0 from
 * `gTrade_aWorkingCards[0]` with `slot * 2888` added (trade_offers.c:169-172, :180),
 * MainMenu_RebuildTradeInventoryRows forms `side * 2888 + (s32)gTrade_aWorkingCards`
 * (trade_screen_helpers.c:102), and MainMenu_UpdateTradeScreen
 * (trade_update.c) indexes `[0][...]`. Row 1 is also named on its own as
 * gTrade_aSide1WorkingCards in that source (+0xB48 = CARD_COUNT * 4), which keeps its
 * private declaration.
 * Two rows end at gTrade_aListScroll, +0x1690. */
extern CardCountEntry gTrade_aWorkingCards[][CARD_COUNT];

/* Preserve frozen Trade C spellings; these are views of existing storage. */
#define D_80185CC8 gTrade_abReady
#define D_80185CC9 gTrade_bSide1Ready
#define D_80185CCF gTrade_bDecisionMenuActive
#define D_80185CD0 gTrade_bCommitRequested
#define D_80185CD1 gTrade_bWritePending

#endif
