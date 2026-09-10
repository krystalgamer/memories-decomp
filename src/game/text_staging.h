#ifndef MEMORIES_DECOMP_TEXT_STAGING_H
#define MEMORIES_DECOMP_TEXT_STAGING_H

#include "../types.h"
#include "../ygo_types.h"
#include "duel_grid.h"

/* Overlapping message inputs, not a persistent record or allocation extent.
 * Each producer selects the view belonging to the text request it opens.
 * Evidence and addressing boundaries: notes/text-staging.md. */
typedef union {
    struct {
        s32 attack;
        s32 defense;
        s32 rank;
    } card_stats;
    struct {
        s32 card_id;
        s32 count;
    } card;
    struct {
        s32 chest;
        s32 deck;
    } build_deck;
    struct {
        s32 used;
        s32 needed;
    } blocks;
    struct {
        s32 field_00[16];
        s32 invalid_side;
    } deck_validation;
    s32 library_count;
    s32 rank_rows[16][DUEL_SIDE_COUNT];
    Pair pair;
} TextStagingValues;

typedef char TextStagingValues_size_must_be_0x80[
    sizeof(TextStagingValues) == 0x80 ? 1 : -1
];
typedef char TextStagingValues_defense_offset_must_be_4[
    (u32)&((TextStagingValues *)0)->card_stats.defense == 4 ? 1 : -1
];
typedef char TextStagingValues_rank_offset_must_be_8[
    (u32)&((TextStagingValues *)0)->card_stats.rank == 8 ? 1 : -1
];
typedef char TextStagingValues_count_offset_must_be_4[
    (u32)&((TextStagingValues *)0)->card.count == 4 ? 1 : -1
];
typedef char TextStagingValues_deck_offset_must_be_4[
    (u32)&((TextStagingValues *)0)->build_deck.deck == 4 ? 1 : -1
];
typedef char TextStagingValues_needed_offset_must_be_4[
    (u32)&((TextStagingValues *)0)->blocks.needed == 4 ? 1 : -1
];
typedef char TextStagingValues_invalid_side_offset_must_be_0x40[
    (u32)&((TextStagingValues *)0)->deck_validation.invalid_side == 0x40
        ? 1 : -1
];

/* Keep the incomplete-array addressing arm for resident message producers.
 * Only element zero is a view; this does not assert an array of allocations.
 * The rank producer and overlays retain their measured table/scalar forms. */
#if defined(TEXT_STAGING_AS_PAIR) && defined(TEXT_STAGING_AS_RANK_ROWS)
#error Select only one text staging declaration view
#endif
#if defined(TEXT_STAGING_AS_PAIR)
extern Pair D_801D5608;
#elif defined(TEXT_STAGING_AS_RANK_ROWS)
extern s32 D_801D5608[16][DUEL_SIDE_COUNT];
#else
extern TextStagingValues D_801D5608[];
#endif

#ifdef TEXT_STAGING_STARCHIPS_ALIAS
extern s32 D_801D5608_starchips asm("D_801D5608");
#endif

#endif
