#include "../types.h"
#include "ai.h"
#include "ai_constants.h"
#include "ai_opponent_data.h"
#include "ai_script_commands.h"
#include "ai_script_read_byte.h"
#include "ai_script_read_short.h"
#include "duel_card_layout.h"

/* The AI script VM's state opcodes, eleven former sources in address order:
   the combo-stack pushes, the hand and field no-ops, the card and type set
   edits, the two flag pairs and AiScript_MoveCard, the opponent-data load,
   the register store, copy and add, and the flag load that ends the run.
   Each reads its operands with AiScript_ReadByte or AiScript_ReadShort and
   writes gAiScript_aMemory, gAiScript_State or the duel card state.

   The former sources were recorded at five profiles. Every member compiles
   to an identical object at gcc_2_8_1_g0_split. Bounded below by
   AiScript_FindBestAttack (gcc_2_8_1_g8_split_no_strength_reduce) and above
   by AiScript_Print (gcc_2_8_1_g8). */

extern u8 D_800EAE88[];

void AiScript_PushComboCard(void)
{
    s32 count = 0;
    s32 i;
    u8 *output;

    i = AI_SCRIPT_COMBO_CARD_COUNT - 1;
    output = D_800EAE88 + i;
    do {
        *output = 0;
        i--;
        output--;
    } while (i >= 0);

    for (i = 0; i <= gAiScript_State.fusion_best_depth; i++) {
        s32 value = gAiScript_State.combo_cards[i];

        if (value >= 11) {
            D_800EAE88[count] = value;
            count++;
        }
    }
}

void AiScript_PushComboEmpty(void)
{
    s32 count;
    s32 index;

    for (count = 0, index = 0;
         index <= gAiScript_State.fusion_best_depth; index++) {
        s32 value = gAiScript_State.combo_cards[index];

        if (value >= 11) {
            D_800EAE88[count] = value;
            count++;
        }
    }
}

void AiScript_HandNop(void)
{
}

void AiScript_FieldNop(void)
{
}

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

void AiScript_ClearCards(void)
{
    s32 i = AI_SCRIPT_CARD_SET_COUNT - 1;
    u16 *entries = (u16 *)&gAiScript_State;

    entries += AI_SCRIPT_CARD_SET_HALFWORD_OFFSET;
    do {
        entries[AI_SCRIPT_CARD_SET_COUNT - 1] = AI_SCRIPT_CARD_SET_EMPTY;
        i--;
        entries--;
    } while (i >= 0);
}

void AiScript_AddType(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 index = AiScript_ReadByte();
    s32 type = memory[index] + AI_SCRIPT_TYPE_SET_ENCODING_BIAS;
    s32 i;

    for (i = 0; i < AI_SCRIPT_TYPE_SET_COUNT; i++) {
        u8 *entry = (u8 *)&gAiScript_State + i;

        if (entry[AI_SCRIPT_TYPE_SET_BYTE_OFFSET] == type) {
            break;
        }
        if (entry[AI_SCRIPT_TYPE_SET_BYTE_OFFSET] == AI_SCRIPT_TYPE_SET_EMPTY) {
            entry[AI_SCRIPT_TYPE_SET_BYTE_OFFSET] = type;
            break;
        }
    }
}

void AiScript_ClearTypes(void)
{
    s32 i = AI_SCRIPT_TYPE_SET_COUNT - 1;
    u8 *entries = (u8 *)&gAiScript_State;

    entries += i;
    do {
        entries[AI_SCRIPT_TYPE_SET_BYTE_OFFSET] = AI_SCRIPT_TYPE_SET_EMPTY;
        i--;
        entries--;
    } while (i >= 0);
}

void func_80073448(void)
{
    D_800EAE90 = 1;
}

void func_80073458(void)
{
    D_800EAE90 = 0;
}

void func_80073464(void)
{
    gAiScript_State.field_97 = 1;
}

void func_80073474(void)
{
    gAiScript_State.field_97 = 0;
}

void AiScript_MoveCard(void)
{
    register s32 *values = gAiScript_aMemory;
    s32 index = AiScript_ReadByte();
    s32 value = values[index];

    gDuel_aActiveCards[value].flags |= DUEL_CARD_FLAG_USED_THIS_TURN;
}

void AiScript_LoadOpponentData(void)
{
    s32 index;
    s32 field;
    s32 dst;
    s32 *mem = gAiScript_aMemory;

    index = mem[AiScript_ReadByte()];
    field = mem[AiScript_ReadByte()];
    dst = AiScript_ReadByte();
    if (field == 0) {
        mem[dst] = gDuel_aOpponentData[index].values[1] * 100;
    } else {
        mem[dst] = gDuel_aOpponentData[index].values[field + 1];
    }
}

void AiScript_Store(void)
{
    int value = AiScript_ReadShort();
    int index = AiScript_ReadByte();
    volatile int *values = gAiScript_aMemory;

    values[index] = value;
}

/* Same 4-byte-stride table as idx_table_copy_lowbyte.c. */

/* Copies one table entry to another: two indices read from the stream. */
void AiScript_SetRegister(void) {
    s32 src = AiScript_ReadByte();
    s32 dst = AiScript_ReadByte();
    gAiScript_aMemory[dst] = gAiScript_aMemory[src];
}

void AiScript_Add(void)
{
    int left = AiScript_ReadByte();
    int right = AiScript_ReadByte();
    int output = AiScript_ReadByte();
    int *values = gAiScript_aMemory;

    values[output] = values[left] + values[right];
}

extern u8 D_800EAE8F;
void func_8007368C(void)
{
    D_800EAE8F = *(u8 *)&gAiScript_aMemory[AiScript_ReadByte()];
}
