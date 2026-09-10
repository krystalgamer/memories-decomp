#include "../types.h"
#include "duel_effect.h"
#include "display_object_fade.h"

s32 DisplayObjectFade_MarkInitialized(DuelEffectChannel *object)
{
    u8 flags = object->field_13;
    if ((flags & DISPLAY_OBJECT_FADE_FLAG_INITIALIZED) == 0) {
        object->field_13 = flags | DISPLAY_OBJECT_FADE_FLAG_INITIALIZED;
        return 0;
    }
    return 1;
}
