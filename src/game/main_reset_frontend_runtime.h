#ifndef MEMORIES_DECOMP_MAIN_RESET_FRONTEND_RUNTIME_H
#define MEMORIES_DECOMP_MAIN_RESET_FRONTEND_RUNTIME_H

#include "../types.h"

void Main_ResetFrontendRuntime(void);

/* The frontend loop's phase byte, and the gate on the reset above.
 *
 * Bit 0 chooses the phase: clear runs the attract path, set runs the menu.
 * Bit 0x80 is the "already reset for this phase" latch -- both arms of
 * func_80043BCC open with `if ((f & 0x80) == 0)`, set the bit, and call
 * Main_ResetFrontendRuntime immediately, so the reset happens once per phase.
 * Main_RunBootSequence clears the whole byte when it sets the frontend up.
 *
 * func_80043BCC also tests bit 0x40, and nothing sets it: notes/global-usage
 * lists Main_RunBootSequence and func_80043BCC as the only accessors of this
 * address
 * in the image, assembly included, and between them they only ever store 0, 1
 * or `f | 0x80`. That arm is recorded here as an observation, not a claim
 * about intent. */
extern u8 D_8009B428;

#endif
