#ifndef MEMORIES_DECOMP_FUNC_80037C74_H
#define MEMORIES_DECOMP_FUNC_80037C74_H

#include "../types.h"
#include "duel_effect.h"

/* Advances an object's scroll position by one step and reports whether it has
 * reached the end.
 *
 * When the counter field_38 reaches its limit field_3E the counter resets and
 * the position field_3A moves on by the step held in field_5B. If that would
 * carry the position past the bound field_42 the step is taken back off and 1
 * is returned, leaving the position at the last value that still fits.
 *
 * field_38 and field_3A are u16 in the record but this function compares them
 * signed, so those two reads keep an explicit `*(s16 *)&` at the use.
 *
 * The return is a flag, not a count. */
s32 func_80037C74(DuelEffectChannel *object);

#endif
