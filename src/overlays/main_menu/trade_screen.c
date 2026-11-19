#include "../../types.h"
#include "../../game/main.h"
#include "../../game/display_object_system.h"
#include "../../game/main_services.h"
#include "../../game/mem_card.h"
#include "../../game/save_pair_workspace.h"
#include "../../ygo_types.h"
#include "../../psyq/qsort.h"
#include "../../game/card_constants.h"
#include "trade_helpers.h"
#include "../../game/duel_card_viewer.h"
#include "../../game/duel_effect_states.h"
#include "../../game/sound.h"
#include "../../game/input.h"
#include "../../game/duel_effect_text.h"
#include "../../game/trade_screen_widgets.h"
#include "../../psyq/rand.h"
#include "card_comparators.h"
#include "card_tables.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/display_object.h"
#include "ordering_tables.h"
#include "../../game/gpu_packets.h"
#include "../../game/file_staging_buffer.h"

/* The Trade screen, 0x80181F68-0x80184558 in one gcc_2_8_1_g0_split object:
   its lifecycle (init, update, inventory refresh), the four card-stat qsort
   comparators, the offer display and bookkeeping, the name and count
   comparators, and the icon, column and row helpers, in executable order.
   The resident Main_RunTrade enters through the init, the update and the
   display-handle release; the six comparators are reached only through the
   refresh's pointer table. */

/* The Trade screen state, zero-initialized and common (this unit builds with
   gcc_2_8_1_g0_split_comm); each layout's trade_screen_commons subsegment
   places it at that image's addresses. gTrade_abCursorRow is only
   halfword-aligned there, as common storage allows. */
DisplayObject *gTrade_pBackground;
DisplayObject *gTrade_pStatsModeDisplay;
MainMenuSlot gTrade_aCardSlots[2];
CardCountEntry gTrade_aWorkingCards[2][CARD_COUNT];
MainMenuPair gTrade_aListScroll[4];
u16 gTrade_awOffers[2][11];
u8 gTrade_abReady[2];
u8 gTrade_abCursorRow[2];
u8 gTrade_abSortMode[2];
u8 gTrade_bFocusPane;
u8 gTrade_bDecisionMenuActive;
u8 gTrade_bCommitRequested;
u8 gTrade_bWritePending;

void MainMenu_InitTradeScreen(void)
{
    DisplayObject *object;
    DisplayObject *entry;
#ifdef VERSION_EUROPE
    DisplayObject *quad;
#endif
    s32 i;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    gTrade_pBackground = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPosition(object, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        gTrade_pBackground->flags |=
            DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
#ifdef VERSION_EUROPE
        DisplayObject_SetDepthOffset(gTrade_pBackground, -3);
#else
        DisplayObject_SetDepthOffset(gTrade_pBackground, -2);
#endif
    }

#ifdef VERSION_EUROPE
    quad = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 4);
    DisplayObject_InitializeGouraudQuad(quad, 0);
    quad->attribute |= 0x60000000;
    DisplayObject_SetDepthOffset(quad, -2);
    quad->position.word = 0xF80000;
    quad->xy.word = 0xF80140;
    quad->field_38.word = 0x1000000;
    quad->field_40.word = 0x1000140;
    quad->field_3C.word = 0x808080;
    quad->field_44.word = 0x808080;

    quad = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 4);
    DisplayObject_InitializeGouraudQuad(quad, 0);
    quad->attribute |= 0x60000000;
    DisplayObject_SetDepthOffset(quad, -2);
    quad->xy.word = 0x140;
    quad->position.word = 0;
    quad->field_38.word = 0x80000;
    quad->field_40.word = 0x80140;
    quad->field_2C.word = 0x808080;
    quad->field_34.word = 0x808080;
#endif

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    gTrade_pStatsModeDisplay = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPosition(object, 0, -3, 0, 4, 4, 0xC, 0x208);
        gTrade_pStatsModeDisplay->flags |=
            DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SetDepthOffset(gTrade_pStatsModeDisplay, -1);
    }

    CardList_CreateDeckBoxStatRows(0, 0x25, 0xA0, 0x25);

    for (i = 0; i < 2; i++) {
        entry = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        if (entry != 0) {
            DisplayObject_ConfigureSpriteAtPosition(entry, i * 0xA0 + 0x1E, 0x24, 0, 4, 8, 0xC, 0x208);
            entry->flags |=
                DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
            DisplayObject_SetDepthOffset(entry, 0);
            entry->field_60.step = -2;
            gTrade_aCardSlots[i].object = entry;
        } else {
            gTrade_aCardSlots[i].object = 0;
        }
        gTrade_aCardSlots[i].unk4 = 0;
        gTrade_aListScroll[i].target = 0;
        gTrade_aListScroll[i].current = 0;
        gTrade_awOffers[i][0] = 0;
        gTrade_abReady[i] = 0;
        gTrade_abCursorRow[i] = 0;
        gTrade_abSortMode[i] = 0;
        MainMenu_RefreshTradeInventory(i, 1);
        MainMenu_RebuildTradeInventoryRows(i);
    }

    gTrade_bFocusPane = 0;
    gTrade_bDecisionMenuActive = 0;
    gTrade_bCommitRequested = 0;
    gTrade_bWritePending = 0;
    gMain_apFrameServiceCallbacks[1] = MainMenu_DrawTradeOffersAndHighlights;
    gGraphics_bSyncVBlankTarget = 1;
}

#ifdef VERSION_JAPAN
#define MAIN_MENU_TRADE_CANCEL_BUTTON PAD_BUTTON_CROSS
#define MAIN_MENU_TRADE_CONFIRM_BUTTON PAD_BUTTON_CIRCLE
#define MAIN_MENU_TRADE_CANCEL_SHIFT 6
#else
#define MAIN_MENU_TRADE_CANCEL_BUTTON PAD_BUTTON_CIRCLE
#define MAIN_MENU_TRADE_CONFIRM_BUTTON PAD_BUTTON_CROSS
#define MAIN_MENU_TRADE_CANCEL_SHIFT 5
#endif
#define MAIN_MENU_TRADE_CONFIRM_MASK (MAIN_MENU_TRADE_CONFIRM_BUTTON | PAD_BUTTON_SQUARE)
#ifdef VERSION_EUROPE
#define MAIN_MENU_TRADE_MAXIMUM 714
#define MAIN_MENU_TRADE_PAGE_STEP 8
#define MAIN_MENU_TRADE_SCROLL_MAX 7
#else
#define MAIN_MENU_TRADE_MAXIMUM 715
#define MAIN_MENU_TRADE_PAGE_STEP 7
#define MAIN_MENU_TRADE_SCROLL_MAX 6
#endif

#define MAIN_MENU_TRADE_BLOCK_INDEX(byte_offset) \
    ((byte_offset) / sizeof(MainMenuTradeBlock16))

/*
 * Exact pure-C match under gcc_2_8_1_g0_split. Natural restore-pointer roles
 * recover a1/a2, and the dirty-counter increment repeated in the sort and
 * stats-mode arms gives GCC the weight needed to allocate that counter to s3
 * without a hard-register binding; cross-jumping folds the second increment
 * back into the first.
 */
s32 MainMenu_UpdateTradeScreen(void)
{
    s32 bounded;
    s32 low;
    s32 decision;
    s32 dirty0;
    s32 dirty1;
    s32 cursor0;
    s32 cursor1;
    s32 flags;
    s32 previousFlags;
    s32 maximum;
    s32 result;
    s32 i;
    s32 j;
    s32 id;
    s32 value;
    s32 other;
    u8 *counts[2];
    u8 *clearFlags;
    u8 *from;
    u8 *to;
    MainMenuTradeBlock16 *base;
    MainMenuTradeBlock16 *source;
    MainMenuTradeBlock16 *save_source;
    MainMenuTradeBlock16 *destination;
    MainMenuTradeBlock16 *save_destination;
    MainMenuTradeBlock16 *end;
    MainMenuTradeBlock16 *source2;
    MainMenuTradeBlock16 *backup_source2;
    MainMenuTradeBlock16 *destination2;
    MainMenuTradeBlock16 *backup_destination2;
    CardCountEntry *card0;
    CardCountEntry *card1;

    dirty0 = 0;
    dirty1 = dirty0;
    cursor0 = 1;
    cursor1 = cursor0;
    value = gTrade_pStatsModeDisplay->resource_variant - 4;
    previousFlags = value;
    flags = value;
    maximum = MAIN_MENU_TRADE_MAXIMUM;

    if (gTrade_bWritePending != 0) {
        result = MemCardDialog_Poll();
        if (result == 0) {
            return 0;
        }
        if (result == 1) {
            destination = (MainMenuTradeBlock16 *)gSaveData_aPairWorkspace;
            source2 = destination +
                MAIN_MENU_TRADE_BLOCK_INDEX(TWO_PLAYER_SAVE_SLOT_STRIDE);
            destination2 = destination + MAIN_MENU_TRADE_BLOCK_INDEX(
                TWO_PLAYER_SAVE_SLOT_STRIDE + SAVE_DATA_STATE_SIZE
            );
            source = destination +
                MAIN_MENU_TRADE_BLOCK_INDEX(SAVE_DATA_STATE_SIZE);
            end = source +
                MAIN_MENU_TRADE_BLOCK_INDEX(TWO_PLAYER_SAVE_TRANSFER_SIZE);
            do {
                *destination = *source;
                source++;
                destination++;
            } while (source != end);
            source = source2;
            destination = destination2;
            end = destination +
                MAIN_MENU_TRADE_BLOCK_INDEX(TWO_PLAYER_SAVE_TRANSFER_SIZE);
            do {
                *source = *destination;
                destination++;
                source++;
            } while (destination != end);
            i = 0;
            do {
                gTrade_awOffers[i][0] = 0;
                MainMenu_RefreshTradeInventory(i, 1);
                MainMenu_RebuildTradeInventoryRows(i);
                gTrade_abReady[i] = 0;
                i++;
            } while (i < 2);
            Input_ResetPads();
            gTrade_bDecisionMenuActive = 0;
            gTrade_bCommitRequested = 0;
            gTrade_bWritePending = 0;
        } else {
            MainMenu_CreateTradeDecisionMenu(
                *(volatile u8 *)&gTrade_bFocusPane);
            Input_ResetPads();
            gTrade_bCommitRequested = 0;
            gTrade_bWritePending = 0;
        }
        return 0;
    }

    if (gTrade_bCommitRequested != 0) {
        base = (MainMenuTradeBlock16 *)gSaveData_aPairWorkspace;
        backup_source2 = base +
            MAIN_MENU_TRADE_BLOCK_INDEX(TWO_PLAYER_SAVE_SLOT_STRIDE);
        backup_destination2 = base + MAIN_MENU_TRADE_BLOCK_INDEX(
            TWO_PLAYER_SAVE_SLOT_STRIDE + SAVE_DATA_STATE_SIZE
        );
        save_destination = base +
            MAIN_MENU_TRADE_BLOCK_INDEX(SAVE_DATA_STATE_SIZE);
        save_source = base;
        counts[0] = (u8 *)(base + MAIN_MENU_TRADE_BLOCK_INDEX(
            SAVE_DATA_STATE_SIZE + SAVE_DATA_CARD_QUANTITIES_OFFSET
        ));
        counts[1] = (u8 *)(base + MAIN_MENU_TRADE_BLOCK_INDEX(
            TWO_PLAYER_SAVE_SLOT_STRIDE + SAVE_DATA_STATE_SIZE +
                SAVE_DATA_CARD_QUANTITIES_OFFSET
        ));
        *(MainMenuTradeBlock1024 *)save_destination =
            *(MainMenuTradeBlock1024 *)save_source;
        save_source = backup_destination2;
        save_destination = backup_source2;
        *(MainMenuTradeBlock1024 *)save_source =
            *(MainMenuTradeBlock1024 *)save_destination;
        for (i = 0; i < 2; i++) {
            for (j = 0; j < gTrade_awOffers[i][0]; j++) {
                id = gTrade_awOffers[i][j + 1] - 1;
                from = counts[i] + id;
                if (*from != 0) {
                    *from = *from - 1;
                }
                to = counts[i ^ 1] + id;
                if (*to < CARD_CHEST_QUANTITY_MAX) {
                    *to = *to + 1;
                }
            }
        }
        SaveData_RequestTradeWrite();
        gTrade_bWritePending = 1;
        return 0;
    }

    if (gTrade_bDecisionMenuActive != 0) {
        decision = 0;
        if ((gInput_awPadRepeat[0] & PAD_DIRECTION_VERTICAL_MASK) != 0 ||
            (gInput_awPadRepeat[1] & PAD_DIRECTION_VERTICAL_MASK) != 0) {
            SD_SEPlay(SD_SE_CURSOR, 255, 0);
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_UP) != 0 ||
                (gInput_awPadRepeat[1] & PAD_DIRECTION_UP) != 0) {
                gTrade_bFocusPane = (gTrade_bFocusPane + 2) % 3;
            } else {
                gTrade_bFocusPane = (gTrade_bFocusPane + 4) % 3;
            }
            MainMenu_CreateTradeDecisionMenu(
                *(volatile u8 *)&gTrade_bFocusPane);
        } else if ((gInput_awPadPressed[0] & MAIN_MENU_TRADE_CANCEL_BUTTON) != 0 ||
            (gInput_awPadPressed[1] & MAIN_MENU_TRADE_CANCEL_BUTTON) != 0) {
            other = ((gInput_awPadPressed[0] >> MAIN_MENU_TRADE_CANCEL_SHIFT) ^ 1) & 1;
            SD_SEPlay(SD_SE_CANCEL, 255, 0);
            TextBox_Destroy(TEXT_CHANNEL(3));
            gTrade_abReady[other] = 0;
            MainMenu_CreateTradeReadyIndicator(other ^ 1);
            gTrade_bDecisionMenuActive = 0;
        } else if ((gInput_awPadPressed[0] & MAIN_MENU_TRADE_CONFIRM_MASK) != 0 ||
            (gInput_awPadPressed[1] & MAIN_MENU_TRADE_CONFIRM_MASK) != 0) {
            SD_SEPlay(SD_SE_COMMIT, 255, 0);
            switch (gTrade_bFocusPane) {
            case 0:
                TextBox_Destroy(TEXT_CHANNEL(3));
                gTrade_bCommitRequested = 1;
                break;
            case 1:
                TextBox_Destroy(TEXT_CHANNEL(3));
                i = 1;
                clearFlags = gTrade_abReady + i;
                do {
                    *clearFlags = 0;
                    i--;
                    clearFlags--;
                } while (i >= 0);
                gTrade_bDecisionMenuActive = 0;
                break;
            case 2:
                decision = 1;
                break;
            }
        }
        return decision;
    }

    switch (gTrade_aListScroll[0].current != gTrade_aListScroll[0].target) {
    case 0:
        card0 = &gTrade_aWorkingCards[0][gTrade_aListScroll[0].current + gTrade_abCursorRow[0]];
        if (gTrade_abReady[0] != 0) {
            if ((gInput_awPadPressed[0] & MAIN_MENU_TRADE_CANCEL_BUTTON) != 0) {
                SD_SEPlay(SD_SE_CANCEL, 255, 0);
                TextBox_Destroy(TEXT_CHANNEL(3));
                gTrade_abReady[0] = 0;
            }
        } else if ((gInput_awPadPressed[0] & PAD_BUTTON_SQUARE) != 0) {
            if (gTrade_abReady[1] != 0 &&
                gTrade_awOffers[0][0] + gTrade_awOffers[1][0] == 0) {
                SD_SEPlay(SD_SE_BUZZER, 255, 0);
            } else {
                SD_SEPlay(SD_SE_COMMIT, 255, 0);
                MainMenu_CreateTradeReadyIndicator(0);
                gTrade_abReady[0] = 1;
            }
        } else if ((gInput_awPadPressed[0] & PAD_BUTTON_TRIANGLE) != 0) {
            if (card0->id != 0) {
                gDuel_wViewerCardID = card0->id;
                gDuel_bCardViewerYOffset = 20;
                gDuel_bEffectState = 2;
            }
        } else if ((gInput_awPadRepeat[0] & (PAD_BUTTON_SELECT | PAD_BUTTON_START)) != 0) {
            SD_SEPlay(SD_SE_CHANGE, 255, 0);
            if ((gInput_awPadRepeat[0] & PAD_BUTTON_SELECT) != 0) {
                gTrade_abSortMode[0] = (gTrade_abSortMode[0] + 5) % 6;
            } else {
                gTrade_abSortMode[0] = (gTrade_abSortMode[0] + 7) % 6;
            }
            MainMenu_RefreshTradeInventory(0, 0);
            dirty0++;
        } else if ((gInput_awPadPressed[0] & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
            if ((gInput_awPadPressed[0] & PAD_DIRECTION_LEFT) != 0) {
                flags &= ~1;
            } else {
                flags |= 1;
            }
            dirty0++;
        } else if ((gInput_awPadPressed[0] & MAIN_MENU_TRADE_CANCEL_BUTTON) != 0) {
            if (gTrade_awOffers[0][0] != 0) {
                MainMenu_AdjustTradeCardCount(0, gTrade_awOffers[0][gTrade_awOffers[0][0]], 1);
                SD_SEPlay(SD_SE_CANCEL, 255, 0);
                gTrade_awOffers[0][0]--;
                dirty0 = 1;
            } else if (gTrade_awOffers[1][0] != 0 || gTrade_abReady[1] != 0) {
                SD_SEPlay(SD_SE_BUZZER, 255, 0);
            } else {
                SD_SEPlay(SD_SE_CANCEL, 255, 0);
                return 1;
            }
        } else if ((gInput_awPadPressed[0] & MAIN_MENU_TRADE_CONFIRM_BUTTON) != 0) {
            if (gTrade_awOffers[0][0] < 10 && card0->id != 0 && card0->count != 0) {
                SD_SEPlay(SD_SE_DECIDE, 255, 0);
                gTrade_awOffers[0][0]++;
                gTrade_awOffers[0][gTrade_awOffers[0][0]] = card0->id;
                card0->count--;
                dirty0 = 1;
            } else {
                SD_SEPlay(SD_SE_BUZZER, 255, 0);
            }
        } else if ((gInput_awPadHeld[0] & PAD_BUTTON_L1_R1_MASK) != 0) {
            if ((gInput_awPadHeld[0] & PAD_BUTTON_L1) != 0) {
                low = gTrade_aListScroll[0].target - MAIN_MENU_TRADE_PAGE_STEP;
                if (low < 0) { low = 0; }
                gTrade_aListScroll[0].target = low;
            } else {
                value = gTrade_aListScroll[0].target + MAIN_MENU_TRADE_PAGE_STEP;
                gTrade_aListScroll[0].target = value < maximum ? value : maximum;
            }
            if (gTrade_aListScroll[0].current == gTrade_aListScroll[0].target) {
                break;
            }
            SD_SEPlay(SD_SE_CURSOR, 255, 0);
        } else if ((gInput_awPadRepeat[0] & PAD_BUTTON_TRIGGER_MASK) != 0) {
            if ((gInput_awPadRepeat[0] & PAD_BUTTON_L2) != 0) {
                low = gTrade_aListScroll[0].target - 50;
                if (low < 0) { low = 0; }
                gTrade_aListScroll[0].target = low;
            } else {
                value = gTrade_aListScroll[0].target + 50;
                gTrade_aListScroll[0].target = value < maximum ? value : maximum;
            }
            if (gTrade_aListScroll[0].current != gTrade_aListScroll[0].target) {
                SD_SEPlay(SD_SE_CURSOR, 255, 0);
            }
            gTrade_aListScroll[0].current = gTrade_aListScroll[0].target;
            dirty0++;
        } else if ((gInput_awPadRepeat[0] & PAD_DIRECTION_VERTICAL_MASK) != 0) {
            if ((gInput_awPadRepeat[0] & PAD_DIRECTION_UP) != 0) {
                if (gTrade_abCursorRow[0] != 0) {
                    SD_SEPlay(SD_SE_CURSOR, 255, 0);
                    gTrade_abCursorRow[0]--;
                    cursor0++;
                } else if (gTrade_aListScroll[0].target != 0) {
                    SD_SEPlay(SD_SE_CURSOR, 255, 0);
                    gTrade_aListScroll[0].current = --gTrade_aListScroll[0].target;
                    dirty0++;
                }
            } else if (gTrade_abCursorRow[0] < MAIN_MENU_TRADE_SCROLL_MAX) {
                SD_SEPlay(SD_SE_CURSOR, 255, 0);
                gTrade_abCursorRow[0]++;
                cursor0++;
            } else if (gTrade_aListScroll[0].target < maximum) {
                SD_SEPlay(SD_SE_CURSOR, 255, 0);
                gTrade_aListScroll[0].current = ++gTrade_aListScroll[0].target;
                dirty0++;
            }
        }
        if (gTrade_aListScroll[0].current == gTrade_aListScroll[0].target) {
            break;
        }
    default:
        gTrade_aListScroll[0].current = gTrade_aListScroll[0].current < gTrade_aListScroll[0].target ?
            gTrade_aListScroll[0].current + 1 : gTrade_aListScroll[0].current - 1;
        dirty0++;
    }

    switch (gTrade_aListScroll[1].current != gTrade_aListScroll[1].target) {
    case 0:
        card1 = &gTrade_aWorkingCards[1][gTrade_aListScroll[1].current + gTrade_abCursorRow[1]];
        if (gTrade_abReady[1] != 0) {
            if ((gInput_awPadPressed[1] & MAIN_MENU_TRADE_CANCEL_BUTTON) != 0) {
                SD_SEPlay(SD_SE_CANCEL, 255, 0);
                TextBox_Destroy(TEXT_CHANNEL(3));
                gTrade_abReady[1] = 0;
            }
        } else if ((gInput_awPadPressed[1] & PAD_BUTTON_SQUARE) != 0) {
            if (gTrade_abReady[0] != 0 &&
                gTrade_awOffers[0][0] + gTrade_awOffers[1][0] == 0) {
                SD_SEPlay(SD_SE_BUZZER, 255, 0);
            } else {
                SD_SEPlay(SD_SE_COMMIT, 255, 0);
                MainMenu_CreateTradeReadyIndicator(1);
                gTrade_abReady[1] = 1;
            }
        } else if ((gInput_awPadPressed[1] & PAD_BUTTON_TRIANGLE) != 0) {
            if (card1->id != 0) {
                gDuel_wViewerCardID = card1->id;
                gDuel_bCardViewerYOffset = 20;
                gDuel_bEffectState = 2;
            }
        } else if ((gInput_awPadRepeat[1] & (PAD_BUTTON_SELECT | PAD_BUTTON_START)) != 0) {
            SD_SEPlay(SD_SE_CHANGE, 255, 0);
            if ((gInput_awPadRepeat[1] & PAD_BUTTON_SELECT) != 0) {
                gTrade_abSortMode[1] = (gTrade_abSortMode[1] + 5) % 6;
            } else {
                gTrade_abSortMode[1] = (gTrade_abSortMode[1] + 7) % 6;
            }
            MainMenu_RefreshTradeInventory(1, 0);
            dirty1++;
        } else if ((gInput_awPadPressed[1] & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
            if ((gInput_awPadPressed[1] & PAD_DIRECTION_LEFT) != 0) {
                flags &= ~2;
            } else {
                flags |= 2;
            }
            dirty1++;
        } else if ((gInput_awPadPressed[1] & MAIN_MENU_TRADE_CANCEL_BUTTON) != 0) {
            if (gTrade_awOffers[1][0] != 0) {
                MainMenu_AdjustTradeCardCount(1, gTrade_awOffers[1][gTrade_awOffers[1][0]], 1);
                SD_SEPlay(SD_SE_CANCEL, 255, 0);
                gTrade_awOffers[1][0]--;
                dirty1++;
            } else if (gTrade_awOffers[0][0] != 0 || gTrade_abReady[0] != 0) {
                SD_SEPlay(SD_SE_BUZZER, 255, 0);
            } else {
                SD_SEPlay(SD_SE_CANCEL, 255, 0);
                return 1;
            }
        } else if ((gInput_awPadPressed[1] & MAIN_MENU_TRADE_CONFIRM_BUTTON) != 0) {
            if (gTrade_awOffers[1][0] < 10 && card1->id != 0 && card1->count != 0) {
                SD_SEPlay(SD_SE_DECIDE, 255, 0);
                /* Use the increment result rather than reloading the count. */
                gTrade_awOffers[1][++gTrade_awOffers[1][0]] = card1->id;
                card1->count--;
                dirty1++;
            } else {
                SD_SEPlay(SD_SE_BUZZER, 255, 0);
            }
        } else if ((gInput_awPadHeld[1] & PAD_BUTTON_L1_R1_MASK) != 0) {
            if ((gInput_awPadHeld[1] & PAD_BUTTON_L1) != 0) {
                low = gTrade_aListScroll[1].target - MAIN_MENU_TRADE_PAGE_STEP;
                if (low < 0) { low = 0; }
                gTrade_aListScroll[1].target = low;
            } else {
                bounded = maximum;
                if (gTrade_aListScroll[1].target + MAIN_MENU_TRADE_PAGE_STEP < bounded) {
                    bounded = gTrade_aListScroll[1].target + MAIN_MENU_TRADE_PAGE_STEP;
                }
                gTrade_aListScroll[1].target = bounded;
            }
            if (gTrade_aListScroll[1].current == gTrade_aListScroll[1].target) {
                break;
            }
            SD_SEPlay(SD_SE_CURSOR, 255, 0);
        } else if ((gInput_awPadRepeat[1] & PAD_BUTTON_TRIGGER_MASK) != 0) {
            if ((gInput_awPadRepeat[1] & PAD_BUTTON_L2) != 0) {
                low = gTrade_aListScroll[1].target - 50;
                if (low < 0) { low = 0; }
                gTrade_aListScroll[1].target = low;
            } else {
                bounded = maximum;
                if (gTrade_aListScroll[1].target + 50 < bounded) {
                    bounded = gTrade_aListScroll[1].target + 50;
                }
                gTrade_aListScroll[1].target = bounded;
            }
            if (gTrade_aListScroll[1].current != gTrade_aListScroll[1].target) {
                SD_SEPlay(SD_SE_CURSOR, 255, 0);
            }
            gTrade_aListScroll[1].current = gTrade_aListScroll[1].target;
            dirty1++;
        } else if ((gInput_awPadRepeat[1] & PAD_DIRECTION_VERTICAL_MASK) != 0) {
            if ((gInput_awPadRepeat[1] & PAD_DIRECTION_UP) != 0) {
                if (gTrade_abCursorRow[1] != 0) {
                    SD_SEPlay(SD_SE_CURSOR, 255, 0);
                    gTrade_abCursorRow[1]--;
                    cursor1++;
                } else if (gTrade_aListScroll[1].target != 0) {
                    SD_SEPlay(SD_SE_CURSOR, 255, 0);
                    gTrade_aListScroll[1].current = --gTrade_aListScroll[1].target;
                    dirty1++;
                }
            } else if (gTrade_abCursorRow[1] < MAIN_MENU_TRADE_SCROLL_MAX) {
                SD_SEPlay(SD_SE_CURSOR, 255, 0);
                gTrade_abCursorRow[1]++;
                cursor1++;
            } else if (gTrade_aListScroll[1].target < maximum) {
                SD_SEPlay(SD_SE_CURSOR, 255, 0);
                gTrade_aListScroll[1].current = ++gTrade_aListScroll[1].target;
                dirty1++;
            }
        }
        if (gTrade_aListScroll[1].current == gTrade_aListScroll[1].target) {
            break;
        }
    default:
        gTrade_aListScroll[1].current = gTrade_aListScroll[1].current < gTrade_aListScroll[1].target ?
            gTrade_aListScroll[1].current + 1 : gTrade_aListScroll[1].current - 1;
        dirty1++;
    }

    if (cursor0 != 0) {
        gTrade_aCardSlots[0].object->xy.h.y = gTrade_abCursorRow[0] * 22 + 36;
    }
    if (cursor1 != 0) {
        gTrade_aCardSlots[1].object->xy.h.y = gTrade_abCursorRow[1] * 22 + 36;
    }
    if (flags != previousFlags) {
        SD_SEPlay(30, 255, 0);
        DisplayObject_SetResourceVariant(gTrade_pStatsModeDisplay, flags + 4);
    }
    if (dirty0 != 0) {
        MainMenu_RebuildTradeInventoryRows(0);
    }
    if (dirty1 != 0) {
        MainMenu_RebuildTradeInventoryRows(1);
    }
    if (gTrade_abReady[0] != 0 && gTrade_abReady[1] != 0) {
        gTrade_bFocusPane = 0;
        TextBox_Destroy(TEXT_CHANNEL(3));
        TextBox_Destroy(TEXT_CHANNEL(3));
        MainMenu_CreateTradeDecisionMenu(*(volatile u8 *)&gTrade_bFocusPane);
        gTrade_bDecisionMenuActive = 1;
    }
    return 0;
}
#undef MAIN_MENU_TRADE_CANCEL_BUTTON
#undef MAIN_MENU_TRADE_CONFIRM_BUTTON
#undef MAIN_MENU_TRADE_CANCEL_SHIFT
#undef MAIN_MENU_TRADE_CONFIRM_MASK
#undef MAIN_MENU_TRADE_MAXIMUM
#undef MAIN_MENU_TRADE_PAGE_STEP
#undef MAIN_MENU_TRADE_SCROLL_MAX

void MainMenu_RefreshTradeInventory(s32 slot, s32 force)
{
    MainMenuComparators comparators;
    SaveDataPairSlot *row;
    u8 *counts;
    s32 i;
    s32 mode;
    s32 id;

    mode = gTrade_abSortMode[slot];
    comparators = gMainMenu_aCardSortComparators;

    if (force != 0 || mode == 0) {
        row = &gSaveData_aPairWorkspace[slot];
        counts = row->loaded.state.card_quantities;
        for (i = 0; i < CARD_COUNT; i++) {
            if (counts[i] != 0) {
                id = i + 1;
            } else {
                id = 0;
            }
            gTrade_aWorkingCards[slot][i].id = id;
            gTrade_aWorkingCards[slot][i].count = counts[i];
        }
        for (i = 0; i < 2; i++) {
            MainMenu_ApplyTradeOfferInventoryDelta(i, -1);
        }
    }

    if (mode != 0) {
        qsort(
            gTrade_aWorkingCards[slot],
            CARD_COUNT,
            sizeof(CardCountEntry),
            comparators.entries[mode - 1]
        );
    }

    gFile_StagingBuffer.card_list_rows[slot].object->resource_variant = mode;
}

s32 MainMenu_CompareCardsByMaxStat(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 statsA;
    s32 statsB;
    s32 atkA;
    s32 atkB;
    s32 defA;
    s32 defB;
    s32 hiA;
    s32 hiB;
    s32 loA;
    s32 loB;
    s32 keyA;
    s32 keyB;

    idA = *a;
    idB = *b;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
        /* Returning here gives the function a shared return label, which
           keeps the final copy to v0 ahead of the epilogue's ra restore. */
        return result;
    } else {
        if (idA != 0) {
            keyA = gCard_asNameSortKey[idA - 1];
            statsA = gDuel_adwCardStats[idA - 1];
            if (((statsA >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkA = (statsA & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defA = ((statsA >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                hiA = defA;
                if (atkA > hiA) {
                    hiA = atkA;
                }
                loA = defA;
                if (atkA < loA) {
                    loA = atkA;
                }
            } else {
                loA = 0x80000001;
                hiA = 0x80000001;
            }
        } else {
            keyA = 0x7FFFFFFF;
            loA = 0x80000001;
            hiA = 0x80000001;
        }
        if (idB != 0) {
            keyB = gCard_asNameSortKey[idB - 1];
            statsB = gDuel_adwCardStats[idB - 1];
            if (((statsB >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkB = (statsB & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defB = ((statsB >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                hiB = defB;
                if (atkB > hiB) {
                    hiB = atkB;
                }
                loB = defB;
                if (atkB < loB) {
                    loB = atkB;
                }
            } else {
                loB = 0x80000001;
                hiB = 0x80000001;
            }
        } else {
            keyB = 0x7FFFFFFF;
            loB = 0x80000001;
            hiB = 0x80000001;
        }
        if (hiB <= hiA) {
            if (hiA <= hiB) {
                if (loB <= loA) {
                    if (loA <= loB) {
                        if (keyA <= keyB) {
                            if (keyA < keyB) {
                                result = -1;
                            } else {
                                result = 0;
                            }
                        } else {
                            result = 1;
                        }
                    } else {
                        result = -1;
                    }
                } else {
                    result = 1;
                }
            } else {
                result = -1;
            }
        } else {
            result = 1;
        }
    }
    return result;
}

s32 MainMenu_CompareCardsByAttack(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 statsA;
    s32 statsB;
    s32 atkA;
    s32 atkB;
    s32 defA;
    s32 defB;
    s32 keyA;
    s32 keyB;

    idA = *a;
    idB = *b;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
        return result;
    } else {
        if (idA != 0) {
            keyA = gCard_asNameSortKey[idA - 1];
            statsA = gDuel_adwCardStats[idA - 1];
            if (((statsA >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkA = (statsA & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defA = ((statsA >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            } else {
                defA = 0x80000001;
                atkA = 0x80000001;
            }
        } else {
            keyA = 0x7FFFFFFF;
            defA = 0x80000001;
            atkA = 0x80000001;
        }
        if (idB != 0) {
            keyB = gCard_asNameSortKey[idB - 1];
            statsB = gDuel_adwCardStats[idB - 1];
            if (((statsB >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkB = (statsB & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defB = ((statsB >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            } else {
                defB = 0x80000001;
                atkB = 0x80000001;
            }
        } else {
            keyB = 0x7FFFFFFF;
            defB = 0x80000001;
            atkB = 0x80000001;
        }
        if (atkB <= atkA) {
            if (atkA <= atkB) {
                if (defB <= defA) {
                    if (defA <= defB) {
                        if (keyA <= keyB) {
                            if (keyA < keyB) {
                                result = -1;
                            } else {
                                result = 0;
                            }
                        } else {
                            result = 1;
                        }
                    } else {
                        result = -1;
                    }
                } else {
                    result = 1;
                }
            } else {
                result = -1;
            }
        } else {
            result = 1;
        }
    }
    return result;
}

s32 MainMenu_CompareCardsByDefense(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 statsA;
    s32 statsB;
    s32 atkA;
    s32 atkB;
    s32 defA;
    s32 defB;
    s32 keyA;
    s32 keyB;

    idA = *a;
    idB = *b;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
        return result;
    } else {
        if (idA != 0) {
            keyA = gCard_asNameSortKey[idA - 1];
            statsA = gDuel_adwCardStats[idA - 1];
            if (((statsA >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkA = (statsA & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defA = ((statsA >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            } else {
                atkA = 0x80000001;
                defA = 0x80000001;
            }
        } else {
            keyA = 0x7FFFFFFF;
            atkA = 0x80000001;
            defA = 0x80000001;
        }
        if (idB != 0) {
            keyB = gCard_asNameSortKey[idB - 1];
            statsB = gDuel_adwCardStats[idB - 1];
            if (((statsB >> CARD_STAT_TYPE_SHIFT) &
                 CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
                atkB = (statsB & CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
                defB = ((statsB >> CARD_STAT_DEFENSE_SHIFT) &
                        CARD_STAT_VALUE_MASK) * CARD_STAT_SCALE;
            } else {
                atkB = 0x80000001;
                defB = 0x80000001;
            }
        } else {
            keyB = 0x7FFFFFFF;
            atkB = 0x80000001;
            defB = 0x80000001;
        }
        if (defB <= defA) {
            if (defA <= defB) {
                if (atkB <= atkA) {
                    if (atkA <= atkB) {
                        if (keyA <= keyB) {
                            if (keyA < keyB) {
                                result = -1;
                            } else {
                                result = 0;
                            }
                        } else {
                            result = 1;
                        }
                    } else {
                        result = -1;
                    }
                } else {
                    result = 1;
                }
            } else {
                result = -1;
            }
        } else {
            result = 1;
        }
    }
    return result;
}

s32 MainMenu_CompareCardsByType(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 typeA;
    s32 typeB;
    s32 keyA;
    s32 keyB;

    idA = *a;
    idB = *b;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
        return result;
    } else {
        if (idA != 0) {
            keyA = gCard_asNameSortKey[idA - 1];
            typeA = (gDuel_adwCardStats[idA - 1] >> CARD_STAT_TYPE_SHIFT) &
                    CARD_STAT_TYPE_MASK;
        } else {
            typeA = 0x7FFFFFFF;
            keyA = 0x7FFFFFFF;
        }
        if (idB != 0) {
            keyB = gCard_asNameSortKey[idB - 1];
            typeB = (gDuel_adwCardStats[idB - 1] >> CARD_STAT_TYPE_SHIFT) &
                    CARD_STAT_TYPE_MASK;
        } else {
            typeB = 0x7FFFFFFF;
            keyB = 0x7FFFFFFF;
        }
        if (typeA <= typeB) {
            if (typeA >= typeB) {
                if (keyA <= keyB) {
                    if (keyA < keyB) {
                        result = -1;
                    } else {
                        result = 0;
                    }
                } else {
                    result = 1;
                }
            } else {
                result = -1;
            }
        } else {
            result = 1;
        }
    }
    return result;
}

/* The Trade screen's offer display and its inventory bookkeeping: the offer
   and highlight draw (0x80183B2C), the three-digit number it prints
   (0x80183E8C), the display-handle release (0x80183FE4), the offer inventory
   delta (0x80184030) and the per-card count it applies (0x801840F8).

   They call each other and share their data. The offer draw calls
   MainMenu_DrawThreeDigitNumber for every offered card, the offer delta calls
   MainMenu_AdjustTradeCardCount for every entry it walks, and the offer list
   gTrade_awOffers is read by the draw and written through by the delta. */

#ifdef VERSION_EUROPE
#define MAIN_MENU_TRADE_OFFER_Y 0xD2
#define MAIN_MENU_TRADE_DIGIT_V_TOP 0x40
#define MAIN_MENU_TRADE_DIGIT_V_BOTTOM 0x48
#define MAIN_MENU_TRADE_DIGIT_CLUT 0x3168
#define MAIN_MENU_TRADE_DIGIT_COLOR 0x80
#define MAIN_MENU_TRADE_DIGIT_TPAGE 10
#else
#define MAIN_MENU_TRADE_OFFER_Y 0xBC
#define MAIN_MENU_TRADE_DIGIT_V_TOP 0x70
#define MAIN_MENU_TRADE_DIGIT_V_BOTTOM 0x78
#define MAIN_MENU_TRADE_DIGIT_CLUT 0x3EA9
#define MAIN_MENU_TRADE_DIGIT_COLOR 0xFF
#define MAIN_MENU_TRADE_DIGIT_TPAGE 11
#endif

void MainMenu_DrawTradeOffersAndHighlights(void)
{
    DisplayObject *volatile obj[2];
    s32 i;

    obj[0] = gTrade_aCardSlots[0].object;
    obj[1] = gTrade_aCardSlots[1].object;

    if (gTrade_abReady[0] != 0) {
        MainMenu_DrawTradeColumnOverlay(0);
        obj[0]->rgb.c.r = obj[0]->rgb.c.g =
            obj[0]->rgb.c.b = 0x40;
        obj[0]->field_60.step = 2;
    } else {
        obj[0]->rgb.c.r += obj[0]->field_60.step;
        obj[0]->rgb.c.g += obj[0]->field_60.step;
        obj[0]->rgb.c.b += obj[0]->field_60.step;
        if (obj[0]->rgb.c.r < 0x41 ||
            (s8)obj[0]->rgb.c.r < 0) {
            obj[0]->field_60.step *= -1;
        }
    }

    if (gTrade_abReady[1] != 0) {
        MainMenu_DrawTradeColumnOverlay(1);
        obj[1]->rgb.c.r = obj[1]->rgb.c.g =
            obj[1]->rgb.c.b = 0x40;
        obj[1]->field_60.step = 2;
    } else {
        obj[1]->rgb.c.r += obj[1]->field_60.step;
        obj[1]->rgb.c.g += obj[1]->field_60.step;
        obj[1]->rgb.c.b += obj[1]->field_60.step;
        if (obj[1]->rgb.c.r < 0x41 ||
            (s8)obj[1]->rgb.c.r < 0) {
            obj[1]->field_60.step *= -1;
        }
    }

    i = 0;
    if (gTrade_awOffers[0][0] != 0) {
        do {
            MainMenu_DrawThreeDigitNumber((i % 5) * 28 + 0x10,
                          (i / 5) * 17 + MAIN_MENU_TRADE_OFFER_Y,
                          gTrade_awOffers[0][i + 1]);
            MainMenu_DrawCardTypeIcon((i % 5) * 28 + 0x10,
                          (i / 5) * 17 + MAIN_MENU_TRADE_OFFER_Y,
                          gTrade_awOffers[0][i + 1]);
            i++;
        } while (i < gTrade_awOffers[0][0]);
    }

    i = 0;
    if (gTrade_awOffers[1][0] != 0) {
        do {
            MainMenu_DrawThreeDigitNumber((i % 5) * 28 + 0xB0,
                          (i / 5) * 17 + MAIN_MENU_TRADE_OFFER_Y,
                          gTrade_awOffers[1][i + 1]);
            MainMenu_DrawCardTypeIcon((i % 5) * 28 + 0xB0,
                          (i / 5) * 17 + MAIN_MENU_TRADE_OFFER_Y,
                          gTrade_awOffers[1][i + 1]);
            i++;
        } while (i < gTrade_awOffers[1][0]);
    }
}

void MainMenu_DrawThreeDigitNumber(s32 x, s32 y, s32 value)
{
    POLY_FT4 sprite;
    s32 i;
    s32 quotient;
    s32 digit;
    s32 base;
    s32 left;
    s32 right;
    s32 u;
    s32 top;
    s32 bottom;

    i = 0;
    top = y + 4;
    bottom = y + 12;
    setPolyFT4(&sprite);
    sprite.tpage = MAIN_MENU_TRADE_DIGIT_TPAGE;
    sprite.clut = MAIN_MENU_TRADE_DIGIT_CLUT;
    sprite.r0 = MAIN_MENU_TRADE_DIGIT_COLOR;
    sprite.g0 = MAIN_MENU_TRADE_DIGIT_COLOR;
    sprite.b0 = MAIN_MENU_TRADE_DIGIT_COLOR;
    for (; i < 3; i++) {
        base = x + 24 - i * 8;
        left = base - 12;
        sprite.x0 = left;
        right = base - 4;
        sprite.x1 = right;
        sprite.x3 = right;
        quotient = value / 10;
        sprite.y0 = top;
        sprite.y1 = top;
        sprite.x2 = left;
        sprite.y2 = bottom;
        sprite.y3 = bottom;
        sprite.v0 = MAIN_MENU_TRADE_DIGIT_V_TOP;
        sprite.v1 = MAIN_MENU_TRADE_DIGIT_V_TOP;
        sprite.v2 = MAIN_MENU_TRADE_DIGIT_V_BOTTOM;
        sprite.v3 = MAIN_MENU_TRADE_DIGIT_V_BOTTOM;
        digit = value - quotient * 10;
        u = digit * 8;
        sprite.u0 = u - 128;
        sprite.u1 = u - 120;
        sprite.u2 = u - 128;
        sprite.u3 = u - 120;
        GsSortPoly(&sprite, gGraphics_apOrderingTables[1], 32);
        value = quotient;
    }
}

void MainMenu_ReleaseTradeDisplayHandles(void)
{
    DisplayObject_ReleaseIfPresent(gTrade_pBackground);
    gTrade_pBackground = 0;
    DisplayObject_ReleaseIfPresent(gTrade_pStatsModeDisplay);
    gTrade_pStatsModeDisplay = 0;
    gMain_apFrameServiceCallbacks[1] = 0;
}

void MainMenu_ApplyTradeOfferInventoryDelta(s32 slot, s32 amount)
{
    s32 i;

    i = 0;
    while (i < gTrade_awOffers[slot][0]) {
        MainMenu_AdjustTradeCardCount(slot, gTrade_awOffers[slot][i + 1], (u32)amount);
        i++;
    }
}

void MainMenu_AdjustTradeCardCount(s32 slot, s32 id, u32 amount)
{
    CardCountEntry *entry;
    u32 total;
    s32 i;

    i = 0;
    while (i < CARD_COUNT) {
        entry = &gTrade_aWorkingCards[slot][i];
        if (entry->id == id) {
            total = entry->count + amount;
            if (total < 0xFB) {
                entry->count = total;
            }
            return;
        }
        i++;
    }
}

s32 MainMenu_CompareCardsByName(s16 *a, s16 *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 keyA;
    s32 keyB;

    idA = *a;
    idB = *b;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
        /* Returning here gives the function a shared return label, which
           keeps the final copy to v0 ahead of the epilogue's ra restore. */
        return result;
    } else {
        if (idA != 0) {
            keyA = gCard_asNameSortKey[idA - 1];
        } else {
            keyA = 0x7FFFFFFF;
            idA = 0x7FFFFFFF;
        }
        if (idB != 0) {
            keyB = gCard_asNameSortKey[idB - 1];
        } else {
            keyB = 0x7FFFFFFF;
            idB = 0x7FFFFFFF;
        }
        if (keyA <= keyB) {
            if (keyA >= keyB) {
                if (idA <= idB) {
                    if (idA < idB) {
                        result = -1;
                    } else {
                        result = 0;
                    }
                } else {
                    result = 1;
                }
            } else {
                result = -1;
            }
        } else {
            result = 1;
        }
    }
    return result;
}

s32 MainMenu_CompareCardsByCount(CardCountEntry *a, CardCountEntry *b)
{
    s32 result;
    s32 idA;
    s32 idB;
    s32 keyA;
    s32 keyB;
    s32 countKeyA;
    s32 countKeyB;

    idA = a->id;
    idB = b->id;
    if (idA == idB) {
        if (rand() & 1) {
            result = 1;
        } else {
            result = -1;
        }
        return result;
    } else {
        if (idA != 0) {
            keyA = gCard_asNameSortKey[idA - 1];
            countKeyA = a->count;
        } else {
            keyA = 0x7FFFFFFF;
            countKeyA = 0x80000001;
        }
        if (idB != 0) {
            keyB = gCard_asNameSortKey[idB - 1];
            countKeyB = b->count;
        } else {
            keyB = 0x7FFFFFFF;
            countKeyB = 0x80000001;
        }
        if (countKeyA < countKeyB) {
            result = 1;
        } else if (countKeyB < countKeyA) {
            result = -1;
        } else if (keyB < keyA) {
            result = 1;
        } else if (keyA < keyB) {
            result = -1;
        } else {
            result = 0;
        }
    }
    return result;
}

/* Three Trade screen helpers: the card-type icon (0x80184344) and the column
   dimming overlay (0x80184454) that the offer draw calls for each entry, and
   the inventory row rebuild (0x801844D8).

   Unlike the offer group they do not call each other. What ties them is the
   screen and its data: all three take their declarations from
   trade_helpers.h, and the row rebuild walks gTrade_aWorkingCards and gTrade_pStatsModeDisplay, the
   same trade inventory and display handle the offer group writes. */

#ifdef VERSION_EUROPE
#define MAIN_MENU_CARD_TYPE_PALETTE_OFFSET 0x80
#define MAIN_MENU_CARD_TYPE_V_TOP 0xC0
#define MAIN_MENU_CARD_TYPE_V_BOTTOM 0xD0
#define MAIN_MENU_TRADE_COLUMN_BOTTOM 0x100
#else
#define MAIN_MENU_CARD_TYPE_PALETTE_OFFSET 0
#define MAIN_MENU_CARD_TYPE_V_TOP 0xC8
#define MAIN_MENU_CARD_TYPE_V_BOTTOM 0xD8
#define MAIN_MENU_TRADE_COLUMN_BOTTOM 0xF0
#endif

void MainMenu_DrawCardTypeIcon(s32 x, s32 y, s32 cardID)
{
    POLY_FT4 sprite;
    u32 palette;
    s32 cardType;

    cardType = (gDuel_adwCardStats[cardID - 1] >> CARD_STAT_TYPE_SHIFT) &
                CARD_STAT_TYPE_MASK;
    if (cardType != CARD_TYPE_MAGIC && cardType != CARD_TYPE_EQUIP) {
        if (cardType != CARD_TYPE_TRAP) {
            if (cardType == CARD_TYPE_RITUAL) {
                palette = 0x290 + MAIN_MENU_CARD_TYPE_PALETTE_OFFSET;
            } else {
                palette = 0x260 + MAIN_MENU_CARD_TYPE_PALETTE_OFFSET;
            }
        } else {
            palette = 0x280 + MAIN_MENU_CARD_TYPE_PALETTE_OFFSET;
        }
    } else {
        palette = 0x270 + MAIN_MENU_CARD_TYPE_PALETTE_OFFSET;
    }
    setPolyFT4(&sprite);
    sprite.r0 = 0x80;
    sprite.g0 = 0x80;
    sprite.b0 = 0x80;
    sprite.tpage = 0xB;
    sprite.clut = (palette >> 4) | 0x3F00;
    sprite.x0 = x;
    sprite.y0 = y;
    sprite.x1 = x + 0x10;
    sprite.y1 = y;
    sprite.x2 = x;
    sprite.y2 = y + 0x10;
    sprite.x3 = x + 0x10;
    sprite.y3 = y + 0x10;
    sprite.u0 = 0;
    sprite.v0 = MAIN_MENU_CARD_TYPE_V_TOP;
    sprite.u1 = 0x10;
    sprite.v1 = MAIN_MENU_CARD_TYPE_V_TOP;
    sprite.u2 = 0;
    sprite.v2 = MAIN_MENU_CARD_TYPE_V_BOTTOM;
    sprite.u3 = 0x10;
    sprite.v3 = MAIN_MENU_CARD_TYPE_V_BOTTOM;
    GsSortPoly(&sprite, gGraphics_apOrderingTables[1], 0x20);
}

void MainMenu_DrawTradeColumnOverlay(s32 column)
{
    POLY_F4 quad;
    s32 left = column * 160;
    s32 right = left + 0xA0;

    setPolyF4(&quad);
    quad.r0 = 0x40;
    quad.g0 = 0x40;
    quad.b0 = 0x40;
    quad.x0 = left;
    quad.y0 = 0;
    quad.x1 = right;
    quad.y1 = 0;
    quad.x2 = left;
    quad.y2 = MAIN_MENU_TRADE_COLUMN_BOTTOM;
    quad.x3 = right;
    quad.y3 = MAIN_MENU_TRADE_COLUMN_BOTTOM;
    Graphics_SubmitDrawModePacket((u32 *)&quad, gGraphics_apOrderingTables[1], 0x1F, 2);
}

void MainMenu_RebuildTradeInventoryRows(s32 side)
{
    s32 flags;

    flags = gTrade_pStatsModeDisplay->resource_variant - 4;
    CardList_BuildRowSetTextBoxes((u16 *)&gTrade_aWorkingCards[side][gTrade_aListScroll[side].current], side,
                  flags & (1 << side), flags);
}
