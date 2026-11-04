#include "../types.h"
#include "display_object.h"
#include "display_object_config.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "duel_create_card_effect_overlay.h"

/* Regional value: the sprite's height argument and the half of it stored
 * beside (0xC4 -> 0xD4). The European build (src/game/european/) defines its
 * own. */
#ifndef DUEL_CARD_EFFECT_OVERLAY_HEIGHT
#define DUEL_CARD_EFFECT_OVERLAY_HEIGHT 0xC4
#endif

DisplayObject *Duel_CreateCardEffectOverlay(DisplayObjectConfigView *source)
{
    DisplayObject *object =
        (DisplayObject *)DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);

    DisplayObject_ConfigureScreenSprite(
        object,
        source->x,
        source->y,
        0x8C,
        DUEL_CARD_EFFECT_OVERLAY_HEIGHT,
        0,
        0,
        0x15,
        0,
        0
    );
    object->field_18.center.x = 0x46;
    object->field_48.pivot.x = 0x46;
    object->field_18.center.y = DUEL_CARD_EFFECT_OVERLAY_HEIGHT / 2;
    object->field_48.pivot.y = DUEL_CARD_EFFECT_OVERLAY_HEIGHT / 2;
    DisplayObject_SelectOrderingTable1(object);
    object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_16BPP;
    return object;
}
