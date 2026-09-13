#ifndef MEMORIES_DECOMP_DUEL_TRAP_RESOLUTION_H
#define MEMORIES_DECOMP_DUEL_TRAP_RESOLUTION_H

#include "../types.h"

/* Decides whether the attacking card trips the trap it is walking into,
 * comparing its attack against gDuel_abTrapAttackThresholds scaled by 100.
 * The table is defined in this unit and its comment names each entry. */
s32 func_8001F0D0(u8 *record);

/* Runs the trap presentation one step and reports whether it is still busy:
 * zero once the sequence has finished, non-zero while it is running or when
 * D_8009B162 blocks it.
 *
 * It takes NO arguments. duel_card_effects.c declared it `s32 (s32)` and
 * called it with a flag at both sites; the definition ignores whatever is in
 * that register. Dropping the argument was measured rather than assumed --
 * the executable is byte-identical with the calls spelled `func_8001F364()`
 * -- so the declaration follows the definition here.
 *
 * duel_action_lock.h describes the mode word it sequences on, `D_8009B210 &
 * 0xF`, and records that duel_card_effects.c clears that word to 0 at the two
 * points the effect finishes -- the same two sites that make these calls. */
s32 func_8001F364(void);

#endif
