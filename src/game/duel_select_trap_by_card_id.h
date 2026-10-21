#ifndef MEMORIES_DECOMP_DUEL_SELECT_TRAP_BY_CARD_ID_H
#define MEMORIES_DECOMP_DUEL_SELECT_TRAP_BY_CARD_ID_H

#include "../types.h"

/* Searches the acting side's five mapped field slots for an occupied trap
 * whose id equals the argument, and returns that id on a hit or 0 on a miss.
 *
 * A hit also records the id in gDuel_wSelectedTrapCardId and the owning object's +0x6A slot
 * index in gDuel_bSelectedTrapSlot; a miss leaves gDuel_wSelectedTrapCardId cleared. duel_side_state.h
 * describes both of those globals, and the definition in duel_select_trap_by_card_id.c
 * carries the rest.
 *
 * duel_card_effects.c calls it with two card ids, so the return is used as a
 * present/absent test rather than for the value it returns. */
s32 Duel_SelectTrapByCardId(s32 card_id);

#endif
