#ifndef MEMORIES_DECOMP_DUEL_FIELD_EFFECT_TRANSITION_H
#define MEMORIES_DECOMP_DUEL_FIELD_EFFECT_TRANSITION_H

#include "../types.h"

/* gDuelEffect_apfnGroupHandler entry: opens the duel-side effect prompt and,
 * once it is acknowledged, hands every occupied slot of the acting side to the
 * func_80025B28 animation by writing that function into each object's callback
 * slot.
 *
 * func_80025B28 itself is deliberately NOT declared here. Its parameter is this
 * unit's own private struct, and duel_field_effect_steps.c -- which needs the
 * declaration to install it as a callback -- has its own private struct over
 * the same record, with the callback member typed against that. A header would
 * have to export one of the two views and retype the other unit's member, which
 * is a type question for #2501 rather than a prototype one. */
void func_80025BEC(void);

#endif
