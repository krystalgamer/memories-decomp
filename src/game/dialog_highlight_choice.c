#include "../types.h"
#include "dialog_highlight_choice.h"
#include "display_object.h"
#include "duel_effect_update_object_layout.h"
#include "dialog_choice.h"

void Dialog_HighlightChoice(u8 *record)
{
    s32 value;
    DisplayObject *sub = *(DisplayObject **)(record + 0x30);

    if (((gDialog_bChoiceEnabled >> gDialog_bChoice) & 1) == 0) value = 0xC0;
    else value = 0xC0C0;
    sub->field_0C = value;
    /* Caching field_30 across these stores changes the exact register flow. */
    ((DisplayObject *)*(u8 **)(record + 0x30))->field_3C.word = value;
    ((DisplayObject *)*(u8 **)(record + 0x30))->field_44.word = value;
    value = value / 4;
    ((DisplayObject *)*(u8 **)(record + 0x30))->field_2C.word = value;
    ((DisplayObject *)*(u8 **)(record + 0x30))->field_34.word = value;
    ((DisplayObject *)*(u8 **)(record + 0x30))->field_4C = value;
    *(s32 *)&((DisplayObject *)*(u8 **)(record + 0x30))->field_54 = value;
    DuelEffect_UpdateObjectLayout((DuelEffectChannel *)record);
}
