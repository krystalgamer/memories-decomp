#include "../types.h"
#include "ai.h"

extern s32 gAiScript_aMemory[];
extern u8 D_800EAE88[];
extern u8 D_800EAE8E[];

extern s32 AiScript_ReadByte(void);

void AiScript_PlayFaceUp(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 a = memory[AiScript_ReadByte()];
    s32 b = memory[AiScript_ReadByte()];
    s32 c = memory[AiScript_ReadByte()];
    s32 d = memory[AiScript_ReadByte()];
    s32 e = memory[AiScript_ReadByte()];

    D_800EAE88[AI_SCRIPT_COMBO_CARD_COUNT - 1] = 0;
    D_800EAE88[0] = a;
    D_800EAE88[1] = b;
    D_800EAE88[2] = c;
    D_800EAE88[3] = d;
    D_800EAE88[4] = e;
}

void AiScript_SetPosition(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 value;

    value = memory[AiScript_ReadByte()];
    D_800EAE8E[0] = value;
}
