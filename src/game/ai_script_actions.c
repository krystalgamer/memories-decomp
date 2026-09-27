#include "../types.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#define D_800EAE88_AS_BYTES
#define D_800EAE88_VISIBLE
#include "../unmatched.h"

#ifdef VERSION_EUROPE
/* Two empty European-only script handlers, right before AiScript_PlayFaceUp
   in the image; the European AI script table points at both. */
void func_800732E0(void)
{
}

void func_800732E8(void)
{
}
#endif

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
