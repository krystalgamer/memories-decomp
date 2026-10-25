/* The front-end menu's initialiser: the three singleton objects and the
 * wheel of eleven entries the module opens on. The per-frame update that
 * follows it, MainMenu_UpdateFrontendMenu, matched only through pinned
 * registers and is a build-integrated candidate since #3859
 * (src/candidates/main_menu/func_80180390.c). The background, the slide
 * transition and the afterimage sprites are in frontend_background.c. */
#include "../../types.h"
#include "../../game/two_player_save_setup.h"
#include "../../game/save_data.h"
#include "../../game/save_data_update_trade_load.h"
#include "../../unmatched.h"
#include "../../game/input.h"
#include "../../game/display_object.h"
#include "../../game/display_object_core.h"
#include "../../game/display_object_layout.h"
#include "../../psyq/libgte.h"
#include "frontend.h"
#include "../../game/display_object_helpers.h"
#include "../../game/main_services.h"
#include "../../game/display_object_config.h"
#include "../../game/sound_output.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_constants.h"
#include "../../game/data_transfer_request.h"
#include "../../game/mem_card.h"
#include "ordering_tables.h"
#include "../../game/sound.h"

#ifdef VERSION_EUROPE
#define MAIN_MENU_ENTRY_TOP_Y 58
#define MAIN_MENU_ENTRY_BOTTOM_Y 50
#else
#define MAIN_MENU_ENTRY_TOP_Y 50
#define MAIN_MENU_ENTRY_BOTTOM_Y 42
#endif

void MainMenu_InitFrontendMenu(s32 unused, s32 menu)
{
    DisplayObject *object;
    DisplayObject *entry;
    DisplayObject *third;
    DisplayObject *fourth;
    s32 i;
    s32 y;
    s32 value;
    u16 state;

    gMain_bMenuID = menu % 11;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    D_80184558 = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 0, 5, 0, 0, 0x1A, 1, gMainMenu_DisplayResources);
        D_80184558->attribute |= 0x1000000;
        D_80184558->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SetDepthOffset(D_80184558, 0);
    }

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    D_8018455C = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 8, 5, 0, 2, 0x1A, 1, gMainMenu_DisplayResources);
        D_8018455C->attribute |= 0x1000000;
        D_8018455C->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SetDepthOffset(D_8018455C, 1);
    }

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    gMainMenu_pStartPrompt = object;
    if (object != 0) {
        DisplayObject_ConfigureSpriteAtPositionWithResource(object, 0, 8, 5, 0, 1, 0x1A, 1, gMainMenu_DisplayResources);
        gMainMenu_pStartPrompt->attribute |= 0x1000000;
        gMainMenu_pStartPrompt->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                            DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable1(gMainMenu_pStartPrompt);
        third = gMainMenu_pStartPrompt;
        third->field_6C = 0x3C;
        fourth = gMainMenu_pStartPrompt;
        third->field_60 = -2;
        fourth->field_34.h.field_36 = 0;
    }

    for (i = 0; i < 11; i++) {
        entry = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        if (i < 5) {
            y = i * 32 + MAIN_MENU_ENTRY_TOP_Y;
        } else {
            y = (i - 5) * 32 + MAIN_MENU_ENTRY_BOTTOM_Y;
        }
        if (entry != 0) {
            DisplayObject_ConfigureSpriteAtPositionWithResource(entry, 0xA0, y, 0, 0, 0, 0x18, 0, gMainMenu_DisplayResources);
            value = i * 2 | (gMain_bMenuID != i);
            entry->attribute |= 0x1000000;
            entry->flags =
                (entry->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE) &
                ~DISPLAY_OBJECT_FLAG_RENDERABLE;
            DisplayObject_SetResourceVariant((DisplayObjectConfig *)entry, value);
            DisplayObject_SelectOrderingTable1(entry);
            gMain_apMenuEntries[i] = (u8 *)entry;
        } else {
            gMain_apMenuEntries[i] = 0;
        }
    }

    gMainMenu_bBackRequested = 0;
    gMainMenu_bTransitionMode = 0;
    gMainMenu_bShadeLevel = 0;
    if (gMain_bMenuID != 0) {
        state = gMainMenu_pStartPrompt->flags;
        gMainMenu_bShadeLevel = 0x80;
        gMainMenu_pStartPrompt->flags = state & ~DISPLAY_OBJECT_FLAG_RENDERABLE;
    }
    gMainMenu_nFadeDirection = 0;
    gMainMenu_bTransitionActive = 0;
    gMainMenu_bSaveWritePending = 0;
    gMainMenu_bLoadPending = 0;
    gMainMenu_bTradeLoadPending = 0;
    gMainMenu_bDuelLoadPending = 0;
    MainMenu_StartFrontendEntryTransition(0);
    gMain_apFrameServiceCallbacks[0] = MainMenu_DrawFrontendBackground;
    SD_BGMPlayDefaultStep(0x7000);
}

#undef MAIN_MENU_ENTRY_TOP_Y
#undef MAIN_MENU_ENTRY_BOTTOM_Y
