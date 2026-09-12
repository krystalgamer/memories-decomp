#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../psyq/libspu.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "../unmatched.h"

/* The rest of the asynchronous disc-transfer runtime: the later
   command-completion callbacks, secondary-to-primary activation, transfer
   advancement and request dispatch. The nine functions are contiguous and
   communicate through the shared descriptors, request slots and D_8009B0F4
   state word. The first three callbacks are in file_transfer_runtime.c, and
   func_80014220 between the two runs is a candidate since #3859
   (src/candidates/func_80014220.c). */

extern void func_80014B30_callback(void) asm("func_80014B30");
extern s32 CdPosToInt_8007E710(s32);
extern u16 D_8009B0EC;
extern void CdIntToPos_8007E600(s32, void *);

void func_80014294(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        DsCommand(0xD, (u8 *)D_8009B11C, (DslCB)func_80014294, -1);
    } else if (event == 2) {
        D_8009B100 = 4;
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

void func_80014308(u8 event)
{
    if (event == 5) {
        D_8009B130++;
        DsPacket(0x4A, (DslLOC *)D_8009B104, 0x1B, (DslCB)func_80014308, -1);
    } else if (event == 2) {
        D_8009B100 = 5;
        D_8009B0F4 |= FILE_TRANSFER_STATE_POSITION_QUERY_PENDING;
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
}

void func_80014390(u8 event, s32 arg1)
{
    s32 value;
    s32 *destination;

    if (event == 2) {
        destination = (s32 *)&gFile_PrimaryTransferDescriptor.field_30;
        value = CdPosToInt_8007E710(arg1);
        if (value > 0)
            *destination = value;
        D_8009B0F4 &= ~FILE_TRANSFER_STATE_POSITION_QUERY_BUSY;
    }
}

void File_ActivateTransfer(void)
{
    *(FileTransferDescriptorWords *)&gFile_PrimaryTransferDescriptor =
        *(FileTransferDescriptorWords *)&gFile_SecondaryTransferDescriptor;
    *(FileRequestSlot *)D_801D4200_raw =
        *(FileRequestSlot *)(D_801D4200_raw + 32);
    if (gFile_PrimaryTransferDescriptor.done == 4)
        D_8009B112 |= 1;
    D_8009B0F4 =
        gFile_PrimaryTransferDescriptor.status_flags |
        FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
}

void func_800144B8(void){D_8009B0F4&=FILE_TRANSFER_STATE_SECONDARY_PENDING|FILE_TRANSFER_STATE_PRIMARY_REQUEST_LOCKED;if((D_8009B0F4&FILE_TRANSFER_STATE_SECONDARY_PENDING)&&!(D_8009B0F4&FILE_TRANSFER_STATE_PRIMARY_REQUEST_LOCKED)){File_ActivateTransfer();if(D_8009B134){int v=0x80;if((D_8009B0F4&FILE_TRANSFER_STATE_PRIMARY_ACTIVE)&&(D_8009B0F4&FILE_TRANSFER_FLAG_SECTOR_RANGE))func_80015010();D_8009B134=v;}}else D_8009B134=0;}

void func_8001455C(void)
{
    u8 *p;
    void (*cb)(void);
    void (*cb2)(void);
    u8 *q;
    void (*step)(u8 *, s32);
    s32 n;
    s32 m;

    p = (u8 *)&gFile_PrimaryTransferDescriptor;
    if (D_8009B0F4 & FILE_TRANSFER_STATE_POSITION_QUERY_PENDING) {
        if (!(D_8009B0F4 & FILE_TRANSFER_STATE_POSITION_QUERY_BUSY)) {
            if (DsCommand(0x10, 0, (DslCB)func_80014390, 0) > 0) {
                D_8009B0F4 =
                    D_8009B0F4 | FILE_TRANSFER_STATE_POSITION_QUERY_BUSY;
            }
        }
    }
    if (D_8009B0F4 & FILE_TRANSFER_STATE_COMMAND_BUSY) {
        return;
    }
    if (D_8009B0F4 & FILE_TRANSFER_FLAG_SECTOR_RANGE) {
        if (!(D_8009B112 & 0x8000)) {
            D_8009B112 = D_8009B112 | 0x8000;
            if ((D_8009B112 & 3) == 0) {
                D_8009B112 = 0;
                goto call_144B8;
            }
            if (!(D_8009B112 & 0x2000)) {
                D_8009B100 = 0;
            }
            if (D_8009B112 & 2) {
                D_8009B112 = D_8009B112 & 0xFFFE;
            } else {
                *(s32 *)(p + 0x30) = 0;
            }
        }
        switch (D_8009B100) {
        case 0:
            D_8009B112 = D_8009B112 | 0x2000;
            D_8009B100 = 1;
        case 1:
            if (DsCommand(9, 0, (DslCB)func_80014220, -1) <= 0) {
                return;
            }
            D_8009B0F4 = D_8009B0F4 | FILE_TRANSFER_STATE_COMMAND_BUSY;
            cb = D_8009B120;
            goto call_back;
        case 2:
            D_8009B112 = D_8009B112 & 0xDFFF;
            if (D_8009B112 & 1) {
                goto set_state3;
            }
            D_8009B112 = 0;
call_144B8:
            func_800144B8();
            return;
set_state3:
            D_8009B100 = 3;
        case 3:
            D_8009B112 = D_8009B112 | 0x1000;
            q = &D_8009B11C_byte + 1;
            *q = p[0x38];
            q[-1] = p[0x39];
            if (DsCommand(0xD, (u8 *)(q - 1), (DslCB)func_80014294, -1) <= 0) {
                return;
            }
            D_8009B0F4 = D_8009B0F4 | FILE_TRANSFER_STATE_COMMAND_BUSY;
            return;
        case 4:
            CdIntToPos_8007E600(*(s32 *)(p + 0x24), D_8009B104);
            if (DsPacket(0x4A, (DslLOC *)D_8009B104, 0x1B, (DslCB)func_80014308, -1) <= 0) {
                return;
            }
            D_8009B0F4 = D_8009B0F4 | FILE_TRANSFER_STATE_COMMAND_BUSY;
            return;
        case 5:
            D_8009B100 = 6;
            D_8009B0EC = 0x258;
            D_8009B112 = D_8009B112 & 0xEFFF;
            D_8009B112 = D_8009B112 | 0x4000;
            cb2 = D_8009B0F0;
            if (cb2 != 0) {
                cb2();
            }
        case 6:
            D_8009B0EC = D_8009B0EC - 1;
            if ((s16)D_8009B0EC > 0) {
                if (*(s32 *)(p + 0x30) < *(s32 *)(p + 0x34)) {
                    return;
                }
            }
            D_8009B112 = D_8009B112 & 0x3FFC;
            cb = D_8009B120;
            D_8009B112 = D_8009B112 | 2;
call_back:
            if (cb != 0) {
                cb();
            }
        }
        return;
    }
    if (p[FILE_TRANSFER_DESCRIPTOR_STATE_BYTE_OFFSET] == 5) {
        switch (p[FILE_TRANSFER_DESCRIPTOR_SUBSTATE_BYTE_OFFSET]) {
        case 0:
            DsEndReadySystem();
            CdReadyCallback(0);
            if (DsCommand(9, 0, (DslCB)func_800141A8, -1) <= 0) {
                return;
            }
            D_8009B0F4 = D_8009B0F4 | FILE_TRANSFER_STATE_COMMAND_BUSY;
            return;
        case 1:
            DsEndReadySystem();
            CdReadyCallback(0);
            goto call_144B8;
        }
        return;
    }
    if (D_8009B0F4 & 0x80) {
        if (D_8009B0F4 & 0x100) {
            return;
        }
        goto call_144B8;
    }
    CdIntToPos_8007E600(*(s32 *)(p + 0x24), D_8009B104);
    if (D_8009B0F4 & 0x100000) {
        if ((s32)D_8009B0F4 < 0) {
            goto call_144B8;
        }
        if (DsPacket(0xA0, (DslLOC *)D_8009B104, 0x15, (DslCB)func_80014134, -1) <= 0) {
            return;
        }
        D_8009B0F4 = D_8009B0F4 | 0x480;
        return;
    }
    if (!(D_8009B0F4 & 0x800000)) {
        D_8009B0F4 = D_8009B0F4 | 0x800000;
        step = *(void (**)(u8 *, s32))(p + 0x20);
        if (step != 0) {
            step(p, (*(s32 *)(p + 0x40))++);
        }
        *(s32 *)(p + 0x28) = *(s32 *)(p + 0x1C);
        return;
    }
    if (D_8009B0F4 & 0x400000) {
        if (SpuIsTransferCompleted(0) == 0) {
            return;
        }
        D_8009B0F4 = D_8009B0F4 & 0xFFBFFFFF;
    }
    if ((s32)D_8009B0F4 >= 0) {
        if (DsPacket(0xA0, (DslLOC *)D_8009B104, 6, (DslCB)func_800140A0, -1) == 0) {
            return;
        }
        D_8009B0F4 = D_8009B0F4 | FILE_TRANSFER_STATE_COMMAND_BUSY;
    }
    D_8009B0F4 = D_8009B0F4 | 0x180;
}

void func_80014A5C(s32 arg0)
{
    if (D_8009B124 != 0) {
        D_8009B124 = 0;
        return;
    }
    D_8009B124 = 1;
    if (D_8009B0E8 != 0) {
        return;
    }
    D_8009B0E8 = 1;
    if (!(D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) &&
        (D_8009B0F4 & FILE_TRANSFER_STATE_SECONDARY_PENDING)) {
        File_ActivateTransfer();
    }
    if (D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) {
        if (D_8009B134 != 0 && !(D_8009B134 & 0x40)) {
            D_8009B134 |= 0x40;
            gFile_PrimaryTransferDescriptor.done = 5;
            gFile_PrimaryTransferDescriptor.substate = 0;
        }
        func_8001455C();
    } else {
        D_8009B134 = 0;
    }
    D_8009B0E8 = 0;
}

/* func_80014B30 is a transfer-phase callback of the same family as
   func_80057544 and func_80057728: func_80014C40 below installs it through
   File_InitTransferDescriptor's FileTransferCallback parameter, so its first
   argument is the descriptor that entry point fills in. It programs the same
   fields the other two do -- the value_08/value_0C source window, mode, the
   word at field_30 and done -- which is what its old private record named
   value_8, value_c, value_1c, value_30 and mode_46. */
void func_80014B30(FileTransferDescriptor *object, s32 mode)
{
    FileRequestSlot *shared;
    s32 value;
    s32 base;
    s32 position;
    void (*callback)(void);

    shared = &D_801D4200;
    if (mode == 1)
        goto reduced;
    if (mode >= 2)
        goto high;
    if (mode == 0)
        goto full;
    return;
high:
    if (mode == 2)
        goto tail;
    return;
full:
    if (shared->field_14 == 0) {
        object->result = 2;
        goto reduced;
    }
    object->done = 3;
    base = D_8009B118;
    object->value_08 = base;
    object->value_0C = base + FILE_SECTOR_SIZE;
    object->field_30.word = shared->field_0C;
    value = shared->field_14;
    object->mode = value;
    goto fix;
reduced:
    if (shared->field_18 == 0)
        goto tail;
    position = shared->field_10;
    D_8009B0F4 &= 0xFFDCFFFF;
    object->value_0C = position;
    object->value_08 = position;
    object->done = 1;
    value = shared->field_18;
    object->mode = value;
fix:
    if (value < 0)
        object->mode = -(value << FILE_SECTOR_SHIFT);
    return;
tail:
    callback = D_8009B128;
    if (callback != 0)
        callback();
}

s32 func_80014C40(FileRequestSlot *p, u8 *q) {
    u8 *e;
    u8 *r;
    FileTransferCallback f;
    s32 a;
    s32 b;
    s32 c;
    s32 n;
    s32 m;
    s32 v;
    s32 w;
    s32 t;

    if (p == (FileRequestSlot *)0) {
        return D_8009B0F4 & FILE_TRANSFER_STATE_SECONDARY_PENDING;
    }

    a = p->field_14;
    b = p->field_18;
    c = p->field_1C;

    if ((a | b | c) == 0) {
        return (s32)func_80013A94(p->field_00, p->field_04);
    }

    if (c != 0) {
        if (c < 0) {
            D_8009B112 = D_8009B112 & 0x3FFC;
            D_8009B112 = D_8009B112 | 2;
            return 1;
        }
        n = *(s32 *)D_800E9EC0 + p->field_04;
        return (s32)File_RequestSecondaryRangeTransfer(
            n, n + c, p->field_1F, p->field_1E
        );
    }

    a = a + b;
    if (a == 0) {
        return 0;
    }

    {
        w = p->field_00;
        /* Borrowed local: `m`'s real assignment is three lines down and this
         * one is dead, but it ties the negation's pseudo to m's allocation
         * and rotates $s2/$s3/$s4 into retail's order. A fresh name, six
         * declaration orders and four statement orders are all 6. */
        m = a;
        t = -m;
        r = D_801D4200_raw;
        *(FileRequestSlot *)(r + 0x20) = *p;
        /* The post-copy reload's scalar view preserves GCC's allocation. */
        m = *(s32 *)((u8 *)p + 4);
        D_8009B0F4 =
            D_8009B0F4 & ~FILE_TRANSFER_STATE_SECONDARY_PENDING;
        v = w | 0x1400000;
        f = func_80014B30_callback;

        if ((D_8009B0F4 & FILE_TRANSFER_STATE_PRIMARY_ACTIVE) != 0) {
            if ((D_8009B0F4 & FILE_TRANSFER_FLAG_SECTOR_RANGE) != 0) {
                func_80015010();
            }
        }

        File_InitTransferDescriptor(
            &gFile_SecondaryTransferDescriptor, v, q, m, t, f, 0, (s32)p
        );
        e = (u8 *)&gFile_SecondaryTransferDescriptor;
        D_8009B0F4 =
            D_8009B0F4 | FILE_TRANSFER_STATE_SECONDARY_PENDING;
    }

    return (s32)e;
}
