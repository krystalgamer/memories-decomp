#ifndef MEMORIES_DECOMP_FUNC_8002A2F4_H
#define MEMORIES_DECOMP_FUNC_8002A2F4_H

#include "../types.h"

/* Stages the selected card for text request 1 and opens its text box.
 * func_8002A6B8's card id goes to gDuel_wSelectedCardID and the text staging
 * record. A non-zero id selects text mode 5, and the selection is cleared
 * again unless the id's flags byte in `state` (func_80029EB0) has bit 0x80;
 * bit 0 of the same byte forces D_8009B320 to 4. func_8002A3CC passes its
 * library motion state. */
void func_8002A2F4(u8 *state);

#endif
