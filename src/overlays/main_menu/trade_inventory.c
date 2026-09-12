#include "../../types.h"
#include "../../game/graphics_frame.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_helpers.h"
#include "../../game/main_services.h"
#include "../../game/two_player_save_setup.h"
#include "../../game/save_data.h"
#include "../../ygo_types.h"
#include "../../psyq/qsort.h"
#include "../../game/card_constants.h"
#include "entrypoints.h"
#include "trade_helpers.h"
#include "../../game/display_object_config.h"
#include "../../game/data_transfer_request.h"
#include "../../game/duel_card_viewer.h"
#include "../../game/duel_effect.h"
#include "../../game/sound.h"
#include "../../game/input.h"
#include "../../game/text_box_lifecycle.h"
#include "../../game/func_80061008.h"
#include "../../game/func_800610E0.h"
#include "../../game/func_800611D0.h"

extern MainMenuState D_801A8000[];

void MainMenu_RefreshTradeInventory(s32 slot, s32 force)
{
    MainMenuComparators comparators;
    u8 *row;
    u8 *counts;
    s32 i;
    s32 mode;
    s32 id;

    mode = D_80185CCC[slot];
    comparators = *(MainMenuComparators *)&D_80180000[1];

    if (force != 0 || mode == 0) {
        row = D_801D1200 + slot * 0x1000;
        counts = row + 0x50;
        for (i = 0; i < CARD_COUNT; i++) {
            if (counts[i] != 0) {
                id = i + 1;
            } else {
                id = 0;
            }
            D_801845FC[slot][i].id = id;
            D_801845FC[slot][i].count = counts[i];
        }
        for (i = 0; i < 2; i++) {
            MainMenu_ApplyTradeOfferInventoryDelta(i, -1);
        }
    }

    if (mode != 0) {
        qsort(D_801845FC[slot], CARD_COUNT, 4, comparators.entries[mode - 1]);
    }

    D_801A8000[slot].object[0x69] = mode;
}
