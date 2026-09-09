#include "../types.h"
#define AI_HAND_SIZE_RETURNS_S32
#include "ai.h"

void Ai_GetWinningCardRange(s32 kind, s32 *low, s32 *high)
{
    s32 value;

    switch (kind) {
    case 0:
    case 1:
        *low = AI_SLOT_OWN_MONSTER_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OWN_MONSTER_FIRST);
        break;
    case 2:
        *low = AI_SLOT_OWN_HAND_FIRST;
        value = Ai_GetHandSize() + AI_SLOT_OWN_HAND_FIRST - 1;
        break;
    case 3:
    case 4:
    case 5:
        *low = AI_SLOT_OPPONENT_MONSTER_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST);
        break;
    case 6:
        *low = AI_SLOT_OPPONENT_HAND_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_HAND_FIRST);
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
        *low = AI_SLOT_OWN_MONSTER_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OWN_MONSTER_FIRST);
        break;
    case 2:
    case 3:
        *low = AI_SLOT_OWN_SPELL_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OWN_SPELL_FIRST);
        break;
    case 4:
        *low = AI_SLOT_OWN_HAND_FIRST;
        value = Ai_GetHandSize() + AI_SLOT_OWN_HAND_FIRST - 1;
        break;
    case 5:
    case 6:
        *low = AI_SLOT_OPPONENT_MONSTER_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST);
        break;
    case 7:
    case 8:
        *low = AI_SLOT_OPPONENT_SPELL_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_SPELL_FIRST);
        break;
    case 9:
        *low = AI_SLOT_OPPONENT_HAND_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_HAND_FIRST);
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

    value = gDuel_aActiveCards[arg0].card_id;
    for (i = 0; i < AI_SCRIPT_CARD_SET_COUNT; i++) {
        if (value == gAiScript_State.card_set[i])
            return 1;
    }
    return 0;
}

s32 Ai_IsTypeInSet(s32 arg0)
{
    s32 value;
    s32 i;

    value = gDuel_aActiveCards[arg0].card_type;
    i = 0;
    for (; i < AI_SCRIPT_TYPE_SET_COUNT; i++) {
        if (value == gAiScript_State.type_set[i] -
                     AI_SCRIPT_TYPE_SET_ENCODING_BIAS)
            return 1;
    }
    return 0;
}
