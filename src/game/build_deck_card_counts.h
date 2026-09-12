#ifndef MEMORIES_DECOMP_BUILD_DECK_CARD_COUNTS_H
#define MEMORIES_DECOMP_BUILD_DECK_CARD_COUNTS_H

#include "../types.h"

/* Puts the build-deck screen's two counters -- the record's +0x5A9C and
 * +0x5AA0 words -- into the D_801D5608 staging pair, then opens the text box
 * that prints them and takes its display object out of screen space. Called
 * after every add or remove, which is what refreshes the count on screen. */
void func_80031E5C(u8 *record);

/* Returns one copy of card `index` to the chest: bumps the owned count at
 * `base` + index + 0x5D97 and the total at +0x5A9C.
 *
 * The two arms differ in what else they touch. From zero owned, the card was
 * not in the chest list at all, so the entry whose id matches is marked
 * visible and the list is re-sorted; from a non-zero count below
 * CARD_CHEST_QUANTITY_MAX only the two numbers move. At the maximum it does
 * nothing, not even the total. The matched visibility-byte write is volatile
 * so the post-search pointer adjustment is not folded into its store offset. */
void func_80031EE4(u8 *base, s32 index);

/* Two updates to the build-deck screen's per-card counts, both taking the
 * screen state block rather than an index into it.
 *
 * func_80031F7C decrements one card's count and, when it reaches zero, clears
 * that card's deck record flag and re-sorts. It does nothing at all when the
 * count is already zero, and returns nothing to say so.
 *
 * func_8003201C recounts every card from the deck records and stores how many
 * leading records are still marked. */
void func_80031F7C(u8 *state, s32 id);
void func_8003201C(u8 *state);

/* Fills the first free entry of the deck list at `base` + 0x2D58 with card
 * `card_id`: marks the entry used, writes the id, and copies the card's type,
 * attack and defense out of gDuel_adwCardStats[card_id - 1] with the same
 * shifts and CARD_STAT_SCALE the rest of the build-deck screen uses. Then it
 * re-sorts the list through func_80032C48 and refreshes the pane, and returns
 * -- so a full deck of DECK_SIZE entries is a silent no-op.
 *
 * The state pointer is retained across sorting and recounting. This pointer
 * contract and the no-strength-reduction profile preserve the slot-pointer
 * form and natural register allocation without the former bindings. */
void BuildDeck_AddCard(u8 *base, s32 card_id);

#endif
