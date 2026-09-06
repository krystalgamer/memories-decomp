#include "../../types.h"

extern s32 gPassword_nDigitIndex;

void Password_SetDigitCursorTarget(u8 *a)
{
    s32 value = gPassword_nDigitIndex;

    *(s16 *)(a + 0x1A) = 0x63;
    *(s16 *)(a + 0x18) = value * 16 + 0xA3;
}
