#ifndef MEMORIES_DECOMP_FUNC_8002FD10_H
#define MEMORIES_DECOMP_FUNC_8002FD10_H

#include "../types.h"

/* Initializes the fixed scene presentation package: seeds the event-script
 * flag word D_8009B2A4 from its argument, clears the viewport origin, and
 * requests the package transfer.
 *
 * Both callers declared the parameter s32 while the definition takes s16,
 * and nothing compared them: this unit had no header. Neither call is
 * affected -- func_80031354 passes a constant 0, and Main_RunCampaign passes
 * the u8 gCampaignSceneIndex, which widens the same way to either type -- so
 * the header states the definition's own width.
 *
 * func_80030998 also reaches this function, but through a hand-written
 * `.reloc .-4, R_MIPS_26, func_8002FD10` in inline assembly rather than a C
 * call, so no declaration governs that site. */
void func_8002FD10(s16 arg0);

#endif
