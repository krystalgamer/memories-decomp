#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
#include "../types.h"
#include "dialog_highlight_choice.h"
#include "duel_effect_update_object_layout.h"
#include "dialog_choice.h"
#include "display_object.h"
#include "input.h"
#include "sound.h"
#include "dialog_read_choice_input.h"
#include "duel_effect.h"

void Dialog_HighlightChoice(DuelEffectChannel *record)
{
    s32 value;
    DisplayObject *sub = record->field_30;

    if (((gDialog_bChoiceEnabled >> gDialog_bChoice) & 1) == 0) value = 0xC0;
    else value = 0xC0C0;
    sub->field_0C = value;
    record->field_30->field_3C.word = value;
    record->field_30->field_44.word = value;
    value = value / 4;
    record->field_30->field_2C.word = value;
    record->field_30->field_34.word = value;
    record->field_30->field_4C = value;
    *(s32 *)&record->field_30->field_54 = value;
    DuelEffect_UpdateObjectLayout(record);
}

/* Up, down or R1 on the repeat pad moves the dialog's choice cursor. R1
   wraps to the top; the directions clamp and report the press as handled
   without re-playing the cursor sound.

   gInput_wPad1Repeat is volatile: the target reads it once per test rather
   than caching it, and the three reads are what the three branches turn on. */
s32 Dialog_ReadChoiceInput(DuelEffectChannel *record)
{
    if (gInput_wPad1Repeat &
        (PAD_DIRECTION_VERTICAL_MASK | PAD_BUTTON_R1)) {
        if (gInput_wPad1Repeat & PAD_BUTTON_R1) {
            u8 choice = gDialog_bChoice;
            s32 count = gDialog_bChoiceCount;

            gDialog_bChoice = choice + 1;
            if ((s8)(choice + 1) >= count) {
                gDialog_bChoice = 0;
            }
        } else if (gInput_wPad1Repeat & PAD_DIRECTION_DOWN) {
            u8 choice = gDialog_bChoice;
            s32 count = gDialog_bChoiceCount;

            gDialog_bChoice = choice + 1;
            if ((s8)(choice + 1) >= count) {
                gDialog_bChoice = choice;
                return 1;
            }
        } else {
            u8 choice = gDialog_bChoice;

            gDialog_bChoice = choice - 1;
            if ((s8)(choice - 1) < 0) {
                gDialog_bChoice = choice;
                return 1;
            }
        }
        SD_SEPlayFull(6);
        Dialog_HighlightChoice(record);
        return 1;
    }
    return 0;
}
