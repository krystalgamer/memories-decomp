#ifndef MEMORIES_DECOMP_SCRIPT_OP_DUEL_RESULT_H
#define MEMORIES_DECOMP_SCRIPT_OP_DUEL_RESULT_H

#include "../types.h"

/* gScript_apfnCommandHandlers handler: the duel result screen. Reads the two-byte result code
 * from the stream, queues the result sector with func_8002F4C0 as the
 * completion callback, and on the first pass -- once the transfer and fade flags
 * are clear -- builds the banner, the two 0x140-wide panels and the fill object
 * kept in gScript_TransitionObject, then starts the music. */
void Script_OpDuelResult(void);

#endif
