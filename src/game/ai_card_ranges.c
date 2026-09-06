#include "../types.h"
#include "ai.h"

s32 Ai_GetHandSize(void);

extern u8 gAiScript_State[];

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

s32 Ai_IsCardInSet(s32 arg0)
{
    s32 value;
    s32 i;
    u16 *set;

    value = gDuel_aActiveCards[arg0].card_id;
    set = (u16 *)gAiScript_State;
    for (i = 0; i < AI_SCRIPT_CARD_SET_COUNT; i++) {
        if (value == set[i + 0x1F])
            return 1;
    }
    return 0;
}

s32 Ai_IsTypeInSet(s32 arg0)
{
    s32 value;
    s32 i;
    u8 *set;

    value = gDuel_aActiveCards[arg0].card_type;
    i = 0;
    set = gAiScript_State;
    for (; i < AI_SCRIPT_TYPE_SET_COUNT; i++) {
        if (value == set[i + 0x7E] - 1)
            return 1;
    }
    return 0;
}
