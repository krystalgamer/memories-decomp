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
        D_8009B100 = event;
        D_8009B0F4 &= ~0x400;
    }
}
