#ifndef MEMORIES_DECOMP_DUEL_SCENE_BATTLE_H
#define MEMORIES_DECOMP_DUEL_SCENE_BATTLE_H

#include "../types.h"

/* Duel scene-state 9, the battle resolution phase, and the two side pairs it
 * keeps private. DuelScene_UpdateBattle (src/game/duel_scene_battle.c) is the
 * only source in the tree that names either pair; duel_scene_hand_actions.h
 * and duel_scene_field_actions.h record the same arrangement for scene-states
 * 4 and 5.
 *
 * Both are a two-entry pair indexed by D_8009B1B9
 * (duel_scene_card_placement.h), which that source takes through its signed
 * view. Every access to both is gp-relative in the retail listing: four
 * relocations each, none through %hi/%lo. The same listing reaches
 * D_800EF658 through %hi/%lo three times and gp-relatively never, which is
 * what tells the two forms apart here.
 *
 * Neither declared bound comes from the address map, and that is worth being
 * explicit about: the next named symbol after both is D_8009B1B4, sixteen and
 * four bytes on, so the gaps are larger than the declarations and are no
 * evidence for them. The bound is what the one declarer wrote, and what the
 * use sites exercise -- [0] and [1] written, and the pair read at that one
 * index.
 */

/* Scene-state 9: presents the attacker and the defender, resolves the
 * exchange, shows the damage and the guardian-star bonus, and hands off to
 * the trap, fusion-result and turn-switch states. */
void DuelScene_UpdateBattle(void);

/* The life-point delta each side takes from the exchange. The resolution step
 * zeroes both entries and then stores func_8001EFD4's result into the entry for
 * the side that lost points; case 8 reads it back as the damage to present,
 * sizing the effect from its magnitude -- |damage| / 1000, capped at 2.
 *
 * The listing shows three sh and no load at all: the read is indexed, so it
 * comes out as one addiu forming the base off $gp. That is the array form,
 * and the private declaration's s16 [2] is kept verbatim. */
extern s16 D_8009B1A4[2];

/* How that exchange went for each side, as a small signed marker rather than a
 * count: the same step writes 0, 1 and -1 into the two entries. Case 8 takes
 * its address into a local and tests the entry against zero to decide whether
 * to raise the damage-number request at all, then tests it again for
 * greater-than-zero to pick the variant.
 *
 * Three sb and, as above, no load: taking the address gives one addiu off
 * $gp. The signed s8 [2] is what the one declarer wrote and what the -1 stores
 * need, and it is kept verbatim. */
extern s8 D_8009B1B0[2];

#endif
