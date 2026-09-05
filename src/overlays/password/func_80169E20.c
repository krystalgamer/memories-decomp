#include "../../types.h"

extern s32 func_80042B98(void);
extern void func_800429D8(u8 *);
extern void func_80042A78(u8 *);

void func_80169E20(u8 *object)
{
    s16 remaining;

    object[0x22] = object[0x22] + 1;
    if ((object[0x6C] & 0x40) != 0) {
        if (func_80042B98() == 0) {
            func_800429D8(object);
            *(s16 *)(object + 0x36) =
                ((*(s16 *)(object + 0x18) - *(s16 *)(object + 0x30)) << 8) /
                *(s16 *)(object + 0x60);
            *(s16 *)(object + 0x38) =
                ((*(s16 *)(object + 0x1A) - *(s16 *)(object + 0x32)) << 8) /
                *(s16 *)(object + 0x60);
        }
        func_80042A78(object);
        remaining = *(u16 *)(object + 0x60) - 1;
        *(s16 *)(object + 0x60) = remaining;
        if (remaining <= 0) {
            *(s32 *)(object + 0x30) = *(s32 *)(object + 0x18);
            object[0x6C] = object[0x6C] & 0x3F;
        }
    }
}
