#include "../types.h"
#include "func_8002CBF4.h"
#include "ai.h"
#include "ai_constants.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
extern void Ai_CompleteFusion(s32);

void AiScript_EvaluateFusion(void)
{
    s32 a;
    s32 b;
    s32 c;
    s32 k;
    s32 x;
    s32 y;
    s32 i;
    s32 n;

    a = gAiScript_aMemory[AiScript_ReadByte()];
    b = gAiScript_aMemory[AiScript_ReadByte()] + 1;
    c = gAiScript_aMemory[AiScript_ReadByte()];
    k = AiScript_ReadByte();
    n = Ai_GetHandSize();

    gAiScript_State.fusion_count = n;
    gAiScript_State.fusion_limit = b;
    gAiScript_State.fusion_best_stat = 0;
    gAiScript_State.fusion_depth = 0;
    gAiScript_State.fusion_best_depth = 0;
    gAiScript_State.fusion_set = c;

    for (i = 0; i < gAiScript_State.fusion_count; i++) {
        gAiScript_State.fusion_used[i] = 0;
    }

    x = Duel_GetBaseCardStat(a, 0);
    y = Duel_GetBaseCardStat(a, 1);

    if (y < x) {
        gAiScript_State.fusion_best_stat = Duel_GetBaseCardStat(a, 0);
    } else {
        gAiScript_State.fusion_best_stat = Duel_GetBaseCardStat(a, 1);
    }

    Ai_CompleteFusion(a);

    if (gAiScript_State.fusion_best_depth != 0) {
        gAiScript_aMemory[k] = 0;
    } else {
        gAiScript_aMemory[k] = 1;
    }
}
