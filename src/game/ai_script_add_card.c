#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
extern AiScriptState gAiScript_State;
void AiScript_AddCard(void) {
    s32 *p = gAiScript_aMemory;
    s32 v;
    s32 i;
    v = p[AiScript_ReadByte()];
    for (i = 0; i < AI_SCRIPT_CARD_SET_COUNT; i++) {
        u16 c = gAiScript_State.card_set[i];
        if (c == v) return;
        if (c == AI_SCRIPT_CARD_SET_EMPTY) {
            gAiScript_State.card_set[i] = v;
            return;
        }
    }
}
