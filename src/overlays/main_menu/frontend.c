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
#include "../../game/display_object_api.h"
#include "../../game/display_object_layout.h"
#include "../../psyq/libgte.h"
#include "entrypoints.h"
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

    object = func_800400AC(func_8004002C(), 2);
    D_80184558 = object;
    if (object != 0) {
        func_800428A8(object, 0, 0, 5, 0, 0, 0x1A, 1, D_801AF800);
        D_80184558->attribute |= 0x1000000;
        D_80184558->flags |= 0x28;
        func_800428EC((u8 *)D_80184558, 0);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_8018455C = object;
    if (object != 0) {
        func_800428A8(object, 0, 8, 5, 0, 2, 0x1A, 1, D_801AF800);
        D_8018455C->attribute |= 0x1000000;
        D_8018455C->flags |= 0x28;
        func_800428EC((u8 *)D_8018455C, 1);
    }

    object = func_800400AC(func_8004002C(), 2);
    D_80184560 = object;
    if (object != 0) {
        func_800428A8(object, 0, 8, 5, 0, 1, 0x1A, 1, D_801AF800);
        D_80184560->attribute |= 0x1000000;
        D_80184560->flags |= 0x28;
        func_80042918(D_80184560);
        third = D_80184560;
        third->field_6C = 0x3C;
        fourth = D_80184560;
        third->field_60 = -2;
        fourth->field_34.h.field_36 = 0;
    }

    for (i = 0; i < 11; i++) {
        entry = func_800400AC(func_8004002C(), 2);
        if (i < 5) {
            y = i * 32 + 50;
        } else {
            y = (i - 5) * 32 + 42;
        }
        if (entry != 0) {
            func_800428A8(entry, 0xA0, y, 0, 0, 0, 0x18, 0, D_801AF800);
            value = i * 2 | (gMain_bMenuID != i);
            entry->attribute |= 0x1000000;
            entry->flags =
                (entry->flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE) &
                ~DISPLAY_OBJECT_FLAG_RENDERABLE;
            func_80040410((DisplayObjectConfig *)entry, value);
            func_80042918(entry);
            gMain_apMenuEntries[i] = (u8 *)entry;
        } else {
            gMain_apMenuEntries[i] = 0;
        }
    }

    D_80184595 = 0;
    D_80184596 = 0;
    D_80184597 = 0;
    if (gMain_bMenuID != 0) {
        state = D_80184560->flags;
        D_80184597 = 0x80;
        D_80184560->flags = state & ~DISPLAY_OBJECT_FLAG_RENDERABLE;
    }
    D_80184598 = 0;
    D_80184599 = 0;
    D_8018459A = 0;
    D_8018459B = 0;
    D_8018459C = 0;
    D_8018459D = 0;
    MainMenu_StartFrontendEntryTransition(0);
    D_800E9DB0[0] = MainMenu_DrawFrontendBackground;
    func_80047314(0x7000);
}
