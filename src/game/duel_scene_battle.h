#ifndef MEMORIES_DECOMP_DUEL_SCENE_BATTLE_H
#define MEMORIES_DECOMP_DUEL_SCENE_BATTLE_H

#include "../types.h"

/* State private to duel scene-state 9, the battle resolution phase.
 * DuelScene_UpdateBattle (src/candidates/func_8001F55C.c) is the only source in
 * the tree that names either of these, and both were declared inside that
 * source because the unit had no header to take them from. This is the
 * arrangement duel_scene_hand_actions.h and duel_scene_field_actions.h record
 * for scene-states 4 and 5.
 *
 * The function's own prototype is deliberately not here. It is still
 * unmatched_asm in config/slus_01411/functions.csv, so it belongs in
 * src/unmatched.h (line 186) with the other generated-assembly entry points,
 * and that is where duel_scene_callbacks.c already reaches it for the phase
 * callback table.
 *
 * Both are a two-entry pair indexed by D_8009B1B9
 * (duel_scene_card_placement.h), which that source takes through its signed
 * view. Every access to both is gp-relative in the versioned listing
 * (src/candidates_target/func_8001F55C.S): four relocations each, none through
 * %hi/%lo. The same listing reaches D_800EF658 through %hi/%lo three times and
 * gp-relatively never, which is what tells the two forms apart here.
 *
 * Neither declared bound comes from the address map, and that is worth being
 * explicit about: the next named symbol after both is D_8009B1B4, sixteen and
 * four bytes on, so the gaps are larger than the declarations and are no
 * evidence for them. The bound is what the one declarer wrote, and what the
 * use sites exercise -- [0] and [1] written, and the pair read at that one
 * index.
 */

/* The life-point delta each side takes from the exchange. The resolution step
 * zeroes both entries and then stores func_8001EFD4's result into the entry for
 * the side that lost points (func_8001F55C.c:309, :311, :330, :336, :337,
 * :345); case 8 reads it back as the damage to present, sizing the effect from
 * its magnitude -- |damage| / 1000, capped at 2 (:495).
 *
 * The listing shows three sh and no load at all: the read is indexed, so it
 * comes out as one addiu forming the base off $gp
 * (func_8001F55C.S:464, :539, :558, :914). That is the array form, and the
 * private declaration's s16 [2] is kept verbatim. */
extern s16 D_8009B1A4[2];

/* How that exchange went for each side, as a small signed marker rather than a
 * count: the same step writes 0, 1 and -1 into the two entries
 * (func_8001F55C.c:308, :310, :315, :329, :334, :335, :346, :348). Case 8 takes
 * its address into a local and tests the entry against zero to decide whether
 * to raise the damage-number request at all, then tests it again for
 * greater-than-zero to pick the variant (:490, :491, :503).
 *
 * Three sb and, as above, no load: taking the address gives one addiu off $gp
 * (func_8001F55C.S:463, :536, :564, :902). The signed s8 [2] is what the one
 * declarer wrote and what the -1 stores need, and it is kept verbatim. */
extern s8 D_8009B1B0[2];

#endif
