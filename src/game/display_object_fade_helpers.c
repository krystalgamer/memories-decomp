#include "../types.h"
#include "duel_effect.h"
#include "func_80039AD4.h"
#include "display_object_fade.h"
#ifdef VERSION_EUROPE
#include "duel_effect_entry_occupancy.h"
#endif

/* The two helpers the display-object fade callbacks
   (display_object_fade_callbacks.c) share: the first-frame latch on the
   channel's byte 0x13, and the release that frees the channel's entry slot
   and requests an entry-list rebuild. */

#ifdef VERSION_EUROPE
/* The European channel is laid out as a EuropeanDuelEffectEntry: the latch
   byte sits at 0x11 and the occupied byte at 0xF. */
#define FADE_LATCH(object) (((EuropeanDuelEffectEntry *)(object))->field_13)
#define FADE_OCCUPIED(object) (((EuropeanDuelEffectEntry *)(object))->flags_11)
#else
#define FADE_LATCH(object) ((object)->field_13)
#define FADE_OCCUPIED(object) ((object)->field_11)
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DISPLAY_FADE_MARK_INITIALIZED)
s32 DisplayObjectFade_MarkInitialized(DuelEffectChannel *object)
{
    u8 flags = FADE_LATCH(object);
    if ((flags & DISPLAY_OBJECT_FADE_FLAG_INITIALIZED) == 0) {
        FADE_LATCH(object) = flags | DISPLAY_OBJECT_FADE_FLAG_INITIALIZED;
        return 0;
    }
    return 1;
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DISPLAY_FADE_RELEASE_CHANNEL)
void DisplayObjectFade_ReleaseChannel(DuelEffectChannel *object)
{
#ifndef VERSION_EUROPE
    /* The European build does not clear the occupancy value, as in
       TextBox_Destroy. */
    tent_DuelEffectOccupancy[object->field_10] = 0;
#endif
    FADE_OCCUPIED(object) = 0;
    D_8009B330 = 1;
}
#endif
