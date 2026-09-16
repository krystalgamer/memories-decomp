#include "../types.h"
#include "display_object.h"
#include "display_object_config.h"
#include "display_object_core.h"
#include "display_object_helpers.h"
#include "duel_create_card_effect_overlay.h"

DisplayObject *Duel_CreateCardEffectOverlay(DisplayObjectConfigView *source)
{
    DisplayObject *object =
        (DisplayObject *)func_800400AC(DisplayObject_FindFreeGeneralSlot(), 1);

    DisplayObject_ConfigureScreenSprite(
        (DisplayObjectConfigView *)object,
        source->x,
        source->y,
        0x8C,
        0xC4,
        0,
        0,
        0x15,
        0,
        0
    );
    object->field_18 = 0x46;
    object->field_48.h.field_48 = 0x46;
    object->field_1A = 0x62;
    object->field_48.h.field_4A = 0x62;
    func_80042918(object);
    object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_16BPP;
    return object;
}
