#ifndef MEMORIES_DECOMP_BUILD_DECK_CARD_COUNTS_H
#define MEMORIES_DECOMP_BUILD_DECK_CARD_COUNTS_H

#include "../types.h"
#include "build_deck_transition_state.h"

/* Puts the build-deck screen's two counters -- chest_total and deck_total --
 * into the D_801D5608 staging pair, then opens the text box
 * that prints them and takes its display object out of screen space. Called
 * after every add or remove, which is what refreshes the count on screen. */
void BuildDeck_RefreshCountDisplay(BuildDeckTransitionState *record);

/* Returns one copy of card `index` to the chest: bumps
 * base->chest_card_quantities[index] and base->chest_total.
 *
 * The two arms differ in what else they touch. From zero owned, the card was
 * not in the chest list at all, so the entry whose id matches is marked
 * visible and the list is re-sorted; from a non-zero count below
 * CARD_CHEST_QUANTITY_MAX only the two numbers move. At the maximum it does
 * nothing, not even the total. The matched visibility-byte write is volatile
 * so the post-search pointer adjustment is not folded into its store offset. */
void BuildDeck_ReturnCardToChest(BuildDeckTransitionState *base, s32 index);

/* Two updates to the build-deck screen's per-card counts, both taking the
 * complete BuildDeckTransitionState. func_8003201C keeps byte cursors only
 * inside its count loop, where retail walks the quantity table and deck rows
 * from measured offsets.
 *
 * BuildDeck_TakeCardFromChest decrements one card's count and, when it reaches zero, clears
 * that card's deck record flag and re-sorts. It does nothing at all when the
 * count is already zero, and returns nothing to say so.
 *
 * func_8003201C recounts every card from the deck records and stores how many
 * leading records are still marked. */
void BuildDeck_TakeCardFromChest(BuildDeckTransitionState *state, s32 id);
void func_8003201C(BuildDeckTransitionState *state);

/* Fills the first free entry of the workspace's second CardList with card
 * `card_id`: marks the entry used, writes the id, and copies the card's type,
 * attack and defense out of gDuel_adwCardStats[card_id - 1] with the same
 * shifts and CARD_STAT_SCALE the rest of the build-deck screen uses. The
 * biased halfword cursor's base and stride are derived from
 * BuildDeckTransitionState and CardEntry. It then re-sorts through
 * func_80032C48, refreshes the pane, and returns, so a full deck of DECK_SIZE
 * entries is a silent no-op.
 *
 * `base` is s32 rather than a pointer because that is what the definition
 * takes: the screen record is reached through integer address arithmetic,
 * never dereferenced as a struct. Its one caller holds the same record as
 * u8 * and casts. The no-strength-reduction profile keeps the ordinary
 * induction pointer in the retail register without a hard-register pin. */
void BuildDeck_AddCard(s32 base, s32 card_id);

#endif
