#include "../types.h"
#include "ai.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_field_effects.h"
#include "../unmatched.h"

s32 Duel_SelectTrapPlay(void) {
    DuelCardRecord *sp10[6];
    s32 slot;
    s8 v;

    if (Duel_CollectFieldCardsByType(sp10, 0, CARD_TYPE_TRAP) == 0) {
        return 1;
    }
    slot = Duel_FindFreeFieldSlot(DUEL_FIELD_SIDE_ZONE_COUNT);
    if (slot < 0) {
        return 1;
    }
    v = sp10[0]->table_index;
    gAi_Selection.bytes[8] = 1;
    gAi_Selection.bytes[1] = 0;
    gAi_Selection.bytes[7] = 0;
    gAi_Selection.bytes[6] = slot % DUEL_FIELD_ROW_SIZE + 6;
    gAi_Selection.bytes[0] = v % DUEL_FIELD_ROW_SIZE + 0xB;
    return 0;
}
