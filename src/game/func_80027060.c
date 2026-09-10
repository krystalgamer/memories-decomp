#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_field_equip_search.h"

extern u8 D_800EAE88[];

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
    D_800EAE88[8] = 1;
    D_800EAE88[1] = 0;
    D_800EAE88[7] = 0;
    D_800EAE88[6] = slot % DUEL_FIELD_ROW_SIZE + 6;
    D_800EAE88[0] = v % DUEL_FIELD_ROW_SIZE + 0xB;
    return 0;
}
