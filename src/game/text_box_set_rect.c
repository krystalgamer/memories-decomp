#include "../types.h"
#include "duel_effect.h"
#include "text_box_set_rect.h"

void TextBox_SetRect(int index, int x, int y, int width, int height)
{
    DuelEffectChannel *entry = &D_800EB0F8[index];
    entry->field_3C = x;
    entry->field_40 = y;
    entry->field_3E = width;
    entry->field_42 = height;
}
