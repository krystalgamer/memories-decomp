#ifndef MEMORIES_DECOMP_MAIN_MENU_MODULE_STATE_H
#define MEMORIES_DECOMP_MAIN_MENU_MODULE_STATE_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/card_constants.h"

#define MAIN_MENU_ENTRY_COUNT 11
#define MAIN_MENU_TRADE_SIDE_COUNT 2
#define MAIN_MENU_TRADE_OFFER_CARD_COUNT 10

typedef struct {
    struct DisplayObject *background;
    struct DisplayObject *title;
    struct DisplayObject *prompt;
    u8 *unknown4564;
    u8 *entries[MAIN_MENU_ENTRY_COUNT];
    u8 menuId;
    u8 pendingAction;
    u8 transitionMode;
    u8 shade;
    s8 fadeDirection;
    u8 transitionRunning;
    u8 actionFlags[4];
    u8 loadedImageTail[2];
} MainMenuFrontendState;

typedef struct {
    struct DisplayObject *unknown45A0;
    struct DisplayObject *unknown45A4;
    struct DisplayObject *unknown45A8;
    struct DisplayObject *unknown45AC;
    struct DisplayObject *widgets[3];
    u8 selections[3];
    u8 padBF;
    ValueSetupEntry entries[2];
    u8 *toggle;
} MainMenuValueSetupState;

typedef struct {
    u8 *object;
    s32 unknown;
} MainMenuTradeSlot;

typedef struct {
    u16 current;
    u16 target;
} MainMenuTradeScroll;

typedef struct {
    u8 *display45DC;
    u8 *display45E0;
    u8 *unknown45E4;
    u8 *unknown45E8;
    MainMenuTradeSlot displaySlots[MAIN_MENU_TRADE_SIDE_COUNT];
    CardCountEntry inventory[MAIN_MENU_TRADE_SIDE_COUNT][CARD_COUNT];
    MainMenuTradeScroll scroll[4];
    u16 offers[MAIN_MENU_TRADE_SIDE_COUNT]
              [MAIN_MENU_TRADE_OFFER_CARD_COUNT + 1];
    u8 inventoryReady[MAIN_MENU_TRADE_SIDE_COUNT];
    u8 listMode[MAIN_MENU_TRADE_SIDE_COUNT];
    u8 sortMode[MAIN_MENU_TRADE_SIDE_COUNT];
    u8 focusedPane;
    u8 pending[3];
    u8 padCD2[2];
} MainMenuTradeState;

extern MainMenuFrontendState gMainMenu_FrontendState;
extern MainMenuValueSetupState gMainMenu_ValueSetupState;
extern MainMenuTradeState gMainMenu_TradeState;

#define MAIN_MENU_STATE_OFFSET(type, member) ((u32)&(((type *)0)->member))

typedef char MainMenuFrontendState_size_must_be_0x48[
    sizeof(MainMenuFrontendState) == 0x48 ? 1 : -1
];
typedef char MainMenuFrontendState_entries_offset_must_be_0x10[
    MAIN_MENU_STATE_OFFSET(MainMenuFrontendState, entries) == 0x10 ? 1 : -1
];
typedef char MainMenuFrontendState_menu_id_offset_must_be_0x3C[
    MAIN_MENU_STATE_OFFSET(MainMenuFrontendState, menuId) == 0x3C ? 1 : -1
];
typedef char MainMenuFrontendState_tail_offset_must_be_0x46[
    MAIN_MENU_STATE_OFFSET(MainMenuFrontendState, loadedImageTail) == 0x46 ? 1 : -1
];
typedef char MainMenuValueSetupState_size_must_be_0x3C[
    sizeof(MainMenuValueSetupState) == 0x3C ? 1 : -1
];
typedef char MainMenuValueSetupState_widgets_offset_must_be_0x10[
    MAIN_MENU_STATE_OFFSET(MainMenuValueSetupState, widgets) == 0x10 ? 1 : -1
];
typedef char MainMenuValueSetupState_entries_offset_must_be_0x20[
    MAIN_MENU_STATE_OFFSET(MainMenuValueSetupState, entries) == 0x20 ? 1 : -1
];
typedef char MainMenuValueSetupState_toggle_offset_must_be_0x38[
    MAIN_MENU_STATE_OFFSET(MainMenuValueSetupState, toggle) == 0x38 ? 1 : -1
];
typedef char MainMenuTradeSlot_size_must_be_8[
    sizeof(MainMenuTradeSlot) == 8 ? 1 : -1
];
typedef char MainMenuTradeScroll_size_must_be_4[
    sizeof(MainMenuTradeScroll) == 4 ? 1 : -1
];
typedef char MainMenuTradeState_size_must_be_0x16F8[
    sizeof(MainMenuTradeState) == 0x16F8 ? 1 : -1
];
typedef char MainMenuTradeState_slots_offset_must_be_0x10[
    MAIN_MENU_STATE_OFFSET(MainMenuTradeState, displaySlots) == 0x10 ? 1 : -1
];
typedef char MainMenuTradeState_inventory_offset_must_be_0x20[
    MAIN_MENU_STATE_OFFSET(MainMenuTradeState, inventory) == 0x20 ? 1 : -1
];
typedef char MainMenuTradeState_scroll_offset_must_be_0x16B0[
    MAIN_MENU_STATE_OFFSET(MainMenuTradeState, scroll) == 0x16B0 ? 1 : -1
];
typedef char MainMenuTradeState_offers_offset_must_be_0x16C0[
    MAIN_MENU_STATE_OFFSET(MainMenuTradeState, offers) == 0x16C0 ? 1 : -1
];
typedef char MainMenuTradeState_flags_offset_must_be_0x16EC[
    MAIN_MENU_STATE_OFFSET(MainMenuTradeState, inventoryReady) == 0x16EC ? 1 : -1
];

#undef MAIN_MENU_STATE_OFFSET

#endif
