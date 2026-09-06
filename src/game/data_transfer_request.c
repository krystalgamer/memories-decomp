#include "../types.h"
#include "../psyq/strings.h"

#include "mem_card.h"

extern u8 D_8009B3C1;
extern u8 D_8009B3DE;
extern u8 D_8009B3EF;
extern u16 D_8009B3FA;
extern void func_8003F454(void);

int func_8003F70C(void)
{
    func_8003F454();
    if (D_8009B3FA != 0) {
        return 0;
    }
    return D_8009B3EF;
}

void func_8003F740(s32 value)
{
    D_8009B3FA = 0x8000;
    D_8009B3DE = value;
    D_8009B3C1 = 0;
}

extern u8 D_800EFE18[];
extern u8 D_8009B3F9;
extern u8 D_8009B3DC;
extern u16 D_8009B3C2;
extern u16 D_8009B3C4;
extern u32 D_8009B3D0;

void func_8003F758(void *arg0, s32 arg1, s32 arg2, s32 arg3)
{
    strcpy(D_800EFE18, arg2);
    D_8009B3F9 = 0;
    D_8009B3C2 = arg1;
    D_8009B3C4 = 0x200;
    D_8009B3DC =
        (arg1 + MEM_CARD_BLOCK_SIZE - 1) / MEM_CARD_BLOCK_SIZE;
    D_8009B3D0 = (u32)arg0;
    func_8003F740(arg3);
}
