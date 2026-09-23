#include "../types.h"
#include "../psyq/rand.h"
#include "../psyq/stdio.h"
#define AI_GET_HAND_SIZE_RETURNS_S32
#include "ai.h"
#include "ai_script_commands.h"
#include "ai_script_control_flow.h"
#include "ai_script_read_byte.h"
#include "ai_script_read_short.h"
#include "ai_script_source_line_format.h"
#include "../unmatched.h"

#ifndef VERSION_JAPAN
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
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_AI_IS_CARD_IN_SET)
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
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_AI_IS_TYPE_IN_SET)
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
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_AI_IS_CARD_IN_SETS)
s32 Ai_IsCardInSets(s32 mode, s32 index)
{
    if (mode == 1 && Ai_IsCardInSet(index))
        return 1;
    if (mode == 2 && Ai_IsTypeInSet(index))
        return 1;
    return 0;
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_AI_SCRIPT_JUMP)
void AiScript_Jump(void)
{
    s32 result = AiScript_ReadShort();
    register AiScriptState *state = &gAiScript_State;

    state->script_cursor =
        (u8 *)(result + (s32)state->script_base);
}
#endif

#ifndef VERSION_JAPAN
/* The AI script VM's control-flow opcodes: the six conditional jumps, which
   read register operands and a script-relative offset and move
   gAiScript_State.script_cursor when their test holds, then the call and
   return pair that push and pop that cursor on the return stack. The final
   random-assignment and subtraction opcodes are data operations over the same
   readers and register file.

   The former support and subtract objects were recorded at gcc_2_8_1_g0 and
   are byte-identical at gcc_2_8_1_g0_split. The nine-function control object
   is byte-identical at gcc_2_8_1_g0_split and gcc_2_8_1_g8_split. */

void AiScript_JumpGreaterEqual(void)
{
    s32 a = AiScript_ReadByte();
    s32 b = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();
    register s32 *values = gAiScript_aMemory;

    if (values[a] >= values[b]) {
        register AiScriptState *s = &gAiScript_State;
        offset += (s32)s->script_base;
        s->script_cursor = (u8 *)offset;
    }
}

void AiScript_JumpGreater(void)
{
    s32 a = AiScript_ReadByte();
    s32 b = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();
    register s32 *values = gAiScript_aMemory;

    if (values[a] > values[b]) {
        register AiScriptState *s = &gAiScript_State;
        offset += (s32)s->script_base;
        s->script_cursor = (u8 *)offset;
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_AI_SCRIPT_JUMP_EQUAL)
void AiScript_JumpEqual(void)
{
    s32 first = AiScript_ReadByte();
    s32 second = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();

    if (gAiScript_aMemory[first] == gAiScript_aMemory[second]) {
        AiScriptState *state = &gAiScript_State;
        offset += (s32)state->script_base;
        state->script_cursor = (u8 *)offset;
    }
}
#endif

#ifndef VERSION_JAPAN
void AiScript_JumpNotEqual(void)
{
    s32 first = AiScript_ReadByte();
    s32 second = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();
    register s32 *values = gAiScript_aMemory;

    if (values[first] != values[second]) {
        register AiScriptState *state = &gAiScript_State;
        offset += (s32)state->script_base;
        state->script_cursor = (u8 *)offset;
    }
}

void AiScript_JumpBetween(void)
{
    s32 first = AiScript_ReadByte();
    s32 second = AiScript_ReadByte();
    s32 third = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();
    s32 value = gAiScript_aMemory[first];

    if (
        value <= gAiScript_aMemory[second] &&
        gAiScript_aMemory[third] <= value
    ) {
        offset += (s32)gAiScript_State.script_base;
        gAiScript_State.script_cursor = (u8 *)offset;
    }
}

void AiScript_JumpRandom(void)
{
    register s32 *values = gAiScript_aMemory;
    register s32 limit;
    register s32 result;
    s32 index = AiScript_ReadByte();

    limit = values[index];
    result = AiScript_ReadShort();
    if (rand() % AI_SCRIPT_PERCENT_SCALE < limit) {
        result += (s32)gAiScript_State.script_base;
        gAiScript_State.script_cursor = (u8 *)result;
    }
}

void AiScript_Call(void) {
    s32 val = AiScript_ReadShort();

    if (gAiScript_State.return_depth < AI_SCRIPT_RETURN_STACK_COUNT) {
        gAiScript_State.return_stack[gAiScript_State.return_depth] =
            gAiScript_State.script_cursor;
        gAiScript_State.return_depth =
            gAiScript_State.return_depth + 1;
    } else {
        printf(D_800118AC);
        printf(gAiScript_szSourceLineFormat, D_800118CC, 0x17B);
        for (;;)
            ;
    }

    {
        AiScriptState *state = &gAiScript_State;
        val += (s32)state->script_base;
        state->script_cursor = (u8 *)val;
    }
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_AI_SCRIPT_RETURN)
void AiScript_Return(void) {
    u8 count = gAiScript_State.return_depth;
    if (count != 0) {
        count = count - 1;
        gAiScript_State.return_depth = count;
        gAiScript_State.script_cursor = gAiScript_State.return_stack[count];
        return;
    }
    printf(D_800118E4);
    printf(gAiScript_szSourceLineFormat, D_800118CC, 0x193);
    for (;;)
        ;
}
#endif

#ifndef VERSION_JAPAN
void AiScript_SetRandom(void) {
    s32 lo = AiScript_ReadShort();
    s32 hi = AiScript_ReadShort();
    s32 idx = AiScript_ReadByte();

    gAiScript_aMemory[idx] = rand() % (hi - lo + 1) + lo;
}
#endif

#if !defined(VERSION_JAPAN) || defined(VERSION_JAPAN_AI_SCRIPT_SUBTRACT)
void AiScript_Subtract(void){int a=AiScript_ReadByte(),b=AiScript_ReadByte(),c=AiScript_ReadByte();register int*values=gAiScript_aMemory;values[c]=values[a]-values[b];}
#endif
