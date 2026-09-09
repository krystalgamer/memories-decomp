#include "../types.h"
#include "duel_card.h"
#include "duel_card_object_cleanup.h"
#include "duel_card_layout.h"
#include "display_object_api.h"

void func_80024914(DuelCardRecord *object)
{
    object->flags &= ~DUEL_CARD_FLAG_OCCUPIED;
    if (object->object != 0) {
        func_8004036C(object->object);
        object->object = 0;
    }
}

void func_80024954(DuelCardRecord *object)
{
    func_80024914(object);
    object->flags = 0;
}
