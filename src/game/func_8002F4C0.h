#ifndef MEMORIES_DECOMP_FUNC_8002F4C0_H
#define MEMORIES_DECOMP_FUNC_8002F4C0_H

#include "../types.h"

/* The completion callback func_8002F630 hands to File_RequestAsyncTransfer
 * for the duel result screen's asset sector. It dispatches on its second
 * parameter, so the transfer layer calls it back for more than one stage of
 * the same request.
 *
 * Worth stating why this declaration belongs here rather than at the caller.
 * The callback slot is typed FileTransferCallback, which ygo_types.h defines
 * as `void (*)()` -- unprototyped -- so passing this function there checks
 * nothing about its arguments, and func_8002F630 only ever took its address.
 * Between them, no site in the tree could compare a declaration against the
 * definition. Declaring it in the defining unit's own header, which
 * func_8002F4C0.c includes, makes the compiler check it on every build. */
void func_8002F4C0(u8 *p, s32 mode);

#endif
