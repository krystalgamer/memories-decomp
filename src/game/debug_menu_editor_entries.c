#define FRONTEND_DEBUG_ROW_VIEWS
#define D_8009AF4C_IS_AGGREGATE
#define GINPUT_PAD1_HELD_SIZED_IN_DATA_VOLATILE
#define GINPUT_PAD2_HELD_SIZED_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_SIZED_IN_DATA_VOLATILE
#define GINPUT_PAD2_PRESSED_SIZED_IN_DATA_VOLATILE
#define GINPUT_PAD1_REPEAT_SIZED_IN_DATA_VOLATILE
#define GINPUT_PAD2_REPEAT_SIZED_IN_DATA_VOLATILE
#define FRONTEND_DEBUG_SOUND_ROW_VIEW
#define CAMPAIGN_PRIMARY_OBJECT_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#include "../types.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "frontend_debug_tables.h"
#include "frontend_debug_state.h"
#include "frontend_step_tables.h"
#include "display_object_brightness.h"
#include "duel_interface_setup.h"
#include "file_transfer.h"
#include "input.h"
#include "sound.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"
#include "../psyq/stdio.h"
#include "frontend_debug_constants.h"
#include "campaign_scene_package.h"
#include "duel_effect_entry_ranges.h"
#include "dialog_choice_state.h"
#include "text_box_lifecycle.h"
#include "func_80039794.h"
#include "input_is_pad1_confirm_pressed.h"
#include "display_object_core.h"
#include "duel_effect_set_entry_range_layout.h"
#include "campaign_create_primary_display_object.h"
#include "../unmatched.h"
#include "debug_menu_editor_entries.h"
#include "main_mode_state.h"
#include "debug_font_format_data.h"

/*
 * The union row view prevents GCC from coalescing the first computed row
 * address with its symbol base. The address stays in v0 and is copied to the
 * long-lived pointer, while the second arm's direct index keeps its one-use
 * address in v0. Sized volatile .data views keep all six pad names absolute
 * under the uniform G8 compiler and assembler profile.
 */
/* The confirm buttons (Cross|Square). The Japanese release swaps Cross and
 * Circle, so a regional build names its own. */
#ifndef DEBUG_SCREEN_CONFIRM_MASK
#define DEBUG_SCREEN_CONFIRM_MASK 0xC0
#endif

/* The cancel button (Circle), swapped the same way. */
#ifndef DEBUG_SCREEN_CANCEL_BUTTON
#define DEBUG_SCREEN_CANCEL_BUTTON 0x20
#endif

/* gDebug_nSceneOrSoundID heads the three FrontendDebugValues rows. */
#define FRONTEND_DEBUG_VALUES (*(FrontendDebugValues *)&gDebug_nSceneOrSoundID)

s32 DebugMenu_UpdateValueEditor(void)
{
    s32 a[5] = {1, 10, 100, 1000, 10000};
    s32 b[4] = {0xA, 0x9A, 0x99A, 0x999A};
    s32 c[5] = {1, 0x10, 0x100, 0x1000, 0x10000};
    u8 d[8];
    s32 ret;
    s32 i;
    s32 n;
    s32 e;
    s32 step;
    u8 *db;
    s32 k;
    u8 *z;
    u16 *p;
    FrontendDebugRowPointer row;
    s32 val;
    s32 mask;
    u8 t2;
    u8 f;
    u8 *r;
    s32 sc;
    s32 jj;
    s32 cc;
    s32 qq;

    ret = 0;
    *(Bytes8 *)d = *(Bytes8 *)D_8009AF4C;
    db = d;
    t2 = tent_DebugInterfaceFieldC0[gDebug_bEditorRow];
    if ((tent_DebugInterfaceFlags & 0x80) == 0) {
        tent_DebugInterfaceFlags = tent_DebugInterfaceFlags | 0x80;
        if ((tent_DebugInterfaceFlags & 0x40) != 0) {
            jj = (s8)t2 - 1;
            row.words = &FRONTEND_DEBUG_VALUES.row[gDebug_bEditorRow];
            val = *row.words;
            *row.words = 0;
            p = row.words;
            do {
                k = a[jj];
                qq = val / k;
                *p = *p | (qq << (jj * 4));
                jj = jj - 1;
                val = val - k * qq;
            } while (jj >= 0);
        }
        goto fill;
    }
    if ((tent_DebugInterfaceFlags & 1) != 0) {
        goto fill;
    }
    if (((gInput_wPad1Held[0] | gInput_wPad2Held[0]) & 0x800) != 0) {
        goto fill;
    }
    if (((gInput_wPad1Pressed[0] | gInput_wPad2Pressed[0]) & DEBUG_SCREEN_CONFIRM_MASK) != 0) {
        ret = 1;
        goto out;
    }
    if (((gInput_wPad1Pressed[0] | gInput_wPad2Pressed[0]) & DEBUG_SCREEN_CANCEL_BUTTON) != 0) {
        ret = -1;
        goto out;
    }
    if (((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0x5000) != 0) {
        e = gDebug_bEditorDigit;
        val = FRONTEND_DEBUG_VALUES.row[gDebug_bEditorRow];
        step = c[e];
        if (((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0x4000) != 0) {
            step = -step;
        }
        if ((tent_DebugInterfaceFlags & 0x40) != 0) {
            /* A u16 cursor folds the negation and swaps the addu operands. */
            mask = *(u16 *)(db - -(e * 2));
            i = e;
            if (i < (s8)t2) {
                n = (s8)t2;
                do {
                    cc = val & mask;
                    val = val & ~mask;
                    cc = cc + step;
                    if (step >= 0) {
                        if (cc < b[i]) {
                            goto joined;
                        }
                    } else {
                        if (cc >= 0) {
                            goto joined;
                        }
                        val = val | ((b[i] - 1) & mask);
                    }
                    cc = 0;
                    mask = mask * 0x10;
                    step = step * 0x10;
                    i = i + 1;
                } while (i < n);
            }
        joined:
            val = val | cc;
        } else {
            val = val + step;
            val = val & (c[(s8)t2] - 1);
        }
        FRONTEND_DEBUG_VALUES.row[gDebug_bEditorRow] = val;
    }
    if (((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0xA000) == 0) {
        goto out;
    }
    if (((gInput_wPad1Repeat[0] | gInput_wPad2Repeat[0]) & 0x2000) != 0) {
        gDebug_bEditorDigit = gDebug_bEditorDigit - 1;
        if (gDebug_bEditorDigit < 0) {
            gDebug_bEditorRow = gDebug_bEditorRow + 1;
            if (gDebug_bEditorRow >= (s8)tent_DebugInterfaceFieldE0) {
                gDebug_bEditorRow = tent_DebugInterfaceFieldE0 - 1;
                gDebug_bEditorDigit = 0;
                goto fill;
            }
            f = tent_DebugInterfaceFieldC0[gDebug_bEditorRow] - 1;
            goto setpos;
        }
    } else {
        gDebug_bEditorDigit = gDebug_bEditorDigit + 1;
        if (gDebug_bEditorDigit >= (s8)t2) {
            gDebug_bEditorDigit = 0;
            gDebug_bEditorRow = gDebug_bEditorRow - 1;
            if (gDebug_bEditorRow < 0) {
                f = tent_DebugInterfaceFieldC0[0] - 1;
                gDebug_bEditorRow = 0;
            setpos:
                gDebug_bEditorDigit = f;
            }
        }
    }
fill:
    r = D_800EAED8;
    sc = 0x20;
    i = 0x27;
    z = r + i;
    do {
        *z = sc;
        i = i - 1;
        z = z - 1;
    } while (i >= 0);
    r = &D_800EAED8[
        tent_DebugInterfaceFieldB4[gDebug_bEditorRow] - gDebug_bEditorDigit];
    r[0] = 0x2A;
    r[1] = 0;
out:
    i = (s8)tent_DebugInterfaceFieldB8;
    if (i != 0) {
        do {
            FntPrint(D_8009AF54);
            i = i - 1;
        } while (i != 0);
    }
    FntPrint((char *)tent_DebugInterfaceData, gDebug_nSceneOrSoundID,
             gDebug_nEditedBgmId, gDebug_nEditedCdSoundId);
    FntPrint(D_8009AF58, D_800EAED8);
    return ret;
}

#ifdef VERSION_EUROPE
#include "duel_effect_resource_setup.h"
#endif

#ifdef VERSION_EUROPE
/* A European-only debug-menu entry, with its own slot in the menu's handler
   table: it edits D_8009C02B (the index duel_effect_resource_setup.c
   reads) through the same editor as the sound entry, and sets it back to
   BUILD_LANGUAGE_INDEX from 5 up. */
void func_80030808(void)
{
    s32 flags;
    s32 result;

    flags = gDebugMenu_bStep;
    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        DebugMenu_DimPrimaryDisplayObject();
        gDebug_nSceneOrSoundID = D_8009C02B;
        DebugMenu_OpenValueEditor(D_80091CB8, 0x15, 0x15, 0x15, 5, 1, 1);
        tent_DebugInterfaceFieldC2 = 3;
        tent_DebugInterfaceFieldC1 = 3;
        return;
    }

    result = DebugMenu_UpdateValueEditor();
    if (result == 0) {
        return;
    }
    if (result < 0) {
        gDebugMenu_bStep = 0;
        DebugMenu_RestorePrimaryDisplayObject();
        return;
    }
    if (D_8009C02B != gDebug_nSceneOrSoundID) {
        D_8009C02B = gDebug_nSceneOrSoundID;
        if (D_8009C02B >= 5) {
            D_8009C02B = BUILD_LANGUAGE_INDEX;
        }
    }
}
#endif

void DebugMenu_UpdateSoundEntry(void)
{
    s32 flags;
    s32 count;
    s32 result;

    flags = gDebugMenu_bStep;
    if ((flags & FRONTEND_STEP_FLAG_ENTERED) == 0) {
        gDebugMenu_bStep = flags | FRONTEND_STEP_FLAG_ENTERED;
        count = 3;
        DebugMenu_DimPrimaryDisplayObject();
        gDebug_nSceneOrSoundID = gDebug_nLastSoundID[0];
        gDebug_nEditedBgmId = gDebug_nLastBgmId;
        gDebug_nEditedCdSoundId = gDebug_nLastSoundID[1];
        DebugMenu_OpenValueEditor(D_80090CB4, 0x11, 0x19, 0x21, 9, 4, count);
        tent_DebugInterfaceFieldC2 = count;
        tent_DebugInterfaceFieldC1 = count;
        return;
    }

    if ((gInput_wPad1Pressed[0] & PAD_BUTTON_START) != 0) {
        SD_StopAll();
        return;
    }

    if ((gInput_wPad1Pressed[0] & PAD_BUTTON_SELECT) != 0) {
        File_CancelTransfers();
        return;
    }

    result = DebugMenu_UpdateValueEditor();
    if (result == 0) {
        return;
    }
    if (result < 0) {
        gDebugMenu_bStep = 0;
        DebugMenu_RestorePrimaryDisplayObject();
        return;
    }

    switch (gDebug_bEditorRow) {
    case 0:
        gDebug_nLastSoundID[0] = gDebug_nSceneOrSoundID;
        if (gDebug_bEditorDigit == 3) {
            SD_ResetPendingEntries();
            SD_ReplaceValueLink((u16)((s16)gDebug_nLastSoundID[0] >> 12));
            return;
        }
        SD_SEPlayFull(gDebug_nSceneOrSoundID & 0xFFF);
        return;
    case 1:
        gDebug_nLastBgmId = gDebug_nEditedBgmId;
        SD_BGMPlay(gDebug_nEditedBgmId);
        return;
    case 2:
        gDebug_nLastSoundID[1] = gDebug_nEditedCdSoundId;
        if ((gInput_wPad1Pressed[0] & PAD_BUTTON_SQUARE) != 0) {
            SD_CdSoundSeek((u16)gDebug_nEditedCdSoundId);
            return;
        }
        SD_CdSoundPlay(gDebug_nEditedCdSoundId);
        return;
    }
}

/*
 * `DebugMenu_UpdateCampaignEntry` matches all 632 bytes on uniform `gcc_2_8_1_g8_split` after
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
 * now selects the same existing DATA spelling through unmatched.h
 * instead of redeclaring it locally. The debug selector, format strings,
 * dialog object and effect-entry byte at `+0x18` use shared declarations.
 * The diagnostic's original format at `0x80010288` is retained, as are the
 * message-preview cleanup, editor toggle and campaign transition.
 *
 * The canonical rows and six terminal refinement records are unchanged;
 * one post-terminal resolution records the declaration and layout evidence.
 */
/* Regional value: the preview text box's height (0x30 -> 0x40). The
 * European build (src/game/european/) defines its own. */
#ifndef DEBUG_MENU_CAMPAIGN_TEXT_BOX_HEIGHT
#define DEBUG_MENU_CAMPAIGN_TEXT_BOX_HEIGHT 0x30
#endif

void DebugMenu_UpdateCampaignEntry(void)
{
    DuelEffectChannel *textbox;
    s32 result;
    s32 flags;
    u16 selection;

    if ((gDebugMenu_bStep & 0x80) == 0) {
        gDebugMenu_bStep = gDebugMenu_bStep | 0x80;
        DebugMenu_DimPrimaryDisplayObject();
        Campaign_LoadScenePackage(0);
        gDebugMenu_bCampaignEditField = 0;
        goto select_editor;
    }
    if ((gInput_wPad1Pressed[0] & PAD_BUTTON_SELECT) != 0) {
        gDebugMenu_bCampaignEditField = gDebugMenu_bCampaignEditField ^ 1;
    select_editor:
        if (gDebugMenu_bCampaignEditField == 0) {
            selection = D_8009AF44;
            gDebug_nSceneOrSoundID = selection;
            DebugMenu_OpenValueEditor(D_80090CDC, 0x12, 0, 0, 2, 3, 1);
        } else {
            selection = D_8009AF46;
            gDebug_nSceneOrSoundID = selection;
            DebugMenu_OpenValueEditor(D_80090CF4, 0x12, 0, 0, 2, 2, 1);
        }
    }

    result = DebugMenu_UpdateValueEditor();
    flags = gDebugMenu_bStep;
    if ((flags & 0x40) != 0) {
        TextBox_UpdateChannels();
        textbox = D_800EB0F8;
        if ((gDebugMenu_bStep & 0x20) == 0) {
            if ((textbox->flags_34 & TEXT_BOX_FLAG_DONE) != 0) {
                textbox->field_30 = Dialog_OpenChoice(textbox);
                gDebugMenu_bStep = gDebugMenu_bStep | 0x20;
            }
        } else if (Input_IsPad1ConfirmPressed() != 0) {
            TextBox_Destroy(textbox);
            DisplayObject_ReleaseIfPresent(gCampaign_PrimaryDisplayObject);
            gDebugMenu_bStep = gDebugMenu_bStep & 0x9F;
            tent_DebugInterfaceFlags = tent_DebugInterfaceFlags & 0xFE;
        }
    } else if (result != 0) {
        if (result < 0) {
            gDebugMenu_bStep = 0;
            DebugMenu_RestorePrimaryDisplayObject();
            return;
        }
        if (gDebugMenu_bCampaignEditField == 0) {
            gDebugMenu_bStep = flags | 0x40;
            D_8009AF44 = gDebug_nSceneOrSoundID;
            tent_DebugInterfaceFlags = tent_DebugInterfaceFlags | 1;
#if !defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)
            /* The Japanese and European builds make neither this call nor
               the diagnostic print. */
            func_8003B6AC(0, 2);
            printf(D_80010288,
                   tent_DuelEffectEntries[gDuelEffect_awEntryRangeBoundaries[0]].field_18);
#endif
            TextBox_Create(0, D_8009AF44, 0x10, 0xB0, 0x120,
                           DEBUG_MENU_CAMPAIGN_TEXT_BOX_HEIGHT);
            gCampaign_PrimaryDisplayObject = Campaign_CreatePrimaryDisplayObject();
            return;
        }
        selection = gDebug_nSceneOrSoundID;
        D_8009B26C = 2;
        D_8009AF46 = selection;
        gCampaignSceneIndex = selection;
    }
}
