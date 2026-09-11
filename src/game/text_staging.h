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

/* One declaration. Only element zero is a view; this does not assert an
 * array of allocations, and the incomplete array is what keeps the symbol
 * outside small data for the resident producers.
 *
 * The rank producer and the two overlay producers used to select their own
 * spellings of this address, `s32 [16][DUEL_SIDE_COUNT]` and `Pair`. Both are
 * already members of the union above, `rank_rows` and `pair`, and nothing
 * separated them from it: the rank table is 128 bytes against the union's
 * 0x80 and both are outside small data at -G8, while the two Pair consumers
 * compile at gcc_2_8_1_g0_split, whose compiler_flags and maspsx_flags both
 * carry -G0, so eight bytes cannot classify differently there either.
 * Written through the members, the retail SHA-256 and all five overlay
 * images are unchanged.
 *
 * The starchip alias below is the one spelling left apart. `library_count` at
 * offset 0 is the same word, but its consumer is not touched here. */
extern TextStagingValues D_801D5608[];

#ifdef TEXT_STAGING_STARCHIPS_ALIAS
extern s32 D_801D5608_starchips asm("D_801D5608");
#endif

#endif
