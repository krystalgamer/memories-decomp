#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"

s32 AiScript_ReadByte(void)
{
    // Preserve the original stream-base register allocation.
    register AiScriptState *stream = &gAiScript_State;

    return *stream->script_cursor++;
}
