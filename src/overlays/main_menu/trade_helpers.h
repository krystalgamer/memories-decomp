#ifndef MEMORIES_DECOMP_MAIN_MENU_TRADE_HELPERS_H
#define MEMORIES_DECOMP_MAIN_MENU_TRADE_HELPERS_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/card_constants.h"

/* One entry of D_801845EC, the two Trade card-display slots.

   Three code paths view this symbol three different ways and all three agree;
   they are views of one array, not competing claims about it.

     MainMenu_InitTradeScreen
                     MainMenuSlot D_801845EC[]  - builds both entries, writing
                     .object from func_800400AC and clearing .unk4
     trade_draw.c    u8 *D_801845EC[]           - reads [0] and [2]
     MainMenu_UpdateTradeScreen
                     MainMenuWidget *D_801845EC - reads ->y

   The 8-byte stride is what reconciles them: trade_draw.c's [0] and [2] are
   the `object` pointers of entries 0 and 1 at a 4-byte pointer stride, and
   the updater's declaration names the same first pointer, so `->y` is
   entry 0's object. Only the initializer's view sees `unk4` at all. */
typedef struct {
    u8 *object;
    s32 unk4;
} MainMenuSlot;

typedef char MainMenuSlot_size_must_be_8[
    sizeof(MainMenuSlot) == 8 ? 1 : -1
];

/* One side's Trade list scroll position, D_80185C8C[side]. The README records
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

/* The six card comparators the inventory sort chooses between, copied out of
   D_80180000[1] as one block. */
typedef struct {
    int (*entries[6])();
} MainMenuComparators;

/* One entry of D_801A8000, the per-side inventory row state. Only the leading
   display-object pointer is named; the rest is carried so the stride is
   right. */
typedef struct {
    u8 *object;
    s32 pad[5];
} MainMenuState;

/* The main-menu view of a display-object pool record. Named for this overlay
   rather than shared with the resident DisplayObject: `y` at 0x32 falls
   inside that record's s32 at 0x30, which is the split display_object.h
   documents as the reason its own callers keep private copies. */
typedef struct {
    u8 pad0[0x32];
    s16 y;
    u8 pad34[0x35];
    u8 frame;
} MainMenuWidget;

void MainMenu_RefreshTradeInventory(s32 slot, s32 force);
void MainMenu_DrawTradeOffersAndHighlights(void);
void MainMenu_DrawThreeDigitNumber(s32 x, s32 y, s32 value);
void MainMenu_DrawCardTypeIcon(s32 x, s32 y, s32 cardID);
void MainMenu_ApplyTradeOfferInventoryDelta(s32 slot, s32 amount);
void MainMenu_AdjustTradeCardCount(s32 slot, s32 id, u32 amount);
void MainMenu_DrawTradeColumnOverlay(s32 column);
void MainMenu_RebuildTradeInventoryRows(s32 side);

/* Trade screen state. All three sources that use these already include this
 * header, so the ten local copies they carried existed only because the
 * declarations were missing here. Every declarer already spelled them u8.
 *
 *   D_80185CCE  The focused pane, stepped as `(D_80185CCE + 2) % 3` and
 *               `(D_80185CCE + 4) % 3`, so it has three positions. One site
 *               reads it as `*(volatile u8 *)&D_80185CCE`; that volatile is
 *               applied at the use, not by the declaration, so moving the
 *               declaration here leaves it in place.
 *   D_80185CC9  Set to 1 and tested; cleared nowhere in this overlay.
 *   D_80185CCF  Three flags, each zeroed on entry, set while their part of
 *   D_80185CD0  the screen is pending, tested, and cleared again.
 *   D_80185CD1
 */
extern u8 D_80185CC9;
extern u8 D_80185CCE;
extern u8 D_80185CCF;
extern u8 D_80185CD0;
extern u8 D_80185CD1;

/* The per-side working card table, two rows of CARD_COUNT CardCountEntry
 * (ygo_types.h) records. Three sources reach it and all three already include
 * this header. The two-dimensional shape is the one that matched
 * MainMenu_RefreshTradeInventory (trade_update.c stores
 * `[slot][i].id` and `.count`, then sorts `[slot]`; the module's functions.csv
 * row for 0x8018338C records why), and the other two sources reach the same
 * rows through it: MainMenu_AdjustTradeCardCount walks row 0 from
 * `D_801845FC[0]` with `slot * 2888` added (trade_offers.c:173-176, :184),
 * MainMenu_RebuildTradeInventoryRows forms `side * 2888 + (s32)D_801845FC`
 * (trade_screen_helpers.c:103), and MainMenu_UpdateTradeScreen indexes
 * `[0][...]` (trade_update.c:217). Row 1 is also named on its own as
 * D_80185144 (trade_update.c:23, :358; +0xB48 = CARD_COUNT * 4), which keeps
 * its private declaration. Two rows end at D_80185C8C, +0x1690. */
extern CardCountEntry D_801845FC[][CARD_COUNT];

#endif
