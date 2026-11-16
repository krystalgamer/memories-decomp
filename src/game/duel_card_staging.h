#ifndef MEMORIES_DECOMP_DUEL_CARD_STAGING_H
#define MEMORIES_DECOMP_DUEL_CARD_STAGING_H

#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_scene.h"

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

/* The combined-deck ID lists. Duel_PopulateCombinedDeckData reads the IDs
 * and pre-shuffle slots; Duel_RequestCombinedDeckData sorts a copy of the
 * IDs (gDuel_awCombinedDeckCardIds, DUEL_DECK_ID_BUFFER_STRIDE bytes on) and
 * writes the END-terminated unique list another stride further on
 * (gDuel_awUniqueDeckCardIds). */
typedef struct {
    u8 pad_00000[0x1BB70];
    u8 pre_shuffle_indices[COMBINED_DECK_SIZE];
    u8 pad_1BBC0[4];
    u16 card_ids[COMBINED_DECK_SIZE];
    u8 pad_1BC64[4];
    u16 sorted_card_ids[COMBINED_DECK_SIZE];
    u8 pad_1BD08[4];
    u16 unique_card_ids[COMBINED_DECK_SIZE + 1];
} DuelCombinedDeckInput;

/* The thirty field records end exactly where the staged deck begins. */
typedef struct {
    u8 pad_00000[0x4B6B4];
    DuelCardRecord field_cards[DUEL_CARD_RECORD_COUNT];
    DuelDeckCardRecord cards[COMBINED_DECK_SIZE];
} DuelCardStagingDeckView;

/* gDuel_abCardStagingBuffer is reused for card-data transfers, GPU readback
 * and duel scratch; each member is one use's view of the same storage, not a
 * declaration of one allocation's lifetime. The deck view reaches the
 * buffer's last known byte (0x801A8000 exclusive). */
typedef union {
    /* Card-data transfer target and the image blocks copied from it. */
    u8 bytes[sizeof(DuelCardStagingDeckView)];
    /* BGR555 pixels of a GPU readback. */
    u16 pixels[sizeof(DuelCardStagingDeckView) / sizeof(u16)];
    /* The words LoadImage2/StoreImage2 transfer. */
    u32 words[sizeof(DuelCardStagingDeckView) / sizeof(u32)];
    DuelCombinedDeckInput deck_lists;
    DuelCardStagingDeckView deck;
} DuelCardStagingBuffer;

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
typedef char DuelCombinedDeckInput_pre_shuffle_indices_offset_must_be_0x1BB70[
    DUEL_CARD_STAGING_OFFSET(DuelCombinedDeckInput, pre_shuffle_indices) ==
        0x1BB70 ? 1 : -1
];
typedef char DuelCombinedDeckInput_card_ids_offset_must_be_0x1BBC4[
    DUEL_CARD_STAGING_OFFSET(DuelCombinedDeckInput, card_ids) == 0x1BBC4
        ? 1 : -1
];
typedef char DuelCombinedDeckInput_sorted_ids_follow_by_one_stride[
    DUEL_CARD_STAGING_OFFSET(DuelCombinedDeckInput, sorted_card_ids) ==
        0x1BBC4 + DUEL_DECK_ID_BUFFER_STRIDE ? 1 : -1
];
typedef char DuelCombinedDeckInput_unique_ids_offset_must_be_0x1BD0C[
    DUEL_CARD_STAGING_OFFSET(DuelCombinedDeckInput, unique_card_ids) ==
        0x1BD0C ? 1 : -1
];
typedef char DuelCardStagingBuffer_size_must_end_at_0x801A8000[
    sizeof(DuelCardStagingBuffer) == 0x801A8000 - 0x8015C424 ? 1 : -1
];
#undef DUEL_CARD_STAGING_OFFSET

/* The deck-view readers index a member at a large constant offset; retail
 * materialises the base `lui %hi` / `addiu %lo`, adds the 0x48000 part
 * separately as `lui 0x4` / `ori 0x8000` and keeps the remaining 0x39FC as
 * the load displacement (`lh 0x39FC` at 0x8002805C in Ai_ExportSideSnapshot
 * and at 0x80018D6C in Duel_HasAllExodiaPieces). */
extern DuelCardStagingBuffer gDuel_abCardStagingBuffer;

/* A pointer into the staging buffer read as bytes, as the BGR555 pixels a
 * GPU readback stores, or as the words LoadImage2/StoreImage2 transfer. */
typedef union {
    u8 *bytes;
    u16 *pixels;
    u32 *words;
} DuelCardStagingPointer;

extern u16 gDuel_awCombinedDeckCardIds[];
extern u16 gDuel_awUniqueDeckCardIds[];

#endif
