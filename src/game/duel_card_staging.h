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

/* The thirty field records end exactly where the staged deck begins. */
typedef struct {
    u8 pad_00000[0x4B6B4];
    DuelCardRecord field_cards[DUEL_CARD_RECORD_COUNT];
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
typedef char DuelCardStagingDeckView_field_cards_offset_must_be_0x4B6B4[
    DUEL_CARD_STAGING_OFFSET(DuelCardStagingDeckView, field_cards) ==
        DUEL_CARD_STAGING_REPLAY_BASE_OFFSET + 0x36B4 ? 1 : -1
];
typedef char DuelCardStagingDeckView_size_must_preserve_deck_extent[
    sizeof(DuelCardStagingDeckView) ==
        0x4B9FC + COMBINED_DECK_SIZE * sizeof(DuelDeckCardRecord) ? 1 : -1
];
#undef DUEL_CARD_STAGING_OFFSET

/* One byte view and one typed view, each under a single name and neither
 * guarded. The byte view is what the raw-buffer consumers take -- image
 * blocks, GPU readback, ID staging lists and trap scratch, none of which are
 * fields of a record.
 *
 * The typed view is an asm alias of the same address rather than a second
 * spelling of the base name. What the addressing needs is a struct-member
 * offset on a symbol declared as the struct: the symbol's own address is
 * materialised `lui %hi` / `addiu %lo`, the 0x48000 part of the member
 * offset is built separately as `lui 0x4` / `ori 0x8000` and added to it,
 * and the remaining 0x39FC stays as the load's displacement. Both typed
 * consumers show that shape -- `lh 0x39FC` at 0x8002805C in func_80027DF8
 * (gcc_2_8_1_g0_split) and at 0x80018D6C in duel_draw_resolution's
 * func_80018CF8 (gcc_2_8_1_g8_split) -- and the symbol's C name is no part
 * of it, so one alias serves both. Neither profile changed and the retail
 * SHA-256 is unchanged. The alias introduces no second linker symbol:
 * objdump -t on the exporter's object lists one undefined D_8015C424. */
extern u8 D_8015C424[];
extern DuelCardStagingDeckView D_8015C424_cards asm("D_8015C424");

extern u16 gDuel_awCombinedDeckCardIds[];
extern u16 gDuel_awUniqueDeckCardIds[];

#endif
