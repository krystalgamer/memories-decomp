#ifndef MEMORIES_DECOMP_DUEL_SCENE_FIELD_ACTIONS_H
#define MEMORIES_DECOMP_DUEL_SCENE_FIELD_ACTIONS_H

#include "../types.h"

/* State private to duel scene-state 5, the field navigation, attack selection
 * and battle transition phase. DuelScene_UpdateFieldActions
 * (src/candidates/func_8001D670.c) is the only source in the tree that names
 * either of these, and both were declared inside that source because the unit
 * had no header to take them from. This is the arrangement
 * duel_scene_hand_actions.h records for the neighbouring scene-state 4.
 *
 * The function's own prototype is deliberately not here. It is still
 * unmatched_asm in config/slus_01411/functions.csv, so it belongs in
 * src/unmatched.h (line 185) with the other generated-assembly entry points,
 * and that is where duel_scene_callbacks.c already reaches it for the phase
 * callback table.
 *
 * Every access to both is gp-relative in the versioned listing
 * (src/candidates_target/func_8001D670.S): 4 and 6 relocations, none through
 * %hi/%lo, so these are the plain scalar declarations. That the distinction is
 * real in this listing rather than an artefact of the pattern is visible in the
 * same file, where D_800EAE88 is reached through %hi/%lo five times and
 * gp-relatively never.
 */

/* The target colour of the field dim ramp: one grey level repeated in three
 * channels. The pass that dims the field sets it to 0x202020
 * (func_8001D670.c:534) and a second site sets it to 0x808080 while starting
 * the same walk over the D_801A7AD8 card records (:616). The ramp moves
 * D_8009B300 (sorted_entry.h, which this unit already includes) eight levels at
 * a time towards it, reading this symbol's low
 * channel through a (u8) cast to get the level, clamping there, repacking the
 * three channels, and comparing the whole word to know it has arrived
 * (:564, :577).
 *
 * Declared s32, which is what the private declaration said and what the
 * listing shows: the two stores are sw, and the two loads are one lw for that
 * word comparison and one lbu for the channel read -- so the narrow read is a
 * cast at the use rather than the declaration's width. */
extern s32 D_8009B1BC;

/* Which of the two field passes the state is in. It is cleared when the state
 * sets up (func_8001D670.c:143), raised once (:246) and cleared again (:255),
 * and its three reads choose between the dim-and-ramp path and the path that
 * waits for the tagged display objects to retire before finishing
 * (:375, :533, :556).
 *
 * Declared u8: the three loads are lbu and the three stores are sb. Every
 * value written is 0 or 1 and every read is a test against 0, so the stores
 * cannot settle a sign here -- but the loads are the unsigned form, which is
 * the spelling its one declarer chose, and it is kept verbatim. */
extern u8 D_8009B21A;

#endif
