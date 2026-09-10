#ifndef MEMORIES_DECOMP_DUEL_CARD_ICON_SETUP_H
#define MEMORIES_DECOMP_DUEL_CARD_ICON_SETUP_H

#include "../types.h"
#include "duel_card.h"
#include "duel_card_display_state.h"

/* View of one replay/card-data block at D_8015C424 + 0x48000. Keeping the
 * leading pad as a member is codegen-significant: retail materializes the
 * block base and reaches record.data with the 0x36B8 load offset. */
typedef struct {
    u8 pad_0000[0x36B4];
    DuelCardRecord record;
} DuelCardReplayRecordBlock;

DuelCardDisplayObject *func_80024C1C(s32 card_id, s32 x, s32 y);
void func_80024D34(s32 slot, s32 card_id);

#endif
