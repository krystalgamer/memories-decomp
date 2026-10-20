#ifndef MEMORIES_DECOMP_DUEL_EFFECT_NOOP_HANDLERS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_NOOP_HANDLERS_H

/* State-table slots 3 and 4. The first remains an unknown empty handler.
 * IdentitySave is also empty in NA/Japan; Europe generates a live identity,
 * requests a save and marks completion for any nonzero dialog outcome.
 * The dispatcher consumes that completion on a later invocation. */
void func_800289AC(void);
void DuelEffect_UpdateIdentitySave(void);

#endif
