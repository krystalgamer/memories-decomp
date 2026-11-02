#include "../types.h"

/* Zero-initialized storage this unit owns at 0x800F5B98-0x800F5C88, defined in
 * address order ahead of the headers that declare it (#2602). */
s32 gAiScript_aMemory[20];
u8 D_800F5BE8[0xA0];

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
