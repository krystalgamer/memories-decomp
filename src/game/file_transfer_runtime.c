#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../psyq/libspu.h"
#include "file_constants.h"
#include "file_transfer.h"
#define FUNC_80013C28_CALLBACK_VIEW
#include "func_80013C28.h"
#include "../unmatched.h"
#undef FUNC_80013C28_CALLBACK_VIEW

/* The first four command-completion callbacks of the asynchronous
   disc-transfer runtime. Each re-issues its command on event 5 and clears the
   busy bit in D_8009B0F4 on event 2. The rest of the runtime is in
   func_80014294.c. */

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
void func_80014220(s32 event)
{
    event &= 0xFF;
    if (event == 5) {
        D_8009B130++;
        DsCommand(9, 0, (DslCB)func_80014220, -1);
    } else if (event == 2) {
        D_8009B100 = event;
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}
