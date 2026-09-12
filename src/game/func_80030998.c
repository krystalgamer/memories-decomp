/*
 * `func_80030998` matches all 632 bytes on uniform `gcc_2_8_1_g8_split` after
 * selecting true-width DATA scalars for the active mode and campaign scene
 * index. The array spellings leave ten differing words; changing only the mode
 * leaves three and changing only the scene index leaves nine. Selecting both
 * restores the `$at` store expansions and the shared editor-value register
 * allocation without mixed thresholds or a pinned local. The primary campaign
 * display pointer independently requires its DATA view for the absolute load
 * and store.
 *
 * The scene index's final store is `sb`, not `sh`: the reference's missing
 * `D_8009B27A` is the canonical byte `gCampaignSceneIndex`. `Main_RunCampaign`
 * now selects the same existing DATA spelling through the shared header
 * instead of redeclaring it locally. The debug selector, format strings,
 * dialog object and effect-entry byte at `+0x18` use shared declarations.
 * The diagnostic's original format at `0x80010288` is retained, as are the
 * message-preview cleanup, editor toggle and campaign transition.
 *
 * The canonical rows and six terminal refinement records are unchanged;
 * one post-terminal resolution records the declaration and layout evidence.
 */
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#define CAMPAIGN_PRIMARY_OBJECT_IN_DATA
#define CAMPAIGN_SCENE_INDEX_IN_DATA
#include "../types.h"
#include "../psyq/stdio.h"
#include "main_mode_state.h"
#include "input.h"
#include "frontend_debug_state.h"
#include "frontend_debug_constants.h"
#include "frontend_debug_tables.h"
#include "display_object_brightness.h"
#include "duel_interface_setup.h"
#include "campaign_scene_package.h"
#include "duel_effect_entry_ranges.h"
#include "dialog_choice_state.h"
#include "text_box_lifecycle.h"
#include "func_80039794.h"
#include "input_is_pad1_confirm_pressed.h"
#include "display_object_api.h"
#include "func_8003B6AC.h"
#include "func_8002E3FC.h"
#include "../unmatched.h"
#include "func_80030998.h"

void func_80030998(void)
{
    u8 *textbox;
    s32 result;
    s32 flags;
    u16 selection;

    if ((D_8009B2EB & 0x80) == 0) {
        D_8009B2EB = D_8009B2EB | 0x80;
        func_80030090();
        Campaign_LoadScenePackage(0);
        D_8009B2BC = 0;
        goto select_editor;
    }
    if ((gInput_wPad1Pressed[0] & PAD_BUTTON_SELECT) != 0) {
        D_8009B2BC = D_8009B2BC ^ 1;
    select_editor:
        if (D_8009B2BC == 0) {
            selection = D_8009AF44;
            gDebug_nSceneOrSoundID = selection;
            func_80030250(D_80090CDC, 0x12, 0, 0, 2, 3, 1);
        } else {
            selection = D_8009AF46;
            gDebug_nSceneOrSoundID = selection;
            func_80030250(D_80090CF4, 0x12, 0, 0, 2, 2, 1);
        }
    }

    result = func_80030294();
    flags = D_8009B2EB;
    if ((flags & 0x40) != 0) {
        func_80039794();
        textbox = (u8 *)D_800EB0F8;
        if ((D_8009B2EB & 0x20) == 0) {
            if ((*(u16 *)(textbox + 0x34) & TEXT_BOX_FLAG_DONE) != 0) {
                ((DuelEffectChannel *)textbox)->field_30 =
                    Dialog_OpenChoice((DuelEffectChannel *)textbox);
                D_8009B2EB = D_8009B2EB | 0x20;
            }
        } else if (Input_IsPad1ConfirmPressed() != 0) {
            TextBox_Destroy(textbox);
            func_8004036C(D_8009B2A0);
            D_8009B2EB = D_8009B2EB & 0x9F;
            D_8009B2EA = D_8009B2EA & 0xFE;
        }
    } else if (result != 0) {
        if (result < 0) {
            D_8009B2EB = 0;
            func_800300AC();
            return;
        }
        if (D_8009B2BC == 0) {
            D_8009B2EB = flags | 0x40;
            D_8009AF44 = gDebug_nSceneOrSoundID;
            D_8009B2EA = D_8009B2EA | 1;
            func_8003B6AC(0, 2);
            printf(D_80010288,
                   D_800EB288[gDuelEffect_awEntryRangeBoundaries[0]].field_18);
            TextBox_Create(0, D_8009AF44, 0x10, 0xB0, 0x120, 0x30);
            D_8009B2A0 = func_8002E3FC();
            return;
        }
        selection = gDebug_nSceneOrSoundID;
        D_8009B26C = 2;
        D_8009AF46 = selection;
        gCampaignSceneIndex = selection;
    }
}
