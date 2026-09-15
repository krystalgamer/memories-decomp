#include "../types.h"
#include "dialog_highlight_choice.h"
#include "duel_effect_update_object_layout.h"
#include "dialog_choice.h"
#include "display_object.h"

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
