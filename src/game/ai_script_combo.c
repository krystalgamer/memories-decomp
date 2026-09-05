#include "../types.h"
#include "ai.h"

extern u8 D_8009B1D5[];
extern u8 gAiScript_State[];

extern s32 AiScript_ReadByte(void);

void AiScript_TestPinned(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 index = memory[AiScript_ReadByte()];

    memory[AiScript_ReadByte()] =
        D_800E9FF0[(D_8009B1D5[0] ^ 1) ^ index].pinned != 0;
}

void AiScript_StartCombo(void)
{
    s32 dest = AiScript_ReadByte();
    s32 i;

    for (i = 0; i < AI_SCRIPT_COMBO_CARD_COUNT - 1; i++) {
        s32 value = gAiScript_State[0x38 + i];

        if ((u32)(value - 1) < 10) {
            gAiScript_aMemory[dest] = value;
            return;
        }
    }
    gAiScript_aMemory[dest] = 0;
}
