#ifndef MEMORIES_DECOMP_DUEL_SCENE_HAND_ACTIONS_H
#define MEMORIES_DECOMP_DUEL_SCENE_HAND_ACTIONS_H

#include "../types.h"

/* State private to duel scene-state 4, the hand navigation and card play
 * phase. DuelScene_UpdateHandActions (src/candidates/func_8001BD88.c) is the
 * only source in the tree that names any of these three, and each was declared
 * inside that source because the unit had no header to take them from.
 *
 * The function's own prototype is deliberately not here. It is still
 * unmatched_asm in config/slus_01411/functions.csv, so it belongs in
 * src/unmatched.h with the other generated-assembly entry points, and that is
 * where duel_scene_callbacks.c already reaches it for the phase callback
 * table. These three globals are the opposite case: unmatched.h is for
 * declarations several sources write for themselves and disagree about -- its
 * neighbours here are annotated "four declarers" and "nine declarers" -- and
 * each of these has exactly one declarer, in a unit that does have a defining
 * translation unit and so can have a header.
 *
 * Every access to all three is gp-relative in the versioned listing
 * (src/candidates_target/func_8001BD88.S): 5, 3 and 4 relocations, none
 * through %hi/%lo. So these are the plain scalar declarations. That the
 * distinction is real in this listing rather than an artefact of the pattern
 * is visible in the same file, where D_800EAE88 is reached through %hi/%lo
 * twice and gp-relatively never.
 *
 * A fourth symbol that source declares privately, D_8009B20A, is not here:
 * 0x8009B20A falls inside D_8009B208[8], which duel_scene_resume.h already
 * declares, so that storage has an owner.
 */

/* Whether the scripted path has a card to stack onto the hand. The AI step
 * clears it, and raises it when the selection's field1 byte D_800EAE89 (ai.h)
 * is non-zero; the following step keeps bit 0 and tests the result, and the
 * delayed step tests bit 0 again to decide whether the hand-stack child is
 * spawned (func_8001BD88.c:189, :191, :206, :207, :235).
 *
 * The two loads are lhu and the three stores are sh. That unsigned view is
 * what the source reaches with an explicit (u16) cast at each arithmetic site
 * rather than through the declaration, so the declared sign does not show in
 * the object code and the stores cannot settle it either. The private
 * declaration's s16 is therefore kept verbatim; re-typing it would be a
 * codegen claim about a candidate rather than a declaration move. The next
 * named symbol is D_8009B1A0, two bytes on, so the declared width fills the
 * gap exactly. */
extern s16 D_8009B19E;

/* The cursor into the AI's pending selection list D_800EAE88 (ai.h). The
 * scripted step sets it to -1 through a signed lvalue cast, and each following
 * step pre-increments it and reads D_800EAE88 at the new index, so the walk
 * starts at the list's first entry (func_8001BD88.c:188, :201, :202).
 *
 * Declared u8, as the private declaration had it: the single load is lbu and
 * the two stores are sb, and the signed views are casts at the use sites --
 * the arrangement duel_hand.h records for D_8009B1EC, where the declaration
 * stays unsigned and the cast at the use is what keeps the signed read. The
 * gap to the next named symbol is six bytes, and D_8009B1E4 below sits inside
 * it, so that gap is not evidence of a wider object. */
extern u8 D_8009B1E2;

/* The step delay for that walk. Each selection entry loads it with 6, every
 * tick takes one off, and the step runs once the signed value reaches zero or
 * below, reloading 6 (func_8001BD88.c:218, :220, :221, :222).
 *
 * Declared u16. The single load is lhu and the three stores are sh; the one
 * signed read is spelled (s16) at the bound test, which is where the private
 * declaration put it. */
extern u16 D_8009B1E4;

#endif
