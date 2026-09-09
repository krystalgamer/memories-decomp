#ifndef MEMORIES_DECOMP_FUNC_80037DA4_H
#define MEMORIES_DECOMP_FUNC_80037DA4_H

#include "../types.h"

/* D_80090EAC entry: the card-name and stat text command. It reads an opcode
 * byte, resolves a card id -- gDuel_wSelectedCardID or one taken from the
 * stream -- and appends the requested field to the object's text.
 *
 * The body is register-pinned on the current stream pointer; the note in the
 * source explains which pair local-alloc otherwise swaps. */
void func_80037DA4(u8 *object);

#endif
