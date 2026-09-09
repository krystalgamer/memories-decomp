#ifndef MEMORIES_DECOMP_DUEL_DECK_LOOKUP_H
#define MEMORIES_DECOMP_DUEL_DECK_LOOKUP_H

#include "../types.h"

/* Two lookups over the player's deck. func_8002C4DC returns the deck index of a
 * card id, or -1 when it is not in the deck. func_8002C518 answers the weaker
 * question the Library screen asks: a set owned-flag at D_801D0000[id + 591]
 * succeeds immediately, and otherwise the deck index decides -- so its -1 means
 * "neither owned nor in the deck". */
s32 func_8002C4DC(s32 card_id);
s32 func_8002C518(s32 card_id);

#endif
