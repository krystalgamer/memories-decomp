#include "../types.h"

s32 Ai_GetHandSize(void);

void Ai_GetWinningCardRange(s32 kind, s32 *low, s32 *high)
{
    s32 value;

    switch (kind) {
    case 0:
    case 1:
        *low = 1;
        value = 5;
        break;
    case 2:
        *low = 0xB;
        value = Ai_GetHandSize() + 0xA;
        break;
    case 3:
    case 4:
    case 5:
        *low = 0x38;
        value = 0x3C;
        break;
    case 6:
        *low = 0x42;
        value = 0x46;
        break;
    default:
        return;
    }

    *high = value;
}

void Ai_GetCardRange(s32 kind, s32 *low, s32 *high)
{
    s32 value;

    switch (kind) {
    case 0:
    case 1:
        *low = 1;
        value = 5;
        break;
    case 2:
    case 3:
        *low = 6;
        value = 0xA;
        break;
    case 4:
        *low = 0xB;
        value = Ai_GetHandSize() + 0xA;
        break;
    case 5:
    case 6:
        *low = 0x38;
        value = 0x3C;
        break;
    case 7:
    case 8:
        *low = 0x3D;
        value = 0x41;
        break;
    case 9:
        *low = 0x42;
        value = 0x46;
        break;
    default:
        return;
    }

    *high = value;
}
