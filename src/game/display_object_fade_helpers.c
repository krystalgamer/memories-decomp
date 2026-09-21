#include "../types.h"
#include "duel_effect.h"
#include "func_80039AD4.h"
#include "display_object_fade.h"

/* Frees the channel's entry slot and requests an entry-list rebuild. */
void DisplayObjectFade_ReleaseChannel(DuelEffectChannel *object)
{
    D_800EAF08[object->field_10] = 0;
    object->field_11 = 0;
    D_8009B330 = 1;
}
