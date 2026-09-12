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

extern MainMenuPair D_80185C8C[];
extern u16 D_80185C9C[2][11];
extern u8 D_80185CC8[2];
extern u8 D_80185CCA[2];
extern u8 D_80185CCC[2];

void MainMenu_InitTradeScreen(void)
{
    u8 *object;
    u8 *entry;
    s32 i;

    object = func_800400AC(func_8004002C(), 2);
    D_801845DC = (MainMenuWidget *)object;
    if (object != 0) {
        func_800404CC(object, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        *(u16 *)((u8 *)D_801845DC + 8) |=
            DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        func_800428EC((u8 *)D_801845DC, -2);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_801845E0 = (MainMenuWidget *)object;
    if (object != 0) {
        func_800404CC(object, 0, -3, 0, 4, 4, 0xC, 0x208);
        *(u16 *)((u8 *)D_801845E0 + 8) |=
            DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        func_800428EC((u8 *)D_801845E0, -1);
    }

    func_80061008(0, 0x25, 0xA0, 0x25);

    for (i = 0; i < 2; i++) {
        entry = func_800400AC(func_8004002C(), 2);
        if (entry != 0) {
            func_800404CC(entry, i * 0xA0 + 0x1E, 0x24, 0, 4, 8, 0xC, 0x208);
            *(u16 *)(entry + 8) |=
                DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
            func_800428EC(entry, 0);
            *(s16 *)(entry + 0x60) = -2;
            D_801845EC[i].object = entry;
        } else {
            D_801845EC[i].object = 0;
        }
        D_801845EC[i].unk4 = 0;
        D_80185C8C[i].target = 0;
        D_80185C8C[i].current = 0;
        D_80185C9C[i][0] = 0;
        D_80185CC8[i] = 0;
        D_80185CCA[i] = 0;
        D_80185CCC[i] = 0;
        MainMenu_RefreshTradeInventory(i, 1);
        MainMenu_RebuildTradeInventoryRows(i);
    }

    D_80185CCE = 0;
    D_80185CCF = 0;
    D_80185CD0 = 0;
    D_80185CD1 = 0;
    D_800E9DB0[1] = MainMenu_DrawTradeOffersAndHighlights;
    D_8009B0C0 = 1;
}
