#include "../../types.h"
#define SCRIPT_COMMAND_BUSY_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C1F0-0x8009C1F4 (#2602); its
 * layout differs from the North American block the shared source defines. */
u16 gScript_wCommandWord;
u16 gScript_SharedOperand;

/* The European builds compile the shared unit through this wrapper so that
 * they can give it their own zero-initialized storage (#2602). */
#include "../script_command_busy.c"
