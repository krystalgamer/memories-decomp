#include "../types.h"
#include "duel_card_object_helpers.h"
#include "display_object_api.h"
#include "card_constants.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"

DisplayObject *func_80018150(int arg0, int arg1)
{
    DisplayObject *object = func_800400AC((s32)func_8004006C(), 2);
    func_800404CC(object, arg0, arg1, 0, 1, 0, 0x1F, 0x100);
    func_80042918(object);
    func_800428EC((u8 *)object, -2);
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
