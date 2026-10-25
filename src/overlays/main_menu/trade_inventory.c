#include "../../types.h"
#include "../../game/graphics_frame.h"
#include "../../game/display_object_helpers.h"
#include "../../game/main_services.h"
#include "../../game/two_player_save_setup.h"
#include "../../game/save_data.h"
#include "../../game/save_pair_workspace.h"
#include "../../ygo_types.h"
#include "../../psyq/qsort.h"
#include "../../game/card_constants.h"
#define MAIN_MENU_TRADE_STATE_BUFFER
#include "trade_helpers.h"
#undef MAIN_MENU_TRADE_STATE_BUFFER
#include "../../game/display_object_config.h"
#include "../../game/data_transfer_request.h"
#include "../../game/duel_card_viewer.h"
#include "../../game/duel_effect.h"
#include "../../game/sound.h"
#include "../../game/input.h"
#include "../../game/text_box_lifecycle.h"
#include "../../game/card_list_create_deck_box_stat_rows.h"
#include "../../game/func_800610E0.h"
#include "../../game/func_800611D0.h"

void MainMenu_RefreshTradeInventory(s32 slot, s32 force)
{
    MainMenuComparators comparators;
    u8 *row;
    u8 *counts;
    s32 i;
    s32 mode;
    s32 id;

    mode = gTrade_abSortMode[slot];
    comparators = gMainMenu_aCardSortComparators;

    if (force != 0 || mode == 0) {
        row = gSaveData_aPairWorkspace + slot * TWO_PLAYER_SAVE_SLOT_STRIDE;
        counts = row + SAVE_DATA_CARD_QUANTITIES_OFFSET;
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

    gTrade_aInventoryRowSets[slot].object[0x69] = mode;
}
