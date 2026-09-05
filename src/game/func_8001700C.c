#include "../types.h"
#include "duel_card_layout.h"

typedef struct {
    u8 pad_00[0x16];
    u16 flags;
} Object;

int func_8001700C(Object *object)
{
    u16 flags = object->flags;

    if (flags & DUEL_CARD_FLAG_OCCUPIED) {
        if (!(flags & DUEL_CARD_FLAG_USED_THIS_TURN)) {
            return 1;
        }
    }
    return 0;
}
