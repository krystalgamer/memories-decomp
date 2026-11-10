
/* Text channel setup: DuelEffect_ResetState, which clears the four
 * gText_aChannels effect channels, their occupancy table and entry markers,
 * and TextBox_SetRect, which sets one channel's text-box rectangle. In every
 * executable these are one contiguous run of text (0x80035A64-0x80035AF0 in
 * the US build). */
#include "../types.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_entry_occupancy.h"
#include "duel_effect_reset_state.h"
#include "text_box_set_rect.h"

void DuelEffect_ResetState(void)
{
    s32 i = 4;
    DuelEffectChannel *entry = gText_aChannels;

    do {
        entry->flags_34 = 0;
        entry->prompt_object = 0;
        entry->aux_object = 0;
        entry->render_object = 0;
#ifdef VERSION_JAPAN
        entry = (DuelEffectChannel *)((u8 *)entry + 0x60);
#elif defined(VERSION_EUROPE)
        /* The European channel is 100 bytes. */
        entry = (DuelEffectChannel *)((u8 *)entry + 100);
#else
        entry++;
#endif
    } while (--i != 0);
#ifndef VERSION_EUROPE
    /* The European build makes no occupancy reset here. */
    DuelEffect_ResetOccupancy();
#endif
    DuelEffect_ResetEntryMarkers();
}

void TextBox_SetRect(int index, int x, int y, int width, int height)
{
    DuelEffectChannel *entry = &gText_aChannels[index];
    entry->rect_x = x;
    entry->rect_y = y;
    entry->rect_width = width;
    entry->rect_height = height;
}
