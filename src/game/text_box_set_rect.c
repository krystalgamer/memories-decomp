#include "../types.h"
#include "duel_effect.h"
#include "text_box_set_rect.h"

#ifndef TEXT_BOX_RECT_CHANNEL_TYPE
#define TEXT_BOX_RECT_CHANNEL_TYPE DuelEffectChannel
#endif

void TextBox_SetRect(int index, int x, int y, int width, int height)
{
    TEXT_BOX_RECT_CHANNEL_TYPE *entry =
        &((TEXT_BOX_RECT_CHANNEL_TYPE *)D_800EB0F8)[index];
    entry->field_3C = x;
    entry->field_40 = y;
    entry->field_3E = width;
    entry->field_42 = height;
}
