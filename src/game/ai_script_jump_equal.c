#include "../types.h"
#include "ai_script_read_byte.h"
#include "ai_script_read_short.h"

typedef struct {
    s32 first;
    s32 jump_base;
    s32 cursor;
} AiScriptStatePrefix;

extern s32 gAiScript_aMemory[];
extern AiScriptStatePrefix gAiScript_State;
void AiScript_JumpEqual(void)
{
    s32 first = AiScript_ReadByte();
    s32 second = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();

    if (gAiScript_aMemory[first] == gAiScript_aMemory[second]) {
        AiScriptStatePrefix *state = &gAiScript_State;
        offset += state->jump_base;
        state->cursor = offset;
    }
}
