#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../psyq/libspu.h"
#include "file_constants.h"
#include "file_transfer.h"
#include "file_cd_transfer.h"
#include "func_800144B8.h"
#include "../unmatched.h"

/* The primary transfer's step function (0x8001455C) and the three control
   entry points that drive it (0x80014A5C, 0x80014B30, 0x80014C40). The four
   are the whole gcc_2_8_1_g8_split run here: below them func_800144B8
   compiles at gcc_2_8_1_g8, and above them file_transfer_flags.c does too.
   file_transfer_control.c already declared func_8001455C and called it, so
   the caller and the callee now share a unit and that prototype is gone. */

extern u16 D_8009B0EC;
extern void (*D_8009B120)(void);
extern void (*D_8009B0F0)(void);
extern u8 D_8009B11C;
extern s32 func_8007B1F4(s32, void *, void *, s32);
extern s32 func_8007B468(s32, void *, s32, void *, s32);
extern void CdIntToPos_8007E600(s32, void *);
extern volatile u16 D_8009B124;
extern volatile s32 D_8009B0E8;
extern u8 D_800E9EC0[];

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
    if (D_8009B0F4 & 0x1000) {
        if (!(D_8009B0F4 & 0x800)) {
            if (func_8007B1F4(0x10, 0, func_80014390, 0) > 0) {
                D_8009B0F4 = D_8009B0F4 | 0x800;
            }
        }
    }
    if (D_8009B0F4 & 0x400) {
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
            if (func_8007B1F4(9, 0, func_80014220, -1) <= 0) {
                return;
            }
            D_8009B0F4 = D_8009B0F4 | 0x400;
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
            q = &D_8009B11C + 1;
            *q = p[0x38];
            q[-1] = p[0x39];
            if (func_8007B1F4(0xD, q - 1, func_80014294, -1) <= 0) {
                return;
            }
            D_8009B0F4 = D_8009B0F4 | 0x400;
            return;
        case 4:
            CdIntToPos_8007E600(*(s32 *)(p + 0x24), D_8009B104);
            if (func_8007B468(0x4A, D_8009B104, 0x1B, func_80014308, -1) <= 0) {
                return;
            }
            D_8009B0F4 = D_8009B0F4 | 0x400;
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
            if (func_8007B1F4(9, 0, func_800141A8, -1) <= 0) {
                return;
            }
            D_8009B0F4 = D_8009B0F4 | 0x400;
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
        if (func_8007B468(0xA0, D_8009B104, 0x15, func_80014134, -1) <= 0) {
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
        if (func_8007B468(0xA0, D_8009B104, 6, func_800140A0, -1) == 0) {
            return;
        }
        D_8009B0F4 = D_8009B0F4 | 0x400;
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

typedef struct {
    u8 gap0[12];
    s32 value_c;
    s32 value_10;
    s32 value_14;
    s32 value_18;
} Shared14B30;

typedef struct {
    u8 gap0[8];
    s32 value_8;
    s32 value_c;
    u8 gap10[12];
    s32 value_1c;
    u8 gap20[16];
    s32 value_30;
    u8 gap34[12];
    s32 value_40;
    u8 gap44[2];
    u8 mode_46;
} Object14B30;

extern Shared14B30 D_801D4200;
extern u8 D_801D4200_raw[] asm("D_801D4200");
extern void (*D_8009B128)(void);
extern void func_80014B30_callback(void) asm("func_80014B30");

typedef struct {
    s32 w[8];
} Blk32;

void func_80014B30(Object14B30 *object, s32 mode)
{
    Shared14B30 *shared;
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
    if (shared->value_14 == 0) {
        object->value_40 = 2;
        goto reduced;
    }
    object->mode_46 = 3;
    base = D_8009B118;
    object->value_8 = base;
    object->value_c = base + FILE_SECTOR_SIZE;
    object->value_30 = shared->value_c;
    value = shared->value_14;
    object->value_1c = value;
    goto fix;
reduced:
    if (shared->value_18 == 0)
        goto tail;
    position = shared->value_10;
    D_8009B0F4 &= 0xFFDCFFFF;
    object->value_c = position;
    object->value_8 = position;
    object->mode_46 = 1;
    value = shared->value_18;
    object->value_1c = value;
fix:
    if (value < 0)
        object->value_1c = -(value << FILE_SECTOR_SHIFT);
    return;
tail:
    callback = D_8009B128;
    if (callback != 0)
        callback();
}

s32 func_80014C40(u8 *p, u8 *q) {
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

    if (p == (u8 *)0) {
        return D_8009B0F4 & FILE_TRANSFER_STATE_SECONDARY_PENDING;
    }

    a = *(s32 *)(p + 0x14);
    b = *(s32 *)(p + 0x18);
    c = *(s16 *)(p + 0x1C);

    if ((a | b | c) == 0) {
        return (s32)func_80013A94(*(s32 *)p, *(s32 *)(p + 4));
    }

    if (c != 0) {
        if (c < 0) {
            D_8009B112 = D_8009B112 & 0x3FFC;
            D_8009B112 = D_8009B112 | 2;
            return 1;
        }
        n = *(s32 *)D_800E9EC0 + *(s32 *)(p + 4);
        return (s32)File_RequestSecondaryRangeTransfer(
            n, n + c, p[0x1F], p[0x1E]
        );
    }

    a = a + b;
    if (a == 0) {
        return 0;
    }

    {
        w = *(s32 *)p;
        /* Borrowed local: `m`'s real assignment is three lines down and this
         * one is dead, but it ties the negation's pseudo to m's allocation
         * and rotates $s2/$s3/$s4 into retail's order. A fresh name, six
         * declaration orders and four statement orders are all 6. */
        m = a;
        t = -m;
        r = D_801D4200_raw;
        *(Blk32 *)(r + 0x20) = *(Blk32 *)p;
        m = *(s32 *)(p + 4);
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
