#include "../types.h"

#include "duel_effect.h"

extern u8 D_8009AF76[];
extern u8 D_8009B3C1;

extern void func_80043178(void *);
extern void func_8004036C(void *);
extern s32 func_8004002C(void);
extern u8 *func_800400AC();
extern void func_80042918();
extern void func_800428EC();
extern void func_80042C08();
extern void func_80043230(void *, s32, s32, s32);
extern void TextBox_SetPos(void *, s32, s32);

void func_8003D518(u8 *state)
{
    u8 *object;
    s32 flags;

    if ((D_8009B3C1 & 128) == 0) {
        D_8009B3C1 |= 128;
        object = func_800400AC(func_8004002C(), 6);
        *(s16 *)(object + 48) = 160;
        *(s16 *)(object + 50) = 120;
        *(s16 *)(object + 72) = 128;
        *(s16 *)(object + 74) = 224;
        func_80042918(object);
        func_800428EC(object, (s8)(D_8009AF76[0] - 3));
        *(s32 *)(object + 76) = (s32)func_80042C08;
        *(u8 **)(state + 4) = object;
    }
    flags = D_8009B3C1;
    object = *(u8 **)(state + 4);
    if (flags & 64) {
        D_8009B3C1 = 0;
    } else {
        s32 x = *(u16 *)(object + 72) - 8;
        s32 y = *(u16 *)(object + 74) - 8;

        *(u16 *)(object + 74) = y;
        *(u16 *)(object + 72) = x;
        if ((s16)x <= 0) {
            D_8009B3C1 = flags | 64;
            *(s16 *)(object + 72) = 0;
            *(s16 *)(object + 74) = 64;
        }
    }
}

void func_8003D614(u8 *state)
{
    u8 *object;
    DuelEffectChannel *entry;

    if (!(D_8009B3C1 & 0x80)) {
        object = *(u8 **)state;
        D_8009B3C1 |= 0x80;
        func_80043178(object);
        *(u16 *)(object + 0x60) = 0x400;
    }
    entry = &D_800EB0F8[state[0x1A]];
    object = *(u8 **)state;
    if (object) {
        *(u16 *)(object + 0x60) -= 0x40;
        if (*(s16 *)(object + 0x60) <= 0) {
            func_8004036C(object);
            *(void **)state = 0;
        } else {
            func_80043230(
                object,
                0x20,
                -0x40,
                *(s16 *)(object + 0x60)
            );
            TextBox_SetPos(
                entry,
                *(s16 *)(object + 0x30),
                *(s16 *)(object + 0x32)
            );
        }
    }
    object = *(u8 **)(state + 4);
    if (object) {
        *(u16 *)(object + 0x4A) += 8;
        *(u16 *)(object + 0x48) += 8;
        if (*(s16 *)(object + 0x48) >= 0xC0) {
            func_8004036C(object);
            *(void **)(state + 4) = 0;
        }
    }
    if (!*(void **)state && !*(void **)(state + 4))
        D_8009B3C1 = 0;
}
