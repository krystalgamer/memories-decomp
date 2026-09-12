#ifndef MEMORIES_DECOMP_SCRIPT_CONTROL_COMMANDS_H
#define MEMORIES_DECOMP_SCRIPT_CONTROL_COMMANDS_H

#include "../types.h"

/* Three D_80090C50 handlers that drive the scene's own state rather than any
 * object. Script_OpGameOver and Script_OpCredits set the mode pair D_8009B269 and
 * D_8009B26C to 12 and 15 -- and both write the same value twice, once to each,
 * which is why they are two functions and not one with an argument.
 *
 * Script_OpWait is the wait: it takes a 16-bit count from the stream on the
 * first tick, then counts down once per tick and clears D_8009B27C when the
 * count reaches zero. */
void Script_OpGameOver(void);
void Script_OpCredits(void);
void Script_OpWait(void);

#endif
