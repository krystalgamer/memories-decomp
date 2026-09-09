#include "../types.h"
#include "ai.h"
#include "ai_script_read_short.h"
#include "ai_script_commands.h"

extern AiScriptState gAiScript_State;

extern s32 Ai_IsCardInSet(s32);
extern s32 Ai_IsTypeInSet(s32);
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
