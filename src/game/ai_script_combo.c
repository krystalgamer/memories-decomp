#define D_8009B1D5_IS_AGGREGATE
#include "../types.h"
#include "duel_side_state.h"
#include "ai.h"
#include "card_constants.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"

void AiScript_LoadDeckSize(void)
{
    s32 k = AiScript_ReadByte();
    s32 count = 0;
    s32 i;

    for (i = AI_SLOT_OWN_HAND_FIRST; i < AI_SLOT_OPPONENT_MONSTER_FIRST; i++) {
        if (gDuel_aActiveCards[i].card_id != 0) {
            count++;
        }
    }
    gAiScript_aMemory[k] = count - HAND_SIZE;
}

void AiScript_LoadSelectionRandom(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;

    values[index] = D_800EAE90;
}

void AiScript_TestPinned(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 index = memory[AiScript_ReadByte()];

    memory[AiScript_ReadByte()] =
        D_800E9FF0[(D_8009B1D5[0] ^ 1) ^ index].
            swords_turns_remaining != 0;
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

void AiScript_LoadBestDifference(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    u32 value;

    index *= sizeof(*values);
    value = gAi_wBestDifference[0];
    *(u32 *)((u8 *)values + index) = value;
}

void AiScript_LoadBestAttacker(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;

    values[index] = gAi_bBestAttacker;
}

void AiScript_LoadBestTarget(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    u32 value;

    index *= sizeof(*values);
    value = gAi_bBestTarget;
    *(u32 *)((u8 *)values + index) = value;
}
