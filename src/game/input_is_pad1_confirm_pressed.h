#ifndef MEMORIES_DECOMP_INPUT_IS_PAD1_CONFIRM_PRESSED_H
#define MEMORIES_DECOMP_INPUT_IS_PAD1_CONFIRM_PRESSED_H

#include "../types.h"

/* Nonzero when a confirm button went down on pad 1 this frame. The value is
 * the masked button bits, not a normalized 1, so callers only test it against
 * zero. */
unsigned int Input_IsPad1ConfirmPressed(void);

#endif
