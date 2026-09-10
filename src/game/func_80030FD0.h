#ifndef MEMORIES_DECOMP_FUNC_80030FD0_H
#define MEMORIES_DECOMP_FUNC_80030FD0_H

#include "../types.h"

/* One step handler of the two tables at 0x80090D7C and 0x80090D84.
 *
 * It tears the screen down -- Fade_WaitInitOut, func_800403F0, func_80035A64
 * -- and then longjmps to D_800E9DC0 with 2, so it does not return to its
 * caller. The definition's own comment records that the imported longjmp
 * prototype lacks noreturn and that retail emits no epilogue after the call,
 * which is why this is still spelled as an ordinary void function. */
void func_80030FD0(void);

#endif
