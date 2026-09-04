#include "../types.h"

extern s32 D_8009B400[];
extern s32 D_8009B404[];
extern s32 func_80036D3C(void *);
extern void func_8003FF08(s32);
extern void func_8003FF88(u16);
extern void SD_SEPlayFull(u16);
extern u16 D_8009B33C;
extern s32 D_8009B350;

void func_80038690(void *object)
{
    func_8003FF08((u16)func_80036D3C(object));
}

void func_800386B8(u8 *object)
{
    u8 **slot = (u8 **)(object + *(s8 *)(object + 0x58) * 4);
    u8 *stream = *slot;
    s32 command = *stream;
    s32 op;

    *slot = stream + 1;
    op = command;
    if (op & 0x3F) {
        func_8003FF08(func_80036D3C(object) & 0xFFFF);
    } else {
        if (op & 1) {
            func_8003FF08(D_8009B404[0]);
        }
        if (op & 2) {
            D_8009B404[0] = func_80036D3C(object) & 0xFFFF;
        }
        if (op & 4) {
            D_8009B404[0] = D_8009B400[0];
        }
    }
    if (op & 0x80) {
        object[0x51] = 0xC;
        D_8009B350 = 1;
    }
}

void DuelEffect_PlaySoundCommand(u8 *object) {
    u32 value = func_80036D3C(object);

    value &= 0xFFFF;
    if (value & 0x8000) {
        func_8003FF88((u16)value);
        D_8009B33C = func_80036D3C(object);
        object[0x51] = 0x11;
        D_8009B350 = 1;
    } else {
        SD_SEPlayFull((u16)value);
    }
}
