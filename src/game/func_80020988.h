#ifndef MEMORIES_DECOMP_FUNC_80020988_H
#define MEMORIES_DECOMP_FUNC_80020988_H

#include "../types.h"

/* Updates the card-use confirmation UI: 0 while active, -1 for cancel,
 * 1 for confirmation at zero rotation, and 2 for confirmation at nonzero
 * rotation. The caller proceeds with card use only for 1 and restores the
 * card for either other completed result. */
s32 Duel_UpdateCardUseConfirmation(void);

#endif
