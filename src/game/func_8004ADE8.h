#ifndef MEMORIES_DECOMP_FUNC_8004ADE8_H
#define MEMORIES_DECOMP_FUNC_8004ADE8_H

#include "../types.h"

/* Starts one secondary-driver note on `channel` (its low byte selects the
 * sequence channel record) for `note` with `velocity`: it walks the VAB
 * program and tone data, allocates a secondary object, fills the voice
 * attributes, keys the voice on and routes its reverb.
 *
 * The definition takes `velocity` as a byte in old style; the caller passes
 * a word, so the prototype keeps the promoted type. */
void func_8004ADE8(s32 channel, s32 note, s32 velocity);

#endif
