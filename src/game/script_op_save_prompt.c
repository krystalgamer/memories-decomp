#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define GDIALOG_CHOICE_IN_DATA
#define GDIALOG_CHOICE_COUNT_IN_DATA
#define D_8009B268_IN_DATA
#define D_8009B26D_IN_DATA
#define SAVE_DATA_WORKSPACE_AS_HALFWORDS
#define MAIN_MODE_STATE_NEXT_IN_DATA
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#include "../types.h"
#include "card_constants.h"
#include "data_transfer_request.h"
#include "duel_effect_mode_7.h"
#include "input.h"
#include "save_data.h"
#include "script_command_busy.h"

#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "text_box_runtime.h"
#include "func_80039794.h"
#include "func_8003B6AC.h"
#include "display_object.h"
#include "display_object_interpolation.h"
#include "script_state.h"
#include "main_services.h"
#define D_8009B34C_IN_DATA
#define GCAMPAIGN_SCENE_INDEX_AS_SCALAR
#include "../unmatched.h"
#include "dialog_read_choice_input.h"
#include "script_op_save_prompt.h"
#include "duel_effect_mark_object_if_active.h"
#include "duel_effect_entry_control.h"
#include "dialog_choice.h"
#include "main_mode_state.h"

#define DISPLAY_OBJECT_X_HALFWORD_INDEX \
    ((u32)&((DisplayObject *)0)->field_30.h.field_30 / sizeof(s16))
#define DISPLAY_OBJECT_Y_HALFWORD_INDEX \
    ((u32)&((DisplayObject *)0)->field_30.h.field_32 / sizeof(s16))

int Duel_IsPlayerDeckComplete(void)
{
    unsigned short *entry = gDuel_awPlayerDeck;
    int i = 0;

    while (i < DECK_SIZE) {
        if (*entry == 0) {
            return 0;
        }
        i++;
        entry++;
    }
    return 1;
}

/* Save-prompt dialog step for the campaign script.
 *
 * On entry it reads the prompt's text id and scene index from the script
 * stream and opens the prompt box. Afterwards it drives the memory-card
 * dialog, the confirm and cancel boxes and the slide-in/out of the prompt,
 * and finally applies the choice read from the pad.
 *
 * Two allocation levers are load-bearing. The choice handling at the end
 * sits in a one-pass do/while: GCC weights the box pointer's references by
 * that loop depth, so the box takes $s0 and its display object $s1, as
 * retail has them. `value` carries both the text id and the prompt's Y
 * coordinate, so it spans blocks and the stream's low byte keeps its own
 * register instead of being tied to the id. */
void Script_OpSavePrompt(void)
{
    DuelEffectChannel *box;
    DisplayObject *obj;
    u8 *p;
    u8 *p2;
    DisplayObject *slot;
    DuelEffectChannel *chan;
    DuelEffectChannel *prompt;
    s32 value;
    s32 lo;
    s32 choice;
    s32 step;
    u16 flags;
    u16 next;

    if (ScriptCommand_MarkStarted() == 0) {
        p = D_8009B290;
        p2 = p + 2;
        gDialog_bChoice = -1;
        D_8009B290 = p2;
        lo = p[0];
        value = lo | (p[1] << 8);
        D_8009B290 = p + 4;
        D_8009B2A6 = p[2] | (p2[1] << 8);
        func_8003B6AC(0, 2);
        DuelEffect_MarkObjectIfActive(
            TextBox_Create(0, value, 0x10, 0xB0, 0x120, 0x30));
        return;
    }

    if ((D_8009B27C & 0x80) != 0) {
        if (MemCardDialog_Poll() == 0) {
            return;
        }
        gDialog_bChoiceCount = 4;
        gDialog_bChoice = 0;
        D_8009B34C = 0;
        D_8009B27C &= 0xFF7F;
        return;
    }

    func_80039794();
    flags = D_8009B27C;
    if ((flags & 0x4000) == 0) {
        chan = D_800EB0F8;
        if ((chan->flags_34 & 0x2000) == 0) {
            return;
        }
        if (DuelEffect_HasActiveEntry(chan) != 0) {
            return;
        }
        D_8009B27C |= 0x4000;
        if (Duel_IsPlayerDeckComplete() == 0) {
            D_8009B27C |= 0x200;
            SD_SEPlayFull(0x2A);
            return;
        }
        func_8003B6AC(0, 2);
        box = TextBox_Create(3, 0x11, -0x90, 0x38, 0x78, 0x30);
        DuelEffect_MarkObjectIfActive((MenuRecord *)box);
        box->flags_34 |= 0x24;
        do {
            func_80039794();
        } while (box->field_30 == 0);
        DisplayObject_SavePosition((DisplayObjectSnapshot *)box->field_28);
        slot = box->field_28;
        next = D_8009B27C | 0x6000;
        slot->field_60 = -0x400;
        D_8009B27C = next;
        return;
    }

    box = &D_800EB0F8[3];
    obj = box->field_28;

    if ((flags & 0x400) != 0) {
        if ((flags & 0x800) == 0) {
            D_8009B27C = flags | 0x800;
            func_8003B6AC(0, 2);
            box = TextBox_Create(2, 0x12, 0x90, 0x70, 0x18, 0x18);
            DuelEffect_MarkObjectIfActive((MenuRecord *)box);
            box->flags_34 |= 0x20;
            do {
                func_80039794();
            } while (box->field_30 == 0);
        }
        prompt = &D_800EB0F8[2];
        if ((prompt->flags_34 & 0x2000) == 0) {
            return;
        }
        D_8009B27C &= 0xF3FF;
        TextBox_Destroy(prompt);
        if (gDialog_bChoice != 0) {
            D_8009B268 = 1;
            D_8009B26D = 5;
            D_8009B26C = 8;
        }
        gDialog_bChoiceCount = 4;
        gDialog_bChoice = 2;
        return;
    }

    if ((flags & 0x200) != 0) {
        if ((flags & 0x800) == 0) {
            D_8009B27C = flags | 0x800;
            func_8003B6AC(0, 2);
            DuelEffect_MarkObjectIfActive(TextBox_CreateFlagged(
                0, 0x1C, 0x10, 0xB0, 0x120, 0x24, 0x1008));
            return;
        }
        if ((D_800EB0F8[0].flags_34 & 8) != 0) {
            return;
        }
        SD_SEPlayFull(8);
        D_8009B268 = 1;
        D_8009B26D = 5;
        D_8009B26C = 8;
        return;
    }

    if ((flags & 0x1000) != 0) {
        if ((flags & 0x800) == 0) {
            D_8009B27C = flags | 0x800;
            DisplayObject_SavePosition((DisplayObjectSnapshot *)obj);
            *(s16 *)&box->field_28->field_60 = 0x400;
        }
        step = *(u16 *)&obj->field_60 - 0x40;
        obj->field_60 = step;
        if ((s16)step <= 0) {
            TextBox_Destroy(box);
            D_8009B27C = 0;
            return;
        }
        Widget_SlideSine((DisplayObjectPosition *)obj, -0x90, 0x38, (s16)step);
        TextBox_SetPos(box, *(s16 *)&obj->field_30.h.field_30,
                       *(s16 *)&obj->field_30.h.field_32);
        return;
    }

    if ((flags & 0x2000) != 0) {
        step = *(u16 *)&obj->field_60 + 0x40;
        obj->field_60 = step;
        if ((s16)step >= 0) {
            *(s16 *)&obj->field_30.h.field_30 = 0x10;
            *(s16 *)&obj->field_30.h.field_32 = 0x38;
            D_8009B27C = flags & 0xDFFF;
            /* These reads keep the halfword-array spelling: direct member
               access costs sixteen bytes, while derived halfword indexes keep
               the accepted allocation tied to the shared layout. */
            value = ((s16 *)obj)[DISPLAY_OBJECT_Y_HALFWORD_INDEX];
            TextBox_SetPos(
                box, ((s16 *)obj)[DISPLAY_OBJECT_X_HALFWORD_INDEX], value
            );
            return;
        }
        Widget_SlideSine((DisplayObjectPosition *)obj, 0x10, 0x38, (s16)step);
        TextBox_SetPos(box, *(s16 *)&obj->field_30.h.field_30,
                       *(s16 *)&obj->field_30.h.field_32);
        return;
    }

    do {
        if (Dialog_ReadChoiceInput(box) != 0) {
            return;
        }
        if ((gInput_wPad1Pressed & 0xC0) == 0) {
            return;
        }
        D_801D0000[(SAVE_DATA_HEADER_SIZE + SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET) /
                  sizeof(s16)] = D_8009B2A6;
        choice = gDialog_bChoice;
        switch (choice) {
        case 0:
            SD_SEPlayFull(7);
            D_801D0000[(SAVE_DATA_HEADER_SIZE + SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET) /
                      sizeof(s16)] = D_8009B2A6;
            SaveData_RequestWrite();
            D_8009B27C |= 0x80;
            break;
        case 1:
            SD_SEPlayFull(7);
            func_80033C90();
            D_8009B269 = 2;
            gCampaignSceneIndex = (u8)D_8009B2A6;
            break;
        case 2:
            SD_SEPlayFull(7);
            D_8009B27C |= 0x400;
            break;
        case 3:
            SD_SEPlayFull(8);
            D_8009B27C |= 0x1000;
            break;
        }
    } while (0);
}
