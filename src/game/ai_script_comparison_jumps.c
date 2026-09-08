#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_read_short.h"
extern AiScriptState gAiScript_State;

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
