#include "../types.h"
#include "func_8004036C.h"
#include "duel_card_layout.h"

typedef struct {
    void *value;
    u8 pad4[0x12];
    u16 flags;
} Object;

void func_80024914(Object *object)
{
    object->flags &= ~DUEL_CARD_FLAG_OCCUPIED;
    if (object->value != 0) {
        func_8004036C(object->value);
        object->value = 0;
    }
}

void func_80024954(Object *object)
{
    func_80024914(object);
    object->flags = 0;
}
