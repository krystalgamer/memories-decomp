#ifndef MEMORIES_DECOMP_DUEL_CARD_STAGING_H
#define MEMORIES_DECOMP_DUEL_CARD_STAGING_H

#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_deck_card.h"

/* gDuel_abCardStagingBuffer is reused for card transfers, GPU readback and duel scratch.
 * These are offset views, not a declaration of one allocation's lifetime. */
#define DUEL_CARD_STAGING_REPLAY_BASE_OFFSET 0x48000
#define DUEL_CARD_STAGING_TRAP_BASE_OFFSET 0x18000
#define DUEL_CARD_STAGING_TRAP_RECORD_INDEX_BIAS 0x10

/* The word view preserves retail's +16 slot bias for record indices. */
typedef union {
    struct {
        u16 card_ids[DUEL_ATTACK_TRAP_COUNT];
        u8 pad_0C[
            (DUEL_CARD_STAGING_TRAP_RECORD_INDEX_BIAS - DUEL_ATTACK_TRAP_COUNT) *
            sizeof(u16)
        ];
        u16 record_indices[DUEL_ATTACK_TRAP_COUNT];
    } entries;
    u16 words[DUEL_CARD_STAGING_TRAP_RECORD_INDEX_BIAS + DUEL_ATTACK_TRAP_COUNT];
} DuelAttackTrapTable;

typedef struct {
    u8 pad_0000[0x3C68];
    DuelAttackTrapTable table;
} DuelAttackTrapScratchBlock;

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

/* Valid during combined-deck population; the staging buffer has other views. */
typedef struct {
    u8 pad_00000[DUEL_COMBINED_DECK_CARD_FLAGS_OFFSET];
    u8 pre_shuffle_indices[COMBINED_DECK_SIZE];
    u8 pad_after_indices[
        DUEL_COMBINED_DECK_CARD_IDS_OFFSET -
        DUEL_COMBINED_DECK_CARD_FLAGS_OFFSET - COMBINED_DECK_SIZE
    ];
    u16 card_ids[COMBINED_DECK_SIZE];
} DuelCombinedDeckInput;

/* The thirty field records end exactly where the staged deck begins. */
typedef struct {
    u8 pad_00000[0x4B6B4];
    DuelCardRecord field_cards[DUEL_CARD_RECORD_COUNT];
    DuelDeckCardRecord cards[COMBINED_DECK_SIZE];
} DuelCardStagingDeckView;

#define DUEL_CARD_STAGING_OFFSET(type, member) ((u32)&(((type *)0)->member))
typedef char DuelAttackTrapScratchBlock_card_ids_offset_must_be_0x3c68[
    DUEL_CARD_STAGING_OFFSET(DuelAttackTrapScratchBlock, table.entries.card_ids) ==
        0x3C68
        ? 1 : -1
];
typedef char DuelAttackTrapScratchBlock_record_indices_offset_must_be_0x3c88[
    DUEL_CARD_STAGING_OFFSET(DuelAttackTrapScratchBlock, table.entries.record_indices) ==
        0x3C68 + DUEL_CARD_STAGING_TRAP_RECORD_INDEX_BIAS * sizeof(u16)
            ? 1 : -1
];
typedef char DuelAttackTrapScratchBlock_size_must_be_0x3c94[
    sizeof(DuelAttackTrapScratchBlock) == 0x3C94 ? 1 : -1
];
typedef char DuelAttackTrapTable_size_must_be_0x2c[
    sizeof(DuelAttackTrapTable) == 0x2C ? 1 : -1
];
typedef char DuelAttackTrapScratchBlock_alignment_must_match_halfwords[
    __alignof__(DuelAttackTrapScratchBlock) == __alignof__(u16) ? 1 : -1
];
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

/* The byte view is what the raw-buffer consumers take -- image blocks, GPU
 * readback, ID staging lists and trap scratch, none of which are fields of a
 * record. The typed consumers cast it to DuelCardStagingDeckView at the use.
 * The addressing needs a struct-member
 * offset from the buffer base: the base address is
 * materialised `lui %hi` / `addiu %lo`, the 0x48000 part of the member
 * offset is built separately as `lui 0x4` / `ori 0x8000` and added to it,
 * and the remaining 0x39FC stays as the load's displacement. Both typed
 * consumers show that shape -- `lh 0x39FC` at 0x8002805C in Ai_ExportSideSnapshot
 * (gcc_2_8_1_g0_split) and at 0x80018D6C in duel_draw_resolution's
 * func_80018CF8 (gcc_2_8_1_g8_split) -- and a cast of the byte array to the
 * struct pointer reproduces it exactly. */
extern u8 gDuel_abCardStagingBuffer[];

extern u16 gDuel_awCombinedDeckCardIds[];
extern u16 gDuel_awUniqueDeckCardIds[];

#endif
