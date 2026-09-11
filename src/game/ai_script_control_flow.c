#include "../types.h"
#include "../psyq/rand.h"
#include "../psyq/stdio.h"
#include "ai.h"
#include "ai_script_commands.h"
#include "ai_script_control_flow.h"
#include "ai_script_read_byte.h"
#include "ai_script_read_short.h"
#include "ai_script_source_line_format.h"

s32 Ai_IsCardInSets(s32 mode, s32 index)
{
    if (mode == 1 && Ai_IsCardInSet(index))
        return 1;
    if (mode == 2 && Ai_IsTypeInSet(index))
        return 1;
    return 0;
}

void AiScript_Jump(void)
{
    s32 result = AiScript_ReadShort();
    register AiScriptState *state = &gAiScript_State;

    state->script_cursor =
        (u8 *)(result + (s32)state->script_base);
}

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

extern char D_800118AC[];
extern char D_800118E4[];
extern char D_800118CC[];

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

void AiScript_SetRandom(void) {
    s32 lo = AiScript_ReadShort();
    s32 hi = AiScript_ReadShort();
    s32 idx = AiScript_ReadByte();

    gAiScript_aMemory[idx] = rand() % (hi - lo + 1) + lo;
}

void AiScript_Subtract(void){int a=AiScript_ReadByte(),b=AiScript_ReadByte(),c=AiScript_ReadByte();register int*values=gAiScript_aMemory;values[c]=values[a]-values[b];}
