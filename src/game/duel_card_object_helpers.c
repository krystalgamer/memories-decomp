#include "../types.h"
#include "duel_card_object_helpers.h"
#include "display_object_core.h"
#include "card_constants.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"

DisplayObject *func_80018150(int arg0, int arg1)
{
    DisplayObject *object =
        DisplayObject_AcquireSlot((s32)DisplayObject_FindFreeSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(object, arg0, arg1, 0, 1, 0, 0x1F, 0x100);
    DisplayObject_SelectOrderingTable1(object);
    DisplayObject_SetDepthOffset((u8 *)object, -2);
    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    return object;
}

int func_800181EC(CardObject *object)
{
    int value = 1;
    switch (object->type) {
    case CARD_TYPE_MAGIC:
    case CARD_TYPE_EQUIP:
        value = 2;
        break;
    case CARD_TYPE_TRAP:
        value = 3;
        break;
    case CARD_TYPE_RITUAL:
        value = 4;
        break;
    }
    if (object->flag != 0) value |= 0x80;
    return value;
}
