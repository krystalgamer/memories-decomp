#include "../types.h"
#include "../psyq/libds.h"
#include "file_transfer.h"
#include "file_cd_transfer.h"

extern char D_8009B11C[1];
extern u8 D_8009B114;
extern s32 D_8009B138;
extern u8 D_801D4200[];

extern void func_80013C28(u8, u8 *, u32 *);
extern s32 CdPosToInt_8007E710(s32);

void File_ReadNCB(u8 event)
{
    if (event == DslDiskError) {
        D_8009B130++;
        DsPacket(DslModeSpeed | DslModeSize1, (DslLOC *)D_8009B104, DslReadN,
                 (DslCB)File_ReadNCB, -1);
    } else if (event == DslComplete) {
        DsReadySystemMode(1);
        DsStartReadySystem(func_80013C28, -1);
        D_8009B114 = 0;
        D_8009B138 = 0;
        D_8009B0F4 &= ~0x400;
    }
}

void File_SeekLCB(u8 event)
{
    if (event == DslDiskError) {
        D_8009B130++;
        DsPacket(DslModeSpeed | DslModeSize1, (DslLOC *)D_8009B104, DslSeekL,
                 (DslCB)File_SeekLCB, -1);
    } else if (event == DslComplete) {
        D_8009B0F4 &= ~0x400;
    }
}

void File_PauseCB(u8 event)
{
    if (event == DslDiskError) {
        D_8009B130++;
        DsCommand(DslPause, 0, (DslCB)File_PauseCB, -1);
    } else if (event == DslComplete) {
        gFile_PrimaryTransferDescriptor.substate = 1;
        D_8009B0F4 &= ~0x400;
    }
}

void File_XaPauseCB(s32 event)
{
    event &= 0xFF;
    if (event == DslDiskError) {
        D_8009B130++;
        DsCommand(DslPause, 0, (DslCB)File_XaPauseCB, -1);
    } else if (event == DslComplete) {
        __asm__ volatile(
            "sh $4, %%gp_rel(D_8009B100)($28)"
            : : : "memory"
        );
        D_8009B0F4 &= ~0x400;
    }
}

void File_XaSetfilterCB(u8 event)
{
    if (event == DslDiskError) {
        D_8009B130++;
        DsCommand(DslSetfilter, (u8 *)D_8009B11C, (DslCB)File_XaSetfilterCB,
                  -1);
    } else if (event == DslComplete) {
        D_8009B100 = 4;
        D_8009B0F4 &= ~0x400;
    }
}

void File_XaReadSCB(u8 event)
{
    if (event == DslDiskError) {
        D_8009B130++;
        DsPacket(DslModeRT | DslModeSF | DslModeAP, (DslLOC *)D_8009B104,
                 DslReadS, (DslCB)File_XaReadSCB, -1);
    } else if (event == DslComplete) {
        D_8009B100 = 5;
        D_8009B0F4 |= 0x1000;
        D_8009B0F4 &= ~0x400;
    }
}

void File_XaGetlocLCB(u8 event, s32 arg1)
{
    s32 value;
    s32 *destination;

    if (event == DslComplete) {
        destination = (s32 *)&gFile_PrimaryTransferDescriptor.field_30;
        value = CdPosToInt_8007E710(arg1);
        if (value > 0)
            *destination = value;
        D_8009B0F4 &= ~0x800;
    }
}

void File_ActivateTransfer(void) {
    *(FileTransferDescriptorWords *)&gFile_PrimaryTransferDescriptor =
        *(FileTransferDescriptorWords *)&gFile_SecondaryTransferDescriptor;
    *(FileRequestSlot *)D_801D4200 = *(FileRequestSlot *)(D_801D4200 + 32);
    if (gFile_PrimaryTransferDescriptor.done == 4)
        D_8009B112 |= 1;
    D_8009B0F4 =
        gFile_PrimaryTransferDescriptor.status_flags |
        FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
}
