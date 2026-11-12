#include "../types.h"

/* Zero-initialized storage this unit owns at 0x800F5B98-0x800F5CBC, defined in
 * address order ahead of the headers that declare it (#2602). */
s32 gAiScript_aMemory[20];

#include "ai.h"

AiScriptState gAiScript_State;

#include "ai_script_commands.h"
#include "../psyq/libetc.h"
#include "../psyq/memory.h"
#include "ai_script.h"

/* Read-only data this unit owns ahead of its jump tables, in the order the
 * nest/return traps and AiScript_Print first print them. The European build
 * makes none of these printf calls. They stay definitions rather than
 * literals: GCC would emit each literal with its first user, after the jump
 * tables of Ai_GetWinningCardRange, Ai_GetCardRange and the functions between
 * AiScript_Return and AiScript_Print, but retail places all of them first. */
#ifndef VERSION_EUROPE
const char gAiScript_szNestOverflowError[] = "ERROR:Sub Routine Nest Over !!\n";
const char gAiScript_szSourceFile[] = "src/hirata/H_mctrl1.c";
const char gAiScript_szReturnUnderflowError[] = "ERROR:Can't Return From Routine !!\n";
const char gAiScript_szCheckpoint[] = "check_point\n";
const char gAiScript_szCheckpointFormat[] = "---- %d ------------------------------------\n";
#endif

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
#include "duel_card_stat_rules.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_card_layout.h"
#include "duel_scene.h"

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
   the nine-function control object at gcc_2_8_1_g0_split; all are
   byte-identical at this unit's gcc_2_8_1_g8_split. */

void AiScript_JumpGreaterEqual(void)
{
    s32 first = AiScript_ReadByte();
    s32 second = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();
    register s32 *values = gAiScript_aMemory;

    if (values[first] >= values[second]) {
        register AiScriptState *state = &gAiScript_State;
        offset += (s32)state->script_base;
        state->script_cursor = (u8 *)offset;
    }
}

void AiScript_JumpGreater(void)
{
    s32 first = AiScript_ReadByte();
    s32 second = AiScript_ReadByte();
    s32 offset = AiScript_ReadShort();
    register s32 *values = gAiScript_aMemory;

    if (values[first] > values[second]) {
        register AiScriptState *state = &gAiScript_State;
        offset += (s32)state->script_base;
        state->script_cursor = (u8 *)offset;
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
        printf("%s:%d\n", gAiScript_szSourceFile, 0x17B);
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
    printf("%s:%d\n", gAiScript_szSourceFile, 0x193);
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

    gAi_Selection.fields.source_code = (s8)first;
    gAi_Selection.fields.field_0B = (s8)third;

    if (category == 0) {
        if (first >= 6 && second == 1) {
            gAi_Selection.fields.target_code = (s8)first;
            return;
        }
        gAi_Selection.fields.target_code = 0;
        return;
    }

    if (category < 0xFF) {
        gAi_Selection.fields.target_code = (s8)category;
    } else {
        gAi_Selection.fields.target_code = 0x3A;
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
    s32 power;
    s32 attack;
    s32 defense;

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
            power = gDuel_aActiveCards[i].attack;
        } else if (mode == 1) {
            power = gDuel_aActiveCards[i].defense;
        } else {
            attack = gDuel_aActiveCards[i].attack;
            defense = gDuel_aActiveCards[i].defense;
            if (defense < attack) {
                if (attack < best_power) {
                    best_power = attack;
                    best_slot = i;
                }
            } else {
                if (defense < best_power) {
                    best_power = defense;
                    best_slot = i;
                }
            }
            continue;
        }
        if (power < best_power) {
            best_power = power;
            best_slot = i;
        }
    }

    gAiScript_aMemory[result] = best_slot;
}

#include "ai_constants.h"
#include "duel_grid.h"

/* Each slot is addressed as &gDuel_aActiveCards[i] from the loop index, so the
 * strength reducer folds the card_id, attack, flags and guardian_star reads
 * (+0, +2, +6, +9) into one cursor with plain displacements, as in retail. */

void AiScript_FindKiller(void) {
    AiActiveCard *card;
    AiActiveCard *ref;
    s32 target_slot;
    s32 mode;
    s32 result;
    s32 i;
    s32 best_slot;
    s32 best;
    s32 margin;
    s32 attack;

    target_slot = gAiScript_aMemory[AiScript_ReadByte()];
    best = CARD_STAT_MAX;
    mode = gAiScript_aMemory[AiScript_ReadByte()];
    best_slot = 0;
    result = AiScript_ReadByte();
    i = 1;
    ref = &gDuel_aActiveCards[target_slot];

    for (; i < DUEL_FIELD_ROW_SIZE + 1; i++) {
        card = &gDuel_aActiveCards[i];
        if (card->card_id == 0) {
            continue;
        }
        if ((card->flags & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
            continue;
        }
        if (mode == 0) {
            margin = card->attack - ref->attack;
        } else {
            margin = card->attack - ref->defense;
        }
        margin = margin + Duel_CalcGuardianStarMatchup(card->guardian_star, ref->guardian_star);
        if (margin > 0) {
            attack = card->attack;
            if (attack < best) {
                best = attack;
                best_slot = i;
            }
        }
    }

    gAiScript_aMemory[result] = best_slot;
}

/* AI script opcode taking two operand bytes: a register that when non-zero
 * makes a face-down opponent card invisible to the scan, and the register to
 * write. It pairs the two fields off strongest against strongest - each round
 * takes the strongest card not yet taken from slots 1..5 and the strongest not
 * yet taken from slots 56..60, marks both taken, and stops as soon as the
 * opponent's pick is not beaten. It answers 1 only if at least one round ran
 * and every round was won, so a losing or empty field answers 0. Slot 0 of
 * gDuel_aActiveCards is the zero entry both searches start from, which is what
 * makes index 0 mean "nothing found" while still comparing cleanly. */
void AiScript_FindDefenseStopper(void)
{
    s32 hide_face_down;
    s32 result;
    s32 answer;
    s32 taken[DUEL_SIDE_COUNT][DUEL_FIELD_ROW_SIZE];
    s32 best;
    s32 other;
    s32 i;
    s32 j;
    AiActiveCard *cards;
    AiActiveCard *others;

    hide_face_down = gAiScript_aMemory[AiScript_ReadByte()];
    result = AiScript_ReadByte();
    answer = 1;

    for (i = 0; i < DUEL_SIDE_COUNT; i++) {
        for (j = 0; j < DUEL_FIELD_ROW_SIZE; j++) {
            taken[i][j] = 0;
        }
    }

    i = 0;
    do {
        best = AI_SLOT_NONE;
        cards = &gDuel_aActiveCards[AI_SLOT_OWN_MONSTER_FIRST];
        for (j = 0; j < DUEL_FIELD_ROW_SIZE; j++) {
            if (taken[0][j] == 0) {
                if (cards[j].attack > gDuel_aActiveCards[best].attack) {
                    best = j + AI_SLOT_OWN_MONSTER_FIRST;
                }
            }
        }
        if (best != AI_SLOT_NONE) {
            taken[0][best - AI_SLOT_OWN_MONSTER_FIRST] = 1;
        }

        other = AI_SLOT_NONE;
        others = &gDuel_aActiveCards[AI_SLOT_OPPONENT_MONSTER_FIRST];
        for (j = 0; j < DUEL_FIELD_ROW_SIZE; j++) {
            if (taken[1][j] == 0) {
                if (hide_face_down == 0 ||
                    !(others[j].flags & DUEL_CARD_FLAG_FACE_DOWN)) {
                    if (others[j].attack > gDuel_aActiveCards[other].attack) {
                        other = j + AI_SLOT_OPPONENT_MONSTER_FIRST;
                    }
                }
            }
        }
        if (other == AI_SLOT_NONE) {
            break;
        }
        taken[1][other - AI_SLOT_OPPONENT_MONSTER_FIRST] = 1;
        if (gDuel_aActiveCards[best].attack <= gDuel_aActiveCards[other].attack) {
            answer = 0;
            break;
        }
        i++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    if (i == 0) {
        answer = 0;
    }
    gAiScript_aMemory[result] = answer;
}

void AiScript_CountCards(void)
{
    s32 count;
    s32 *table = gAiScript_aMemory;
    s32 type;
    s32 output;
    s32 start;
    s32 end;
    s32 i;

    type = AiScript_ReadByte();
    type = table[type];
    output = AiScript_ReadByte();
    count = 0;

    Ai_GetCardRange(type, &start, &end);

    for (i = start; i <= end; i++) {
        AiActiveCard *entry = &gDuel_aActiveCards[i];
        if (entry->card_id != 0) {
            if (type == 1 || type == 3 || type == 6 || type == 8) {
                if (!(entry->flags & DUEL_CARD_FLAG_USED_THIS_TURN)) {
                    count++;
                }
            } else {
                count++;
            }
        }
    }

    gAiScript_aMemory[output] = count;
}

/* AI script opcode taking three operand bytes: a register holding the wanted
 * slot state, a register holding the zone type, and the register to write.
 * It walks the zone's slot range in order and stops at the first slot whose
 * state matches - 0 for an empty slot, 1 for a face-up card, 2 for a face-down
 * card - writing that slot index, or 0 when the range runs out. In the four
 * zone types that can act, a card already used this turn is passed over.
 * Slot 0 doubles as the not-found answer, which is why the caller's range
 * never starts there. */
void AiScript_FindFirstCard(void)
{
    s32 wanted;
    s32 type;
    s32 result;
    s32 *table = gAiScript_aMemory;
    s32 wanted_idx;
    s32 type_idx;
    s32 start;
    s32 end;
    s32 i;

    wanted_idx = AiScript_ReadByte();
    wanted = table[wanted_idx];
    type_idx = AiScript_ReadByte();
    type = table[type_idx];
    result = AiScript_ReadByte();

    Ai_GetCardRange(type, &start, &end);

    for (i = start; i <= end; i++) {
        if (type == 1 || type == 3 || type == 6 || type == 8) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
                continue;
            }
        }
        if (gDuel_aActiveCards[i].card_id != 0) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_FACE_DOWN) {
                if (wanted == AI_CARD_STATE_FACE_DOWN) {
                    break;
                }
            } else {
                if (wanted == AI_CARD_STATE_FACE_UP) {
                    break;
                }
            }
        } else {
            if (wanted == AI_CARD_STATE_EMPTY) {
                break;
            }
        }
    }

    if (end < i) {
        gAiScript_aMemory[result] = 0;
    } else {
        gAiScript_aMemory[result] = i;
    }
}

/* AI script opcode taking four operand bytes: a register holding the card id
 * to look for, a register holding the zone type, a register that when 1 makes
 * a face-down card in a hidden zone (type 5 and up) not count, and the
 * register to write. It walks the zone's slot range in order and stops at the
 * first slot holding that card id, writing the slot index, or 0 when the range
 * runs out. In the four zone types that can act, a card already used this turn
 * is passed over. */
void AiScript_FindCard(void)
{
    s32 wanted;
    s32 type;
    s32 visible_only;
    s32 result;
    s32 *table = gAiScript_aMemory;
    s32 wanted_idx;
    s32 type_idx;
    s32 visible_idx;
    s32 start;
    s32 end;
    s32 i;

    wanted_idx = AiScript_ReadByte();
    wanted = table[wanted_idx];
    type_idx = AiScript_ReadByte();
    type = table[type_idx];
    visible_idx = AiScript_ReadByte();
    visible_only = table[visible_idx];
    result = AiScript_ReadByte();

    Ai_GetCardRange(type, &start, &end);

    for (i = start; i <= end; i++) {
        if (gDuel_aActiveCards[i].card_id != wanted) {
            continue;
        }
        if (type == 1 || type == 3 || type == 6 || type == 8) {
            if (gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_USED_THIS_TURN) {
                continue;
            }
        }
        if (visible_only != 1) {
            break;
        }
        if (type < 5) {
            break;
        }
        if (!(gDuel_aActiveCards[i].flags & DUEL_CARD_FLAG_FACE_DOWN)) {
            break;
        }
    }

    if (end < i) {
        gAiScript_aMemory[result] = 0;
    } else {
        gAiScript_aMemory[result] = i;
    }
}

/* Each slot is indexed from gDuel_aActiveCards; gcc's strength reduction
 * turns the index into retail's single cursor, so it needs no
 * -fno-strength-reduce. A walking AiActiveCard cursor instead makes it build
 * a second induction variable for the repeated flag reads. */
void AiScript_MatchType(void)
{
    s32 start;
    s32 end;
    s32 type;
    s32 range;
    s32 set;
    s32 sets;
    s32 output;
    s32 index;
    s32 one;

    type = gAiScript_aMemory[AiScript_ReadByte()];
    range = gAiScript_aMemory[AiScript_ReadByte()];
    set = gAiScript_aMemory[AiScript_ReadByte()];
    sets = gAiScript_aMemory[AiScript_ReadByte()];
    output = AiScript_ReadByte();

    Ai_GetCardRange(range, &start, &end);

    index = start;
    if (end >= index) {
        one = 1;
        do {
            if (gDuel_aActiveCards[index].card_id == 0) {
                continue;
            }
            if (gDuel_aActiveCards[index].card_type != type) {
                continue;
            }
            if (range == one || range == 3 || range == 6 || range == 8) {
                if ((gDuel_aActiveCards[index].flags & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
                    continue;
                }
            }
            if (set == one && range >= 5) {
                if ((gDuel_aActiveCards[index].flags & DUEL_CARD_FLAG_FACE_DOWN) != 0) {
                    continue;
                }
            }
            if (Ai_IsCardInSets(sets, index) == 0) {
                break;
            }
        } while (++index <= end);
    }

    if (end < index) {
        gAiScript_aMemory[output] = 0;
    } else {
        gAiScript_aMemory[output] = index;
    }
}

#include "duel_effect_object_pool.h"

void AiScript_FindEquipTarget(void)
{
    s32 owner = gAiScript_aMemory[AiScript_ReadByte()];
    s32 mode = gAiScript_aMemory[AiScript_ReadByte()];
    s32 type = gAiScript_aMemory[AiScript_ReadByte()];
    s32 dest = AiScript_ReadByte();
    s32 i;

    for (i = 1; i < DUEL_FIELD_ROW_SIZE + 1; i++) {
        AiActiveCard *card = &gDuel_aActiveCards[i];

        if (!card->card_id)
            continue;
        if (mode == 1 && (card->flags & DUEL_CARD_FLAG_USED_THIS_TURN))
            continue;
        if (Ai_IsCardInSets(type, i))
            continue;
        if (Duel_CheckEquip(owner, card->card_id)) {
            gAiScript_aMemory[dest] = i;
            return;
        }
    }
    gAiScript_aMemory[dest] = 0;
}

void AiScript_CheckRitual(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 value = memory[AiScript_ReadByte()];
    s32 dest = AiScript_ReadByte();

    if (Duel_CheckRitual(0, value) != 0)
        memory[dest] = 1;
    else
        memory[dest] = 0;
}

void AiScript_FindFirstMonster(void)
{
    s32 dest = AiScript_ReadByte();
    s32 i;

    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        s32 index = gAi_Selection.bytes[i];
        AiActiveCard *card = &gDuel_aActiveCards[index];

        if (card->card_id && card->card_type < CARD_TYPE_MAGIC) {
            gAiScript_aMemory[dest] = index;
            return;
        }
    }
    gAiScript_aMemory[dest] = 0;
}

void AiScript_FindFirstType(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 type = memory[AiScript_ReadByte()];
    s32 dest = AiScript_ReadByte();
    s32 i = 0;
    u8 *indices = gAi_Selection.bytes;
    AiActiveCard *cards = gDuel_aActiveCards;

    do {
        s32 index = *(u8 *)(i + (s32)indices);
        AiActiveCard *card =
            (AiActiveCard *)((u32)&((AiActiveCard *)0)[index] + (s32)cards);

        if (card->card_id != 0 && card->card_type == type) {
            memory[dest] = index;
            return;
        }
        i++;
    } while (i < DUEL_FIELD_ROW_SIZE);

    gAiScript_aMemory[dest] = 0;
}

void Ai_CompleteFusion(s32 candidate_card_id)
{
    s32 hand_index;
    s32 hand_slot_index;
    s32 hand_card_id;
    s32 result_card_id;
    s32 path_index;

    hand_index = 0;
    if (gAiScript_State.fusion_count == 0)
        return;
    do {
        if (gAiScript_State.fusion_used[hand_index] == 0) {
            hand_card_id = gDuel_aActiveCards[hand_index + AI_SLOT_OWN_HAND_FIRST].card_id;
            hand_slot_index = hand_index + AI_SLOT_OWN_HAND_FIRST;
            if (hand_card_id != 0) {
                if (Ai_IsCardInSets(gAiScript_State.fusion_set, hand_slot_index) == 0) {
                    result_card_id = Duel_CheckFusion(candidate_card_id, hand_card_id);
                    if (result_card_id == 0)
                        result_card_id = Duel_CheckEquip(candidate_card_id, hand_card_id);
                    if (result_card_id != 0) {
                        gAiScript_State.fusion_path[gAiScript_State.fusion_depth] = hand_slot_index;
                        if (
                            Duel_GetBaseCardStat(result_card_id, 0) >
                                gAiScript_State.fusion_best_stat ||
                            (
                                Duel_GetBaseCardStat(result_card_id, 0) ==
                                    gAiScript_State.fusion_best_stat &&
                                gAiScript_State.fusion_depth <
                                    gAiScript_State.fusion_best_depth
                            )
                        ) {
                            gAiScript_State.fusion_best_stat =
                                Duel_GetBaseCardStat(result_card_id, 0);
                            gAiScript_State.fusion_best_depth =
                                gAiScript_State.fusion_depth;
                            for (path_index = 0;
                                 path_index <= (s32)gAiScript_State.fusion_best_depth;
                                 path_index++)
                                gAiScript_State.combo_cards[path_index] =
                                    gAiScript_State.fusion_path[path_index];
                            gAiScript_State.combo_cards[path_index] = 0;
                        }
                        if (
                            Duel_GetBaseCardStat(result_card_id, 1) >
                                gAiScript_State.fusion_best_stat ||
                            (
                                Duel_GetBaseCardStat(result_card_id, 1) ==
                                    gAiScript_State.fusion_best_stat &&
                                gAiScript_State.fusion_depth <
                                    gAiScript_State.fusion_best_depth
                            )
                        ) {
                            gAiScript_State.fusion_best_stat =
                                Duel_GetBaseCardStat(result_card_id, 1);
                            gAiScript_State.fusion_best_depth =
                                gAiScript_State.fusion_depth;
                            for (path_index = 0;
                                 path_index <= (s32)gAiScript_State.fusion_best_depth;
                                 path_index++)
                                gAiScript_State.combo_cards[path_index] =
                                    gAiScript_State.fusion_path[path_index];
                            gAiScript_State.combo_cards[path_index] = 0;
                        }
                        if (gAiScript_State.fusion_depth <
                            gAiScript_State.fusion_limit - 1) {
                            gAiScript_State.fusion_used[hand_index] = 1;
                            gAiScript_State.fusion_depth++;
                            Ai_CompleteFusion(result_card_id);
                            gAiScript_State.fusion_used[hand_index] = 0;
                            gAiScript_State.fusion_depth--;
                        }
                    }
                }
            }
        }
        hand_index++;
    } while (hand_index < gAiScript_State.fusion_count);
}

/* The two fusion-search opcodes below seed the fusion scratch in
   gAiScript_State and recurse through Ai_CompleteFusion above. They were
   recorded at gcc_2_8_1_cc_g0_as_g8_split and gcc_2_8_1_g8_split, and each
   compiles to identical code at this unit's gcc_2_8_1_g8_split. */

/* AI script opcode taking three operand bytes: a register holding the search
 * depth minus one, a register naming a set of slots to exclude, and the
 * register to write. It seeds the fusion scratch in gAiScript_State - hand size
 * at 0x9C, depth limit at 0x9D, excluded sets at 0x9E, best power at 0xA0,
 * current depth at 0xA2, the current path from 0xA4 and per-slot in-use flags
 * from 0xAA - then walks the five field slots and then the hand, recording the
 * strongest card by either statistic and recursing through Ai_CompleteFusion
 * once the depth allows it. The selected combo is stored separately at 0x38,
 * with its inclusive best-depth bound at 0xA3.
 * The answer classifies where the best combo starts:
 * 3 when nothing was found, 0 when it starts past the field, otherwise 1 or 2
 * on the second combo byte. */
void AiScript_FindBestCombo(void)
{
    s32 depth_limit_work;
    s32 excluded_set;
    s32 result_register;
    s32 hand_search_count;
    s32 search_index;
    s32 candidate_slot;
    s32 candidate_card_id;
    u8 *state_byte_cursor;
    s32 *script_memory;

    script_memory = gAiScript_aMemory;
    depth_limit_work = script_memory[AiScript_ReadByte()];
    excluded_set = script_memory[AiScript_ReadByte()];
    depth_limit_work = depth_limit_work + 1;
    result_register = AiScript_ReadByte();
    hand_search_count = Ai_GetHandSearchCount();

    gAiScript_State.fusion_count = hand_search_count;
    gAiScript_State.fusion_best_stat = 0;
    gAiScript_State.fusion_depth = 0;
    gAiScript_State.fusion_limit = depth_limit_work;
    gAiScript_State.fusion_set = excluded_set;

    for (search_index = 0; search_index < gAiScript_State.fusion_count; search_index++) {
        gAiScript_State.fusion_used[search_index] = 0;
    }

    for (search_index = 0; search_index < DUEL_FIELD_ROW_SIZE; search_index++) {
        candidate_card_id = gDuel_aActiveCards[search_index + AI_SLOT_OWN_MONSTER_FIRST].card_id;
        candidate_slot = search_index + 1;
        if (candidate_card_id == 0) {
            continue;
        }
        if (Ai_IsCardInSets(excluded_set, candidate_slot) != 0) {
            continue;
        }
        gAiScript_State.fusion_path[gAiScript_State.fusion_depth] = candidate_slot;
        if (gDuel_aActiveCards[search_index + AI_SLOT_OWN_MONSTER_FIRST].attack >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[search_index + AI_SLOT_OWN_MONSTER_FIRST].attack;
            gAiScript_State.combo_cards[0] = candidate_slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gDuel_aActiveCards[search_index + AI_SLOT_OWN_MONSTER_FIRST].defense >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[search_index + AI_SLOT_OWN_MONSTER_FIRST].defense;
            gAiScript_State.combo_cards[0] = candidate_slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gAiScript_State.fusion_limit >= 2) {
            /* state_byte_cursor deliberately points at the state base plus search_index, not at
             * &fusion_used[search_index]: keeping fusion_used's 0xAA in the access
             * displacement lets the address arithmetic fill the branch delay
             * slot. Spelling it &fusion_used[search_index] costs two instructions. */
            state_byte_cursor = (u8 *)&gAiScript_State + search_index;
            state_byte_cursor[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 1;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth + 1;
            Ai_CompleteFusion(candidate_card_id);
            state_byte_cursor[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 0;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth - 1;
        }
    }

    for (search_index = 0; search_index < gAiScript_State.fusion_count; search_index++) {
        candidate_card_id = gDuel_aActiveCards[search_index + AI_SLOT_OWN_HAND_FIRST].card_id;
        candidate_slot = search_index + AI_SLOT_OWN_HAND_FIRST;
        if (candidate_card_id == 0) {
            continue;
        }
        if (Ai_IsCardInSets(excluded_set, candidate_slot) != 0) {
            continue;
        }
        gAiScript_State.fusion_path[gAiScript_State.fusion_depth] = candidate_slot;
        if (gDuel_aActiveCards[search_index + AI_SLOT_OWN_HAND_FIRST].attack >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[search_index + AI_SLOT_OWN_HAND_FIRST].attack;
            gAiScript_State.combo_cards[0] = candidate_slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gDuel_aActiveCards[search_index + AI_SLOT_OWN_HAND_FIRST].defense >
            gAiScript_State.fusion_best_stat) {
            gAiScript_State.fusion_best_stat =
                gDuel_aActiveCards[search_index + AI_SLOT_OWN_HAND_FIRST].defense;
            gAiScript_State.combo_cards[0] = candidate_slot;
            gAiScript_State.combo_cards[1] = 0;
            gAiScript_State.fusion_best_depth =
                gAiScript_State.fusion_depth;
        }
        if (gAiScript_State.fusion_limit >= 3) {
            /* state_byte_cursor deliberately points at the state base plus search_index, not at
             * &fusion_used[search_index]: keeping fusion_used's 0xAA in the access
             * displacement lets the address arithmetic fill the branch delay
             * slot. Spelling it &fusion_used[search_index] costs two instructions. */
            state_byte_cursor = (u8 *)&gAiScript_State + search_index;
            state_byte_cursor[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 1;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth + 1;
            Ai_CompleteFusion(candidate_card_id);
            state_byte_cursor[AI_SCRIPT_FUSION_USED_BYTE_OFFSET] = 0;
            gAiScript_State.fusion_depth =
                gAiScript_State.fusion_depth - 1;
        }
    }

    if (gAiScript_State.combo_cards[0] != 0) {
        if (gAiScript_State.combo_cards[0] >= DUEL_FIELD_ROW_SIZE + 1) {
            gAiScript_aMemory[result_register] = 0;
        } else if (gAiScript_State.combo_cards[1] != 0) {
            gAiScript_aMemory[result_register] = 1;
        } else {
            gAiScript_aMemory[result_register] = 2;
        }
    } else {
        gAiScript_aMemory[result_register] = 3;
    }
}

void AiScript_EvaluateFusion(void)
{
    s32 starting_card_id;
    s32 depth_limit;
    s32 excluded_set;
    s32 result_register;
    s32 attack_stat;
    s32 defense_stat;
    s32 hand_index;
    s32 hand_search_count;

    starting_card_id = gAiScript_aMemory[AiScript_ReadByte()];
    depth_limit = gAiScript_aMemory[AiScript_ReadByte()] + 1;
    excluded_set = gAiScript_aMemory[AiScript_ReadByte()];
    result_register = AiScript_ReadByte();
    hand_search_count = Ai_GetHandSearchCount();

    gAiScript_State.fusion_count = hand_search_count;
    gAiScript_State.fusion_limit = depth_limit;
    gAiScript_State.fusion_best_stat = 0;
    gAiScript_State.fusion_depth = 0;
    gAiScript_State.fusion_best_depth = 0;
    gAiScript_State.fusion_set = excluded_set;

    for (hand_index = 0; hand_index < gAiScript_State.fusion_count; hand_index++) {
        gAiScript_State.fusion_used[hand_index] = 0;
    }

    attack_stat = Duel_GetBaseCardStat(starting_card_id, 0);
    defense_stat = Duel_GetBaseCardStat(starting_card_id, 1);

    if (defense_stat < attack_stat) {
        gAiScript_State.fusion_best_stat = Duel_GetBaseCardStat(starting_card_id, 0);
    } else {
        gAiScript_State.fusion_best_stat = Duel_GetBaseCardStat(starting_card_id, 1);
    }

    Ai_CompleteFusion(starting_card_id);

    if (gAiScript_State.fusion_best_depth != 0) {
        gAiScript_aMemory[result_register] = 0;
    } else {
        gAiScript_aMemory[result_register] = 1;
    }
}

/* Script opcodes 0x2C and 0x2D. The European image has no operand-skipping
   bodies; its script table points the same two slots at the empty handlers
   below instead. */
#ifndef VERSION_EUROPE
void AiScript_SkipHand(void)
{
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
}

void AiScript_SkipField(void)
{
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
    AiScript_ReadByte();
}
#endif

#ifdef VERSION_EUROPE
/* The European opcode 0x2C and 0x2D handlers: empty bodies right before
   AiScript_PlayFaceUp in the image, which consume no operand bytes. */
void AiScript_SkipHand(void)
{
}

void AiScript_SkipField(void)
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

    gAi_Selection.bytes[AI_SCRIPT_COMBO_CARD_COUNT - 1] = 0;
    gAi_Selection.bytes[0] = a;
    gAi_Selection.bytes[1] = b;
    gAi_Selection.bytes[2] = c;
    gAi_Selection.bytes[3] = d;
    gAi_Selection.bytes[4] = e;
}

void AiScript_SetPosition(void)
{
    s32 *memory = gAiScript_aMemory;
    s32 value;

    value = memory[AiScript_ReadByte()];
    gAi_Selection.bytes[6] = value;
}

/* The opponent row is indexed from gDuel_aActiveCards (cards[j]); gcc's
 * strength reduction turns the index into retail's single inner cursor, so it
 * needs no -fno-strength-reduce. A walking cursor over the same row instead
 * makes it build a second induction variable (four more instructions).
 * Retail never advances the outer slot: `i` and `attacker` stay at slot 1, so
 * the outer loop as compiled never exits. */
void AiScript_FindBestAttack(void) {
    AiActiveCard *cards;
    AiActiveCard *attacker;
    s32 want;
    s32 out;
    s32 i;
    s32 j;
    s32 margin;
    u16 flags;

    want = gAiScript_aMemory[AiScript_ReadByte()];
    out = AiScript_ReadByte();
    i = 1;
    cards = gDuel_aActiveCards;
    attacker = &cards[AI_SLOT_OWN_MONSTER_FIRST];
    gAiScript_State.attack_best_stat = 0;

    while (i < DUEL_FIELD_ROW_SIZE + 1) {
        if (attacker->card_id == 0) {
            continue;
        }
        if ((attacker->flags & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
            continue;
        }

        for (j = AI_SLOT_OPPONENT_MONSTER_FIRST;
             j < AI_SLOT_OPPONENT_MONSTER_FIRST + AI_ACTIVE_CARD_ROW_SLOT_COUNT;
             j++) {
            if (cards[j].card_id == 0) {
                continue;
            }
            flags = cards[j].flags;
            if ((flags & DUEL_CARD_FLAG_USED_THIS_TURN) != 0) {
                continue;
            }
            if ((flags & DUEL_CARD_FLAG_DEFENSE_POSITION) != 0) {
                continue;
            }
            if (want != 0) {
                if ((flags & DUEL_CARD_FLAG_FACE_DOWN) != 0) {
                    continue;
                }
            }
            margin = attacker->attack - cards[j].attack;
            margin += Duel_CalcGuardianStarMatchup(attacker->guardian_star, cards[j].guardian_star);
            if (gAiScript_State.attack_best_stat < margin) {
                gAiScript_State.attack_best_stat = margin;
                gAiScript_State.attack_best_slot = i;
                gAiScript_State.attack_best_target = j;
            }
        }
    }

    if (gAiScript_State.attack_best_stat != 0) {
        gAiScript_aMemory[out] = 0;
    } else {
        gAiScript_aMemory[out] = 1;
    }
}

/* The AI script VM's state opcodes, eleven former sources in address order:
   the combo-stack pushes, the hand and field no-ops, the card and type set
   edits, the two flag pairs and AiScript_MoveCard, the opponent-data load,
   the register store, copy and add, and the flag load that ends the run.
   Each reads its operands with AiScript_ReadByte or AiScript_ReadShort and
   writes gAiScript_aMemory, gAiScript_State or the duel card state.

   The former sources were recorded at five profiles; every member compiles
   to identical code at this unit's gcc_2_8_1_g8_split. AiScript_Print ends
   the unit, which runs from AiScript_ReadByte. */

void AiScript_PushComboCard(void)
{
    s32 count = 0;
    s32 i;
    u8 *output;

    i = AI_SCRIPT_COMBO_CARD_COUNT - 1;
    output = gAi_Selection.bytes + i;
    do {
        *output = 0;
        i--;
        output--;
    } while (i >= 0);

    for (i = 0; i <= gAiScript_State.fusion_best_depth; i++) {
        s32 value = gAiScript_State.combo_cards[i];

        if (value >= 11) {
            gAi_Selection.bytes[count] = value;
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
            gAi_Selection.bytes[count] = value;
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

void AiScript_SetHandFaceDownRequest(void)
{
    gAi_Selection.fields.face_down = 1;
}

void AiScript_ClearHandFaceDownRequest(void)
{
    gAi_Selection.fields.face_down = 0;
}

void AiScript_SetUnreadFlag(void)
{
    gAiScript_State.unread_flag = 1;
}

void AiScript_ClearUnreadFlag(void)
{
    gAiScript_State.unread_flag = 0;
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

void AiScript_LoadGuardianStarChoice(void)
{
    gAi_Selection.fields.zero = *(u8 *)&gAiScript_aMemory[AiScript_ReadByte()];
}

void AiScript_Print(void)
{
#ifdef VERSION_EUROPE
    /* The European build reads the checkpoint byte and prints nothing. */
    AiScript_ReadByte();
#else
    int checkpoint = AiScript_ReadByte();

    printf(gAiScript_szCheckpoint);
    printf(gAiScript_szCheckpointFormat, checkpoint);
#endif
}
