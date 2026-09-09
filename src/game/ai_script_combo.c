#define D_8009B1D5_IS_AGGREGATE
#include "../types.h"
#include "duel_side_state.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

void AiScript_TestPinned(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 index = memory[AiScript_ReadByte()];

    memory[AiScript_ReadByte()] =
        D_800E9FF0[(D_8009B1D5[0] ^ 1) ^ index].field_19 != 0;
}

void AiScript_StartCombo(void)
{
    s32 dest = AiScript_ReadByte();
    s32 i;

    for (i = 0; i < AI_SCRIPT_COMBO_CARD_COUNT - 1; i++) {
        s32 value = gAiScript_State.combo_cards[i];

        if ((u32)(value - 1) < 10) {
            gAiScript_aMemory[dest] = value;
            return;
        }
    }
    gAiScript_aMemory[dest] = 0;
}
