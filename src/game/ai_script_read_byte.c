#include "../types.h"
#include "ai_script_read_byte.h"
#include "ai.h"

extern AiScriptState gAiScript_State;

s32 AiScript_ReadByte(void)
{
    // Preserve the original stream-base register allocation.
    register AiScriptState *stream = &gAiScript_State;

    return *stream->script_cursor++;
}
