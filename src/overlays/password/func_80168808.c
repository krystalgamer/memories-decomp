#include "../../types.h"

extern void func_800429D8(void);
extern s32 func_800358FC(s32);
extern s32 func_80042B08(s32, s32);
extern s32 func_80042AD8(s32, s32, s32);
extern void func_80042A78(void *);
extern void func_8004036C(void *);

void func_80168808(u8 *object)
{
    u8 flags;

    flags = object[0x6C];
    if ((flags & 0x80) == 0) {
        object[0x6C] = flags | 0x80;
        func_800429D8();
        *(s16 *)(object + 0x36) = func_800358FC(0x200) - 0x100;
        *(s16 *)(object + 0x38) = -func_800358FC(0x180);
    }
    *(s16 *)(object + 0x36) = func_80042B08(*(s16 *)(object + 0x36), 8);
    *(s16 *)(object + 0x38) = func_80042AD8(*(s16 *)(object + 0x38), 0x800, 0x40);
    func_80042A78(object);
    if (*(s16 *)(object + 0x32) >= 0xF0) {
        func_8004036C(object);
    }
}
