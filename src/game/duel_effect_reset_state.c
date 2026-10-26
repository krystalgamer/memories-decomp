#include "../types.h"
#include "duel_effect.h"
#include "text_channel_state.h"
#include "duel_effect_entry_occupancy.h"
#include "duel_effect_reset_state.h"

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
