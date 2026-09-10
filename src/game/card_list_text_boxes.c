#include "../types.h"
#include "card_constants.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "duel_effect.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "card_list_text_boxes.h"

/* Builds the text box for one card slot of a trade or deck screen. The entry
   is picked out of the list at the scroll offset plus the slot, and the list
   kind at +0x2D47 selects both the box template and a fixed 0x160 shift. */

void func_80031CD4(CardList *list, s32 slot)
{
    DuelEffectChannel *box;
    s32 style;

    gDuel_wSelectedCardID = list->entries[list->first + slot].id;
    style = 0;
    if (list->entries[list->first + slot].flags != 0) {
        style = 6;
    }
    box = TextBox_Create(list->kind + 1, style, 0x22, 0x2B, 0x120, 0xB0);
    box->field_3A = slot * 22;
    ((DisplayObject *)box->field_28)->flags &=
        ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    if ((list->entries[list->first + slot].flags & 0x80) != 0) {
        box->field_54 = 4;
    }
    if (list->kind != 0) {
        *(u16 *)&box->field_3C += 0x160;
    }
    if (slot != 0) {
        box->flags_34 |= 0x40;
    }
    func_80039A14((u8 *)box);
}

void func_80031E04(CardList *list, s32 count)
{
    s32 i;

    for (i = 0; i < count; i++)
        func_80031CD4(list, i);
}
