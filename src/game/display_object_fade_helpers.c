#include "../types.h"
#include "duel_effect.h"
#include "func_80039AD4.h"
#include "display_object_fade.h"

/* The two helpers the display-object fade callbacks
   (display_object_fade_callbacks.c) share: the first-frame latch on the
   channel's byte 0x13, and the release that frees the channel's entry slot
   and requests an entry-list rebuild. */

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_DISPLAY_FADE_MARK_INITIALIZED)
s32 DisplayObjectFade_MarkInitialized(DuelEffectChannel *object)
{
    u8 flags = object->field_13;
    if ((flags & DISPLAY_OBJECT_FADE_FLAG_INITIALIZED) == 0) {
        object->field_13 = flags | DISPLAY_OBJECT_FADE_FLAG_INITIALIZED;
        return 0;
    }
    return 1;
}
#endif

#ifndef VERSION_JAPAN
void DisplayObjectFade_ReleaseChannel(DuelEffectChannel *object)
{
    D_800EAF08[object->field_10] = 0;
    object->field_11 = 0;
    D_8009B330 = 1;
}
#endif
