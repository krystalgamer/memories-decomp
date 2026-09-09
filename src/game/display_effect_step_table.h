#ifndef YUGIOH_GAME_DISPLAY_EFFECT_STEP_TABLE_H
#define YUGIOH_GAME_DISPLAY_EFFECT_STEP_TABLE_H

#include "../types.h"

/* The display effect step table at 0x80090F68.
 *
 * Left unsized deliberately. Its consumer indexes it with `& 0x1F`, which
 * permits thirty-two entries against the thirteen defined, so a bound here
 * would contradict the call site rather than describe it. */
extern void (*D_80090F68[])(u8 *);

#endif
