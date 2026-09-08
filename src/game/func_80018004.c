#include "../types.h"
#include "duel_side_state.h"
#include "duel_card_layout.h"

typedef struct Child { char p[4]; u8 field_04; } Child;
typedef struct Object {
    char p0[4]; Child *child; char p8[0xE]; u16 flags;
    char p18[0x4F]; u8 field_67;
} Object;
extern Object *func_80017F04(Object *);

Object *func_80018004(Object *object)
{
    Object *result = func_80017F04(object);
    if (D_8009B1C8->field_1F != 0) {
        object->flags |= DUEL_CARD_FLAG_DISPLAY_MARKER;
        result->field_67 = object->child->field_04 + 1;
        if (D_8009B1C8->field_1F < 0) result->field_67 = 0xFF;
    }
    return result;
}
