#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_read_short.h"
extern AiScriptState gAiScript_State;

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
