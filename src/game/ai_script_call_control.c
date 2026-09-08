#include "../types.h"
#include "../psyq/rand.h"
#include "../psyq/stdio.h"
#include "ai.h"
#include "ai_script_call_control.h"
#include "ai_script_read_byte.h"
#include "ai_script_read_short.h"

extern AiScriptState gAiScript_State;
extern char D_800118AC[];
extern char D_800118E4[];
extern char D_800118CC[];

char gAiScript_szSourceLineFormat[] = "%s:%d\n";

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
