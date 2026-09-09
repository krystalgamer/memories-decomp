#ifndef MEMORIES_DECOMP_FUNC_8002E5AC_H
#define MEMORIES_DECOMP_FUNC_8002E5AC_H

#include "../types.h"

/* D_80090C50 handler: opens a dialog text box at a fixed 0x10,0xB0 rectangle
 * from the 16-bit string id in the stream, marks it through
 * DuelEffect_MarkObjectIfActive and sets its 0x8 flag. Bit 15 of the id
 * instead raises 0x4000 in D_8009B27C and CLEARS that 0x8 bit, reading the
 * box's flags word through a volatile view so the two writes are not merged.
 * The busy word is then latched into D_8009B28C, which this unit defines
 * rather than declares so the store stays gp-relative. */
void func_8002E5AC(void);

#endif
