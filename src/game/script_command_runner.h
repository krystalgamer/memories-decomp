#ifndef MEMORIES_DECOMP_SCRIPT_COMMAND_RUNNER_H
#define MEMORIES_DECOMP_SCRIPT_COMMAND_RUNNER_H

#include "../types.h"

/* gScript_apfnCommandHandlers handler: the conditional script jump on deck state. Always
 * consumes a 16-bit offset, and repoints gScript_StreamCursor into D_801A8000 only when
 * Duel_IsPlayerDeckComplete reports the deck is NOT full -- so the jump is the
 * deck-incomplete path and falling through is the complete one. */
void Script_OpJumpIfDeckIncomplete(void);

/* gScript_apfnCommandHandlers handler: hands the scene to mode 8, with D_8009B268 set to 1 and
 * D_8009B26D to 5. Consumes nothing from the stream and never clears the busy
 * word. */
void Script_OpReturnToMenu(void);

void Script_RunTick(void);

#endif
