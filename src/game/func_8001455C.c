#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "../psyq/libspu.h"
#include "file_constants.h"

extern u8 gFile_PrimaryTransferDescriptor[];
extern volatile s32 D_8009B0F4;
extern volatile u16 D_8009B112;
extern volatile u16 D_8009B100;
extern u16 D_8009B0EC;
extern void (*D_8009B120)(void);
extern void (*D_8009B0F0)(void);
extern u8 D_8009B11C;
extern char D_8009B104[1];

extern void func_80014390(void);
extern void func_80014308(void);
extern void func_80014294(void);
extern void func_80014220(void);
extern void func_800141A8(void);
extern void func_80014134(void);
extern void func_800140A0(void);

extern s32 func_8007B1F4(s32, void *, void *, s32);
extern s32 func_8007B468(s32, void *, s32, void *, s32);
extern void CdIntToPos_8007E600(s32, void *);
extern void func_800144B8(void);

void func_8001455C(void)
{
    u8 *p;
    void (*cb)(void);
    void (*cb2)(void);
    u8 *q;
    void (*step)(u8 *, s32);
    s32 n;
    s32 m;

    p = gFile_PrimaryTransferDescriptor;
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
        if (D_8009B0F4 < 0) {
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
    if (D_8009B0F4 >= 0) {
        if (func_8007B468(0xA0, D_8009B104, 6, func_800140A0, -1) == 0) {
            return;
        }
        D_8009B0F4 = D_8009B0F4 | 0x400;
    }
    D_8009B0F4 = D_8009B0F4 | 0x180;
}
