#include "../types.h"
#include "ai.h"

extern u8 gAiScript_State[];

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
