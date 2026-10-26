#include "../types.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "text_box_set_rect.h"

#ifndef TEXT_BOX_RECT_CHANNEL_TYPE
#define TEXT_BOX_RECT_CHANNEL_TYPE DuelEffectChannel
#endif

void TextBox_SetRect(int index, int x, int y, int width, int height)
{
    TEXT_BOX_RECT_CHANNEL_TYPE *entry =
        &((TEXT_BOX_RECT_CHANNEL_TYPE *)gText_aChannels)[index];
    entry->rect_x = x;
    entry->rect_y = y;
    entry->rect_width = width;
    entry->rect_height = height;
}
