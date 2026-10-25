#ifndef MEMORIES_DECOMP_DUEL_RESULT_LOAD_PACKAGE_STAGE_H
#define MEMORIES_DECOMP_DUEL_RESULT_LOAD_PACKAGE_STAGE_H

#include "../types.h"
#include "../ygo_types.h"

/* The completion callback Script_OpDuelResult hands to File_RequestAsyncTransfer
 * for the duel result screen's asset sector. It dispatches on its second
 * parameter, so the transfer layer calls it back for more than one stage of
 * the same request.
 *
 * Worth stating why this declaration belongs here rather than at the caller.
 * The callback slot is typed FileTransferCallback, which ygo_types.h defines
 * as `void (*)()` -- unprototyped -- so passing this function there checks
 * nothing about its arguments, and Script_OpDuelResult only ever took its address.
 * Between them, no site in the tree could compare a declaration against the
 * definition. Declaring it in the defining unit's own header, which
 * duel_result_load_package_stage.c includes, makes the compiler check it on every build. */
void DuelResult_LoadPackageStage(FileTransferDescriptor *d, s32 mode);

#endif
