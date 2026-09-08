#include "../types.h"
#include "ai_constants.h"

extern s32 gAiScript_aMemory[];
extern u8 gAiScript_State[];
extern s16 D_800F5C88[];
extern u8 D_800F5C8B[];

extern s32 Duel_GetBaseCardStat(s32, s32);
extern s32 AiScript_ReadByte(void);
extern s32 Ai_GetHandSize(void);
extern void Ai_CompleteFusion(s32);

void AiScript_EvaluateFusion(void)
{
    u8 *r;
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

    r = (u8 *)gAiScript_State;
    r[AI_SCRIPT_FUSION_COUNT_BYTE_OFFSET] = n;
    r[AI_SCRIPT_FUSION_LIMIT_BYTE_OFFSET] = b;
    *(s16 *)(r + AI_SCRIPT_FUSION_BEST_STAT_BYTE_OFFSET) = 0;
    r[AI_SCRIPT_FUSION_DEPTH_BYTE_OFFSET] = 0;
    r[AI_SCRIPT_FUSION_BEST_DEPTH_BYTE_OFFSET] = 0;
    r[AI_SCRIPT_FUSION_SET_BYTE_OFFSET] = c;

    for (i = 0; i < r[AI_SCRIPT_FUSION_COUNT_BYTE_OFFSET]; i++) {
        r[i + AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 0;
    }

    x = Duel_GetBaseCardStat(a, 0);
    y = Duel_GetBaseCardStat(a, 1);

    if (y < x) {
        D_800F5C88[0] = Duel_GetBaseCardStat(a, 0);
    } else {
        D_800F5C88[0] = Duel_GetBaseCardStat(a, 1);
    }

    Ai_CompleteFusion(a);

    if (D_800F5C8B[0] != 0) {
        gAiScript_aMemory[k] = 0;
    } else {
        gAiScript_aMemory[k] = 1;
    }
}
