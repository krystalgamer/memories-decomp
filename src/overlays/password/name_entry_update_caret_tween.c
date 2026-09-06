#include "../../types.h"

extern void func_800429D8(void);
extern void func_80042A78(void *);

void NameEntry_UpdateCaretTween(u8 *object)
{
    u8 flags;
    s16 remaining;

    flags = object[0x6C];
    if ((flags & 0x80) == 0) {
        object[0x6C] = flags | 0x80;
        func_800429D8();
        *(s16 *)(object + 0x36) =
            ((*(s16 *)(object + 0x44) - *(s16 *)(object + 0x30)) << 8) /
            *(s16 *)(object + 0x60);
        *(s16 *)(object + 0x38) =
            ((*(s16 *)(object + 0x46) - *(s16 *)(object + 0x32)) << 8) /
            *(s16 *)(object + 0x60);
    }
    func_80042A78(object);
    remaining = *(u16 *)(object + 0x60) - 1;
    *(s16 *)(object + 0x60) = remaining;
    if (remaining <= 0) {
        *(s32 *)(object + 0x24) = 0;
        object[0x6C] = 0;
        *(s32 *)(object + 0x30) = *(s32 *)(object + 0x44);
    }
}
