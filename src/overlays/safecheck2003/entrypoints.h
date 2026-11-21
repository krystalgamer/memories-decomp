#ifndef MEMORIES_DECOMP_OVERLAYS_SAFECHECK2003_ENTRYPOINTS_H
#define MEMORIES_DECOMP_OVERLAYS_SAFECHECK2003_ENTRYPOINTS_H

#include "../../types.h"

/* The two entries Sony's safechk library exports (module ID 0x16, WA_MRG
 * 0xB61800, loaded at 0x80168000). Resident callers must load the module
 * first; the boot sequence then uses them as the developers wrote:
 *
 *     SafetyInit();
 *     while (SafetySync(1));
 *
 * SafetyInit reads the BIOS region byte and sets the start state.
 * SafetySync runs the controller's states and returns the last one's result,
 * 0 once the check is finished. Like CdSync, MODE 1 runs one state and
 * returns, and MODE 0 runs states until one returns 0. */
void SafetyInit(void);
s32 SafetySync(s32 mode);

#endif
