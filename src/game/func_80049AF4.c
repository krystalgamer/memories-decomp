#include "../types.h"

struct SoundState {
    u8 pad0[0x500];
    u8 field500;
    u8 pad501;
    u8 field502;
    u8 pad503[0x7DC - 0x503];
    u32 field7DC;
    s16 field7E0;
    u16 field7E2;
    u8 pad7E4[0x7E8 - 0x7E4];
    u32 field7E8;
    u32 field7EC;
};

extern struct SoundState *D_8009B458;
extern void func_8004C77C(struct SoundState *);

void func_80049AF4(s32 arg0)
{
    register s32 one asm("$17") = 1;

    D_8009B458->field500 = one;

    if (D_8009B458->field7E0 == -1) {
        D_8009B458->field500 = 0;
        return;
    }

    D_8009B458->field7EC = 0x10000;
    D_8009B458->field7DC = D_8009B458->field7E8;
    func_8004C77C(D_8009B458);

    if ((arg0 & 0xFF) == 0) {
        D_8009B458->field7E2 = 4;
    } else {
        D_8009B458->field502 = one;
        D_8009B458->field7E2 = 1;
    }

    D_8009B458->field500 = 0;
}
