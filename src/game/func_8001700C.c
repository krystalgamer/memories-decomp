#include "../types.h"
#include "duel_card.h"
#include "duel_card_layout.h"

int func_8001700C(DuelCardRecord *object)
{
    u16 flags = object->flags;

    if (flags & DUEL_CARD_FLAG_OCCUPIED) {
        if (!(flags & DUEL_CARD_FLAG_USED_THIS_TURN)) {
            return 1;
        }
    }
    return 0;
}
