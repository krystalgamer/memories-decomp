#include "../types.h"

/* Zero-initialized storage this unit owns at 0x800F5B98-0x800F5CBC, defined in
 * address order ahead of the headers that declare it (#2602). */
s32 gAiScript_aMemory[20];

#include "ai.h"

AiScriptState gAiScript_State;

#include "ai_script_commands.h"
#include "../psyq/libetc.h"
#include "../psyq/memory.h"
#include "ai.h"
#include "ai_script_opcodes.h"
#include "ai_script_read_byte.h"
#include "ai_script_read_short.h"
#include "../unmatched.h"
#include "ai_controller_handoff.h"

/* Initialized data this unit owns at 0x800916E0: the opcode dispatch table AiScript_Run
   indexes. Opcode 0 is unused and its slot is null. */
AiScriptHandler gAiScript_apfnCommand[68] = {
    (AiScriptHandler)0,
    AiScript_Jump,
    AiScript_JumpGreaterEqual,
    AiScript_JumpGreater,
    AiScript_JumpEqual,
    AiScript_JumpNotEqual,
    AiScript_JumpBetween,
    AiScript_JumpRandom,
    AiScript_Call,
    AiScript_Return,
    AiScript_SetRandom,
    AiScript_Subtract,
    AiScript_PlayFieldCard,
    AiScript_EndHand,
    AiScript_EndField,
    AiScript_CalcCardPower,
    AiScript_TestDefenseAtLeastAttack,
    AiScript_LoadCardType,
    AiScript_LoadCardID,
    AiScript_LoadCardState,
    AiScript_LoadLifePoint,
    AiScript_LoadOpponentID,
    AiScript_LoadTerrain,
    AiScript_LoadDeckSize,
    AiScript_LoadHandFaceDownRequest,
    AiScript_TestSwordsActive,
    AiScript_LoadFirstFieldComboSlot,
    AiScript_LoadBestDifference,
    AiScript_LoadBestAttacker,
    AiScript_LoadBestTarget,
    AiScript_FindStrongest,
    AiScript_FindWeakest,
    AiScript_FindKiller,
    AiScript_FindDefenseStopper,
    AiScript_CountCards,
    AiScript_FindFirstCard,
    AiScript_FindCard,
    AiScript_MatchType,
    AiScript_FindEquipTarget,
    AiScript_CheckRitual,
    AiScript_FindFirstMonster,
    AiScript_FindFirstType,
    AiScript_FindBestCombo,
    AiScript_EvaluateFusion,
    AiScript_SkipHand,
    AiScript_SkipField,
    AiScript_PlayFaceUp,
    AiScript_SetPosition,
    AiScript_FindBestAttack,
    AiScript_PushComboCard,
    AiScript_PushComboEmpty,
    AiScript_HandNop,
    AiScript_FieldNop,
    AiScript_AddCard,
    AiScript_ClearCards,
    AiScript_AddType,
    AiScript_ClearTypes,
    AiScript_SetHandFaceDownRequest,
    AiScript_ClearHandFaceDownRequest,
    AiScript_SetUnreadFlag,
    AiScript_ClearUnreadFlag,
    AiScript_MoveCard,
    AiScript_LoadOpponentData,
    AiScript_Store,
    AiScript_SetRegister,
    AiScript_Add,
    AiScript_LoadGuardianStarChoice,
    AiScript_Print,
};

s32 AiScript_ReadByte(void)
{
    // Preserve the original stream-base register allocation.
    register AiScriptState *stream = &gAiScript_State;

    return *stream->script_cursor++;
}

/* Reads a little-endian 16-bit value from the stream and advances the
   cursor by 2. */
s32 AiScript_ReadShort(void) {
    u8 *p = gAiScript_State.script_cursor;
    gAiScript_State.script_cursor += 2;
    return p[0] | (p[1] << 8);
}

void AiScript_Init(u8 *script)
{
    u8 *state = (u8 *)&gAiScript_State;

    bzero(state, sizeof(gAiScript_State));
    bzero((u8 *)&gAi_Selection, sizeof(gAi_Selection));
    bzero((u8 *)gAiScript_aMemory, sizeof(gAiScript_aMemory));
    if (script == 0)
        state[0] = 1;
    gAiScript_State.script_base = script;
    gAiScript_State.script_cursor = script;
}

s32 AiScript_Run(void)
{
    for (;;) {
        s32 idx;
        AiScriptHandler handler;

        gAiScript_State.previous_cursor = gAiScript_State.script_cursor;
        idx = AiScript_ReadByte();
        gAiScript_apfnCommand[idx]();
        handler = gAiScript_apfnCommand[idx];

        if (handler == AiScript_EndHand)
            return 1;
        if (handler == AiScript_EndField)
            return 3;
        if (handler == AiScript_PlayFieldCard)
            return 2;
        if (VSync(1) >= 0xF0)
            break;
    }

    return 0;
}

#include "ai.h"
#include "ai_opponent_data.h"

/* Initialized data this unit owns at 0x800917F0: the per-duellist AI parameters, in duellist
   order. */
AiOpponentData gDuel_aOpponentData[AI_OPPONENT_COUNT] = {
    {{5, 20, 10, 1, 1, 0, 0, 25, 50}},
    {{5, 20, 10, 1, 1, 0, 0, 25, 50}},
    {{5, 30, 20, 1, 2, 0, 0, 25, 25}},
    {{5, 10, 5, 2, 1, 0, 0, 25, 75}},
    {{5, 20, 10, 2, 2, 1, 1, 25, 50}},
    {{5, 30, 20, 3, 1, 1, 1, 25, 75}},
    {{5, 10, 5, 1, 3, 1, 1, 25, 25}},
    {{10, 20, 10, 3, 3, 2, 2, 50, 50}},
    {{20, 20, 10, 2, 2, 3, 3, 75, 50}},
    {{8, 10, 5, 3, 2, 2, 2, 25, 75}},
    {{8, 10, 5, 2, 3, 2, 2, 25, 50}},
    {{10, 20, 10, 3, 3, 2, 2, 75, 25}},
    {{12, 20, 10, 2, 2, 2, 2, 50, 50}},
    {{12, 30, 20, 2, 2, 2, 2, 50, 25}},
    {{14, 20, 10, 3, 3, 3, 3, 75, 25}},
    {{16, 20, 10, 3, 3, 3, 3, 75, 0}},
    {{16, 20, 10, 3, 3, 3, 3, 75, 25}},
    {{16, 20, 10, 3, 3, 3, 3, 75, 50}},
    {{12, 10, 5, 1, 1, 1, 1, 25, 75}},
    {{10, 10, 5, 1, 2, 1, 1, 25, 75}},
    {{10, 30, 20, 2, 1, 1, 1, 25, 25}},
    {{14, 10, 5, 2, 2, 2, 2, 50, 50}},
    {{16, 20, 10, 2, 3, 3, 3, 50, 25}},
    {{14, 10, 5, 2, 2, 2, 2, 50, 50}},
    {{16, 20, 10, 3, 3, 3, 3, 50, 25}},
    {{14, 10, 5, 3, 2, 2, 2, 50, 50}},
    {{16, 20, 10, 3, 3, 3, 3, 50, 25}},
    {{14, 10, 5, 2, 2, 2, 2, 50, 50}},
    {{16, 20, 10, 2, 3, 3, 3, 50, 25}},
    {{14, 10, 5, 3, 2, 2, 2, 50, 50}},
    {{16, 20, 10, 3, 3, 3, 3, 50, 25}},
    {{16, 20, 10, 3, 3, 3, 3, 75, 75}},
    {{18, 10, 5, 1, 2, 3, 3, 50, 25}},
    {{20, 10, 5, 2, 2, 3, 3, 50, 50}},
    {{20, 30, 20, 2, 1, 3, 3, 50, 75}},
    {{20, 20, 10, 2, 2, 3, 3, 75, 0}},
    {{20, 20, 10, 2, 2, 4, 4, 75, 0}},
    {{20, 20, 10, 2, 2, 5, 5, 50, 0}},
    {{20, 10, 5, 2, 2, 5, 5, 75, 0}},
    {{15, 20, 10, 3, 3, 5, 5, 50, 25}},
};

s32 Ai_GetHandSearchCount(void)
{
    return gDuel_aOpponentData[gDuel_bOpponentID].values[
        AI_OPPONENT_HAND_SEARCH_COUNT_FIELD];
}

#include "../psyq/rand.h"
#include "../psyq/stdio.h"
#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#include "duel_card_stat_rules.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_side_state.h"
#include "ai_controller_handoff.h"
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
        value = Ai_GetHandSearchCount() + AI_SLOT_OWN_HAND_FIRST - 1;
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
        value = Ai_GetHandSearchCount() + AI_SLOT_OWN_HAND_FIRST - 1;
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

s32 Ai_IsCardInSet(s32 index)
{
    s32 value;
    s32 i;

    value = gDuel_aActiveCards[index].card_id;
    for (i = 0; i < AI_SCRIPT_CARD_SET_COUNT; i++) {
        if (value == gAiScript_State.card_set[i])
            return 1;
    }
    return 0;
}

s32 Ai_IsTypeInSet(s32 index)
{
    s32 value;
    s32 i;

    value = gDuel_aActiveCards[index].card_type;
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
        printf(gAiScript_szNestOverflowError);
        printf(gAiScript_szSourceLineFormat, gAiScript_szSourceFile, 0x17B);
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
    printf(gAiScript_szReturnUnderflowError);
    printf(gAiScript_szSourceLineFormat, gAiScript_szSourceFile, 0x193);
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
        gAi_Selection.fields.field_0A = 0;
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

void AiScript_TestDefenseAtLeastAttack(void)
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

/* Reads snapshot state without changing the card. An empty card_id takes
   precedence over stale face-down flags; the three result codes are also
   the state-operand domain of AiScript_FindFirstCard. */
void AiScript_LoadCardState(void) {
    s32 *table = gAiScript_aMemory;
    s32 idx1 = AiScript_ReadByte();
    s32 val1 = table[idx1];
    s32 idx2 = AiScript_ReadByte();
    s32 flagval;

    if (gDuel_aActiveCards[val1].card_id == 0) {
        flagval = AI_CARD_STATE_EMPTY;
    } else if (gDuel_aActiveCards[val1].flags & DUEL_CARD_FLAG_FACE_DOWN) {
        flagval = AI_CARD_STATE_FACE_DOWN;
    } else {
        flagval = AI_CARD_STATE_FACE_UP;
    }

    gAiScript_aMemory[idx2] = flagval;
}

void AiScript_LoadLifePoint(void) {
    s32 *p = gAiScript_aMemory;
    s32 i = p[AiScript_ReadByte()];
    p[AiScript_ReadByte()] = gDuel_aSideStates[1 - i].life_points.signed_value;
}

#include "ai.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#include "duel_terrain_boost.h"
#include "ai_opponent_data.h"

void AiScript_LoadOpponentID(void)
{
    s32 index;
    volatile s32 *values;
    s32 value;

    index = AiScript_ReadByte();
    values = gAiScript_aMemory;
    value = gDuel_bOpponentID;
    values[index] = value;
}

void AiScript_LoadTerrain(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    u32 value = gDuel_bTerrain;

    values[index] = value;
}

#include "duel_side_state.h"
#include "ai.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "ai_script_read_byte.h"
#include "ai_script_commands.h"
#include "ai_controller_handoff.h"

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

void AiScript_LoadHandFaceDownRequest(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;

    values[index] = gAi_Selection.fields.face_down;
}

void AiScript_TestSwordsActive(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 index = memory[AiScript_ReadByte()];

    memory[AiScript_ReadByte()] =
        gDuel_aSideStates[(gDuel_bActiveSide ^ 1) ^ index].
            swords_turns_remaining != 0;
}

void AiScript_LoadFirstFieldComboSlot(void)
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
    value = gAiScript_State.attack_best_stat;
    *(u32 *)((u8 *)values + index) = value;
}

void AiScript_LoadBestAttacker(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;

    values[index] = gAiScript_State.attack_best_slot;
}

void AiScript_LoadBestTarget(void)
{
    s32 index = AiScript_ReadByte();
    s32 *values = gAiScript_aMemory;
    u32 value;

    index *= sizeof(*values);
    value = gAiScript_State.attack_best_target;
    *(u32 *)((u8 *)values + index) = value;
}

/* AI script opcode taking five operand bytes: a register selecting which
 * statistic to rank by (0 attack, 1 defence, anything else the better of the
 * two), a register holding the zone type, a register that when 1 hides
 * face-down cards in types 3..5, a register naming a set of slots to exclude,
 * and the register to write. It scans the zone's winning-card range and writes
 * the slot index of the strongest card, or 0 when nothing qualifies. Types 1,
 * 4 and 5 pass over a card already used this turn; type 4 wants attack
 * position only and type 5 defence position only. */
void AiScript_FindStrongest(void)
{
    s32 mode;
    s32 type;
    s32 hide_face_down;
    s32 sets;
    s32 result;
    s32 best_slot;
    s32 best_power;
    s32 *table = gAiScript_aMemory;
    s32 start;
    s32 end;
    s32 i;
    s32 v;

    mode = table[AiScript_ReadByte()];
    best_slot = 0;
    type = table[AiScript_ReadByte()];
    best_power = -1;
    hide_face_down = table[AiScript_ReadByte()];
    sets = table[AiScript_ReadByte()];
    result = AiScript_ReadByte();

    Ai_GetWinningCardRange(type, &start, &end);

    for (i = start; i <= end; i++) {
        if (gDuel_aActiveCards[i].card_id == 0) {
            continue;
        }
        if (type == 1 || type == 4 || type == 5) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
                continue;
            }
        }
        if (type == 4) {
            if (gDuel_aActiveCards[i].flags &
                DUEL_CARD_FLAG_DEFENSE_POSITION) {
                continue;
            }
        }
        if (type == 5) {
            if (!(gDuel_aActiveCards[i].flags &
                  DUEL_CARD_FLAG_DEFENSE_POSITION)) {
                continue;
            }
        }
        if (type >= 3 && type <= 5) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_FACE_DOWN) {
                if (hide_face_down == 1) {
                    continue;
                }
            }
        }
        if (Ai_IsCardInSets(sets, i) != 0) {
            continue;
        }
        if (mode != 1) {
            v = gDuel_aActiveCards[i].attack;
            if (best_power < v) {
                best_power = v;
                best_slot = i;
            }
        }
        if (mode == 0) {
            continue;
        }
        v = gDuel_aActiveCards[i].defense;
        if (best_power < v) {
            best_power = v;
            best_slot = i;
        }
    }

    gAiScript_aMemory[result] = best_slot;
}

/* AI script opcode taking five operand bytes: a register selecting which
 * statistic to rank by (0 attack, 1 defence, anything else the better of the
 * two), a register holding the zone type, a register that when 1 hides
 * face-down cards in types 3..5, a register naming a set of slots to exclude,
 * and the register to write. It scans the zone's winning-card range and writes
 * the slot index of the weakest card, or 0 when nothing qualifies;
 * CARD_STAT_MAX is the starting bar, so only a lower selected stat qualifies.
 * A card exactly at the cap ties and is not selected. Types 1, 4 and 5 pass
 * over a card already used this turn; type 4 wants attack position only and
 * type 5 defence position only. */
void AiScript_FindWeakest(void)
{
    s32 mode;
    s32 type;
    s32 hide_face_down;
    s32 sets;
    s32 result;
    s32 best_slot;
    s32 best_power;
    s32 *table = gAiScript_aMemory;
    s32 start;
    s32 end;
    s32 i;
    s32 v;
    s32 a;
    s32 d;

    mode = table[AiScript_ReadByte()];
    best_slot = 0;
    type = table[AiScript_ReadByte()];
    best_power = CARD_STAT_MAX;
    hide_face_down = table[AiScript_ReadByte()];
    sets = table[AiScript_ReadByte()];
    result = AiScript_ReadByte();

    Ai_GetWinningCardRange(type, &start, &end);

    for (i = start; i <= end; i++) {
        if (gDuel_aActiveCards[i].card_id == 0) {
            continue;
        }
        if (type == 1 || type == 4 || type == 5) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
                continue;
            }
        }
        if (type == 4) {
            if (gDuel_aActiveCards[i].flags &
                DUEL_CARD_FLAG_DEFENSE_POSITION) {
                continue;
            }
        }
        if (type == 5) {
            if (!(gDuel_aActiveCards[i].flags &
                  DUEL_CARD_FLAG_DEFENSE_POSITION)) {
                continue;
            }
        }
        if (type >= 3 && type <= 5) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_FACE_DOWN) {
                if (hide_face_down == 1) {
                    continue;
                }
            }
        }
        if (Ai_IsCardInSets(sets, i) != 0) {
            continue;
        }
        if (mode == 0) {
            v = gDuel_aActiveCards[i].attack;
        } else if (mode == 1) {
            v = gDuel_aActiveCards[i].defense;
        } else {
            a = gDuel_aActiveCards[i].attack;
            d = gDuel_aActiveCards[i].defense;
            if (d < a) {
                if (a < best_power) {
                    best_power = a;
                    best_slot = i;
                }
            } else {
                if (d < best_power) {
                    best_power = d;
                    best_slot = i;
                }
            }
            continue;
        }
        if (v < best_power) {
            best_power = v;
            best_slot = i;
        }
    }

    gAiScript_aMemory[result] = best_slot;
}
