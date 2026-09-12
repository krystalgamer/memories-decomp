#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../psyq/libspu.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "../unmatched.h"

/* The first three command-completion callbacks of the asynchronous
   disc-transfer runtime. Each re-issues its command on event 5 and clears the
   busy bit in D_8009B0F4 on event 2. The next callback, func_80014220, is in
   file_transfer_command_callback.c, and the rest of the runtime is in
   func_80014294.c. */

extern void func_80013C28(u8, u8 *, u32 *);

void func_800140A0(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        DsPacket(0xA0, (DslLOC *)D_8009B104, 6, (DslCB)func_800140A0, -1);
    } else if (event == 2) {
        DsReadySystemMode(1);
        DsStartReadySystem(func_80013C28, -1);
        D_8009B114 = 0;
        D_8009B138 = 0;
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

void func_80014134(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        DsPacket(0xA0, (DslLOC *)D_8009B104, 0x15, (DslCB)func_80014134, -1);
    } else if (event == 2) {
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

void func_800141A8(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        DsCommand(9, 0, (DslCB)func_800141A8, -1);
    } else if (event == 2) {
        gFile_PrimaryTransferDescriptor.substate = 1;
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}
