#include "../types.h"
#define AI_GET_HAND_SIZE_RETURNS_S32
#include "../psyq/rand.h"
#include "../psyq/stdio.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#include "duel_get_base_card_stat.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_side_state.h"
#include "ai_script_opcodes.h"
#include "ai_script_read_short.h"
#include "ai_script_source_line_format.h"
#include "../unmatched.h"

void Ai_GetWinningCardRange(s32 kind, s32 *low, s32 *high)
{
    s32 value;

    switch (kind) {
    case 0:
    case 1:
        *low = AI_SLOT_OWN_MONSTER_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OWN_MONSTER_FIRST);
        break;
    case 2:
        *low = AI_SLOT_OWN_HAND_FIRST;
        value = Ai_GetHandSize() + AI_SLOT_OWN_HAND_FIRST - 1;
        break;
    case 3:
    case 4:
    case 5:
        *low = AI_SLOT_OPPONENT_MONSTER_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST);
        break;
    case 6:
        *low = AI_SLOT_OPPONENT_HAND_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_HAND_FIRST);
        break;
    default:
        return;
    }

    *high = value;
}

void Ai_GetCardRange(s32 kind, s32 *low, s32 *high)
{
    s32 value;

    switch (kind) {
    case 0:
    case 1:
        *low = AI_SLOT_OWN_MONSTER_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OWN_MONSTER_FIRST);
        break;
    case 2:
    case 3:
        *low = AI_SLOT_OWN_SPELL_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OWN_SPELL_FIRST);
        break;
    case 4:
        *low = AI_SLOT_OWN_HAND_FIRST;
        value = Ai_GetHandSize() + AI_SLOT_OWN_HAND_FIRST - 1;
        break;
    case 5:
    case 6:
        *low = AI_SLOT_OPPONENT_MONSTER_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_MONSTER_FIRST);
        break;
    case 7:
    case 8:
        *low = AI_SLOT_OPPONENT_SPELL_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_SPELL_FIRST);
        break;
    case 9:
        *low = AI_SLOT_OPPONENT_HAND_FIRST;
        value = AI_SLOT_ROW_LAST(AI_SLOT_OPPONENT_HAND_FIRST);
        break;
    default:
        return;
    }

    *high = value;
}

s32 Ai_IsCardInSet(s32 arg0)
{
    s32 value;
    s32 i;

    value = gDuel_aActiveCards[arg0].card_id;
    for (i = 0; i < AI_SCRIPT_CARD_SET_COUNT; i++) {
        if (value == gAiScript_State.card_set[i])
            return 1;
    }
    return 0;
}

s32 Ai_IsTypeInSet(s32 arg0)
{
    s32 value;
    s32 i;

    value = gDuel_aActiveCards[arg0].card_type;
    i = 0;
    for (; i < AI_SCRIPT_TYPE_SET_COUNT; i++) {
        if (value == gAiScript_State.type_set[i] -
                     AI_SCRIPT_TYPE_SET_ENCODING_BIAS)
            return 1;
    }
    return 0;
}

s32 Ai_IsCardInSets(s32 mode, s32 index)
{
    if (mode == 1 && Ai_IsCardInSet(index))
        return 1;
    if (mode == 2 && Ai_IsTypeInSet(index))
        return 1;
    return 0;
}

void AiScript_Jump(void)
{
    s32 result = AiScript_ReadShort();
    register AiScriptState *state = &gAiScript_State;

    state->script_cursor =
        (u8 *)(result + (s32)state->script_base);
}

/* The AI script VM's control-flow opcodes: the six conditional jumps, which
   read register operands and a script-relative offset and move
   gAiScript_State.script_cursor when their test holds, then the call and
   return pair that push and pop that cursor on the return stack. The final
   random-assignment and subtraction opcodes are data operations over the same
   readers and register file.

   The former support and subtract objects were recorded at gcc_2_8_1_g0 and
   are byte-identical at gcc_2_8_1_g0_split. The nine-function control object
   is byte-identical at gcc_2_8_1_g0_split and gcc_2_8_1_g8_split. */

void AiScript_JumpGreaterEqual(void)
{
    s32 a = AiScript_ReadByte();
    s32 b = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();
    register s32 *values = gAiScript_aMemory;

    if (values[a] >= values[b]) {
        register AiScriptState *s = &gAiScript_State;
        offset += (s32)s->script_base;
        s->script_cursor = (u8 *)offset;
    }
}

void AiScript_JumpGreater(void)
{
    s32 a = AiScript_ReadByte();
    s32 b = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();
    register s32 *values = gAiScript_aMemory;

    if (values[a] > values[b]) {
        register AiScriptState *s = &gAiScript_State;
        offset += (s32)s->script_base;
        s->script_cursor = (u8 *)offset;
    }
}

void AiScript_JumpEqual(void)
{
    s32 first = AiScript_ReadByte();
    s32 second = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();

    if (gAiScript_aMemory[first] == gAiScript_aMemory[second]) {
        AiScriptState *state = &gAiScript_State;
        offset += (s32)state->script_base;
        state->script_cursor = (u8 *)offset;
    }
}

void AiScript_JumpNotEqual(void)
{
    s32 first = AiScript_ReadByte();
    s32 second = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();
    register s32 *values = gAiScript_aMemory;

    if (values[first] != values[second]) {
        register AiScriptState *state = &gAiScript_State;
        offset += (s32)state->script_base;
        state->script_cursor = (u8 *)offset;
    }
}

void AiScript_JumpBetween(void)
{
    s32 first = AiScript_ReadByte();
    s32 second = AiScript_ReadByte();
    s32 third = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();
    s32 value = gAiScript_aMemory[first];

    if (
        value <= gAiScript_aMemory[second] &&
        gAiScript_aMemory[third] <= value
    ) {
        offset += (s32)gAiScript_State.script_base;
        gAiScript_State.script_cursor = (u8 *)offset;
    }
}

void AiScript_JumpRandom(void)
{
    register s32 *values = gAiScript_aMemory;
    register s32 limit;
    register s32 result;
    s32 index = AiScript_ReadByte();

    limit = values[index];
    result = AiScript_ReadShort();
    if (rand() % AI_SCRIPT_PERCENT_SCALE < limit) {
        result += (s32)gAiScript_State.script_base;
        gAiScript_State.script_cursor = (u8 *)result;
    }
}

/* The European build keeps the overflow and underflow traps of
   AiScript_Call and AiScript_Return without their printf calls. */
void AiScript_Call(void) {
    s32 val = AiScript_ReadShort();

    if (gAiScript_State.return_depth < AI_SCRIPT_RETURN_STACK_COUNT) {
        gAiScript_State.return_stack[gAiScript_State.return_depth] =
            gAiScript_State.script_cursor;
        gAiScript_State.return_depth =
            gAiScript_State.return_depth + 1;
    } else {
#ifndef VERSION_EUROPE
        printf(D_800118AC);
        printf(gAiScript_szSourceLineFormat, D_800118CC, 0x17B);
#endif
        for (;;)
            ;
    }

    {
        AiScriptState *state = &gAiScript_State;
        val += (s32)state->script_base;
        state->script_cursor = (u8 *)val;
    }
}

void AiScript_Return(void) {
    u8 count = gAiScript_State.return_depth;
    if (count != 0) {
        count = count - 1;
        gAiScript_State.return_depth = count;
        gAiScript_State.script_cursor = gAiScript_State.return_stack[count];
        return;
    }
#ifndef VERSION_EUROPE
    printf(D_800118E4);
    printf(gAiScript_szSourceLineFormat, D_800118CC, 0x193);
#endif
    for (;;)
        ;
}

void AiScript_SetRandom(void) {
    s32 lo = AiScript_ReadShort();
    s32 hi = AiScript_ReadShort();
    s32 idx = AiScript_ReadByte();

    gAiScript_aMemory[idx] = rand() % (hi - lo + 1) + lo;
}

void AiScript_Subtract(void){int a=AiScript_ReadByte(),b=AiScript_ReadByte(),c=AiScript_ReadByte();register int*values=gAiScript_aMemory;values[c]=values[a]-values[b];}

/* The three AI script opcodes that end an interpreter run. After each
   dispatch AiScript_Run (ai_script_vm.c) compares the handler it just called
   against these and returns 2 for AiScript_PlayFieldCard, 1 for
   AiScript_EndHand and 3 for AiScript_EndField. Only the field play has a
   body: it stamps the script-selected slot, card and category into the
   play-command record before the run ends. */

void AiScript_PlayFieldCard(void)
{
    s32 first = gAiScript_aMemory[AiScript_ReadByte()];
    s32 second = gAiScript_aMemory[AiScript_ReadByte()];
    s32 third = gAiScript_aMemory[AiScript_ReadByte()];
    s32 category = gAiScript_aMemory[AiScript_ReadByte()];

    gAi_Selection.fields.field_09 = (s8)first;
    gAi_Selection.fields.field_0B = (s8)third;

    if (category == 0) {
        if (first >= 6 && second == 1) {
            gAi_Selection.fields.field_0A = (s8)first;
            return;
        }
        D_800EAE92 = 0;
        return;
    }

    if (category < 0xFF) {
        gAi_Selection.fields.field_0A = (s8)category;
    } else {
        gAi_Selection.fields.field_0A = 0x3A;
    }
}

void AiScript_EndHand(void)
{
}

void AiScript_EndField(void)
{
}

void AiScript_CalcCardPower(void)
{
    s32 *mem = gAiScript_aMemory;
    s32 card;
    s32 mode;
    s32 dst;
    s32 power;

    card = mem[AiScript_ReadByte()];
    mode = mem[AiScript_ReadByte()];
    dst = AiScript_ReadByte();

#ifdef VERSION_EUROPE
    /* The European build clears the result before the type test, so a
       monster with an unknown mode stores 0 too. */
    power = 0;
#endif
    if (((gDuel_adwCardStats[card - 1] >> CARD_STAT_TYPE_SHIFT) &
         CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
        switch (mode) {
        case 0:
            power = Duel_GetBaseCardStat(card, 0);
            break;
        case 1:
            power = Duel_GetBaseCardStat(card, 1);
            break;
        case 2:
            if (Duel_GetBaseCardStat(card, 0) > Duel_GetBaseCardStat(card, 1)) {
                power = Duel_GetBaseCardStat(card, 0);
            } else {
                power = Duel_GetBaseCardStat(card, 1);
            }
            break;
        }
    } else {
#ifndef VERSION_EUROPE
        power = 0;
#endif
        if (mode == 0) {
            switch (card) {
            case DUEL_DIRECT_DAMAGE_FIRST_CARD_ID:
                power = DUEL_SPARKS_DAMAGE;
                break;
            case DUEL_DIRECT_DAMAGE_FIRST_CARD_ID + 1:
                power = DUEL_HINOTAMA_DAMAGE;
                break;
            case DUEL_DIRECT_DAMAGE_FIRST_CARD_ID + 2:
                power = DUEL_FINAL_FLAME_DAMAGE;
                break;
            case DUEL_DIRECT_DAMAGE_FIRST_CARD_ID + 3:
                power = DUEL_OOKAZI_DAMAGE;
                break;
            case DUEL_DIRECT_DAMAGE_FIRST_CARD_ID + 4:
                power = DUEL_TREMENDOUS_FIRE_DAMAGE;
                break;
            }
        }
    }

    gAiScript_aMemory[dst] = power;
}

void AiScript_TestHighStat(void)
{
    register s32 *values = gAiScript_aMemory;
    s32 value = values[AiScript_ReadByte()];
    s32 dest = AiScript_ReadByte();
    s32 result;

    if (((gDuel_adwCardStats[value - 1] >> CARD_STAT_TYPE_SHIFT) &
         CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
        s32 attack = Duel_GetBaseCardStat(value, 0);
        s32 defense = Duel_GetBaseCardStat(value, 1);
        result = !(defense < attack);
    } else {
        result = 0;
    }
    gAiScript_aMemory[dest] = result;
}

void AiScript_LoadCardType(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 index = memory[AiScript_ReadByte()] - 1;

    memory[AiScript_ReadByte()] =
        (gDuel_adwCardStats[index] >> CARD_STAT_TYPE_SHIFT) &
        CARD_STAT_TYPE_MASK;
}

void AiScript_LoadCardID(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 index = memory[AiScript_ReadByte()];

    memory[AiScript_ReadByte()] = gDuel_aActiveCards[index].card_id;
}

/* Looks up gDuel_aActiveCards[gAiScript_aMemory[idx1]] and classifies it: 0 if
   field0 is zero, 2 if the card is face-down, else 1. Stores the result at
   gAiScript_aMemory[idx2]. */
void AiScript_SetFaceDown(void) {
    s32 *table = gAiScript_aMemory;
    s32 idx1 = AiScript_ReadByte();
    s32 val1 = table[idx1];
    s32 idx2 = AiScript_ReadByte();
    s32 flagval;

    if (gDuel_aActiveCards[val1].card_id == 0) {
        flagval = 0;
    } else if (gDuel_aActiveCards[val1].flags & DUEL_CARD_FLAG_FACE_DOWN) {
        flagval = 2;
    } else {
        flagval = 1;
    }

    gAiScript_aMemory[idx2] = flagval;
}

void AiScript_LoadLifePoint(void) {
    s32 *p = gAiScript_aMemory;
    s32 i = p[AiScript_ReadByte()];
    p[AiScript_ReadByte()] = gDuel_aSideStates[1 - i].life_points.signed_value;
}
