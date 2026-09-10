#ifndef MEMORIES_DECOMP_DUEL_CARD_STAGING_H
#define MEMORIES_DECOMP_DUEL_CARD_STAGING_H

#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_deck_card.h"

/* D_8015C424 is reused for card transfers, GPU readback and duel scratch.
 * These are offset views, not a declaration of one allocation's lifetime. */
#define DUEL_CARD_STAGING_REPLAY_BASE_OFFSET 0x48000

/* Keep the high base separate from the record's small displacement: the
 * matching consumers load object/data/card_id at +0x36B4/+0x36B8/+0x36C0. */
typedef struct {
    u8 pad_0000[0x36B4];
    DuelCardRecord record;
} DuelCardReplayRecordBlock;

typedef struct {
    u8 pad_0000[0x39FC];
    DuelDeckCardRecord record;
} DuelStagedDeckRecordBlock;

typedef struct {
    u8 pad_00000[0x4B9FC];
    DuelDeckCardRecord cards[COMBINED_DECK_SIZE];
} DuelCardStagingDeckView;

#define DUEL_CARD_STAGING_OFFSET(type, member) ((u32)&(((type *)0)->member))
typedef char DuelCardReplayRecordBlock_record_offset_must_be_0x36B4[
    DUEL_CARD_STAGING_OFFSET(DuelCardReplayRecordBlock, record) == 0x36B4
        ? 1 : -1
];
typedef char DuelStagedDeckRecordBlock_record_offset_must_be_0x39FC[
    DUEL_CARD_STAGING_OFFSET(DuelStagedDeckRecordBlock, record) == 0x39FC
        ? 1 : -1
];
typedef char DuelCardStagingDeckView_cards_offset_must_be_0x4B9FC[
    DUEL_CARD_STAGING_OFFSET(DuelCardStagingDeckView, cards) ==
        DUEL_CARD_STAGING_REPLAY_BASE_OFFSET + 0x39FC ? 1 : -1
];
#undef DUEL_CARD_STAGING_OFFSET

/* Preserve the scalar view in the AI exporter and the distinct compiler
 * identity used beside the byte view in the grouped draw-resolution unit. */
#ifdef DUEL_CARD_STAGING_AS_DECK
extern DuelCardStagingDeckView D_8015C424;
#else
extern u8 D_8015C424[];
#endif
#ifdef DUEL_CARD_STAGING_WITH_DECK_ALIAS
extern DuelCardStagingDeckView D_8015C424_cards asm("D_8015C424");
#endif

extern u16 gDuel_awCombinedDeckCardIds[];
extern u16 gDuel_awUniqueDeckCardIds[];

#endif
