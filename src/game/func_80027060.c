#include "../types.h"
#include "ai.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_field_equip_search.h"

s32 func_80027060(void) {
    DuelCardRecord *sp10[6];
    s32 slot;
    s8 v;

    if (Duel_CollectFieldCardsByType(sp10, 0, CARD_TYPE_TRAP) == 0) {
        return 1;
    }
    slot = func_80026C0C(DUEL_FIELD_SIDE_ZONE_COUNT);
    if (slot < 0) {
        return 1;
    }
    v = sp10[0]->table_index;
    D_800EAE88.random = 1;
    D_800EAE88.field1 = 0;
    D_800EAE88.zero = 0;
    D_800EAE88.value = slot % DUEL_FIELD_ROW_SIZE + 6;
    D_800EAE88.result = v % DUEL_FIELD_ROW_SIZE + 0xB;
    return 0;
}
