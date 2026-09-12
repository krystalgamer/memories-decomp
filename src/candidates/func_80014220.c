/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/file_transfer_runtime.c.
 */
#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../psyq/libspu.h"
#include "../game/file_constants.h"
#include "../game/file_transfer.h"
#include "../unmatched.h"

/* The asynchronous disc-transfer runtime: command-completion callbacks,
   secondary-to-primary activation, transfer advancement and request dispatch.
   The thirteen functions are contiguous and communicate through the shared
   descriptors, request slots and D_8009B0F4 state word. */

void func_80014220(s32 event)
{
    event &= 0xFF;
    if (event == 5) {
        D_8009B130++;
        DsCommand(9, 0, (DslCB)func_80014220, -1);
    } else if (event == 2) {
        __asm__ volatile(
            "sh $4, %%gp_rel(D_8009B100)($28)"
            : : : "memory"
        );
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}
