#ifndef MEMORIES_DECOMP_FUNC_80025028_H
#define MEMORIES_DECOMP_FUNC_80025028_H

#include "../types.h"

/* Searches the acting side's five monster slots for an occupied card whose id
 * equals the argument, and returns that id on a hit or 0 on a miss.
 *
 * A hit also records the id in D_8009B22A and the owning object's +0x6A slot
 * index in D_8009B1B8; a miss leaves D_8009B22A cleared. duel_side_state.h
 * describes both of those globals, and the definition in func_80025028.c
 * carries the rest.
 *
 * duel_card_effects.c calls it with two card ids, so the return is used as a
 * present/absent test rather than for the value it returns. */
s32 func_80025028(s32 card_id);

#endif
