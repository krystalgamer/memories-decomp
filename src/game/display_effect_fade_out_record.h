#ifndef MEMORIES_DECOMP_DISPLAY_EFFECT_FADE_OUT_RECORD_H
#define MEMORIES_DECOMP_DISPLAY_EFFECT_FADE_OUT_RECORD_H

#include "../types.h"
#include "menu_record.h"

/* Outgoing effect step 6: initialize a secondary blend row and ramp 0x80,
 * then subtract D_8009B0D8 * 8 in the same call. A nonpositive signed result
 * clears the step, releases grid[1] and then releases the record.
 *
 * Keep the MenuRecord callback type and explicit narrow initialization view.
 * Primary objects do not survive this endpoint as they do for fade-in. */
void DisplayEffect_FadeOutRecord(MenuRecord *record);

#endif
