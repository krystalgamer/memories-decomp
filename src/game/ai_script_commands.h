#ifndef MEMORIES_DECOMP_AI_SCRIPT_COMMANDS_H
#define MEMORIES_DECOMP_AI_SCRIPT_COMMANDS_H

#include "ai.h"

/* The AI script opcode handlers, in the order gAiScript_apfnCommand holds
   them. Opcode 0 has no handler; every other opcode indexes this table.
   src/game/ai_script_commands.c owns the table at 0x800916E0. */
void AiScript_Jump(void);
void AiScript_JumpGreaterEqual(void);
void AiScript_JumpGreater(void);
void AiScript_JumpEqual(void);
void AiScript_JumpNotEqual(void);
void AiScript_JumpBetween(void);
void AiScript_JumpRandom(void);
void AiScript_Call(void);
void AiScript_Return(void);
void AiScript_SetRandom(void);
void AiScript_Subtract(void);
void AiScript_PlayFieldCard(void);
void AiScript_CalcCardPower(void);
void AiScript_TestHighStat(void);
void AiScript_LoadCardType(void);
void AiScript_LoadCardID(void);
void AiScript_SetFaceDown(void);
void AiScript_LoadLifePoint(void);
void AiScript_LoadOpponentID(void);
void AiScript_LoadTerrain(void);
void AiScript_LoadDeckSize(void);
void AiScript_LoadSelectionRandom(void);
void AiScript_TestPinned(void);
void AiScript_StartCombo(void);
void AiScript_LoadBestDifference(void);
void AiScript_LoadBestAttacker(void);
void AiScript_LoadBestTarget(void);
void AiScript_FindStrongest(void);
void AiScript_FindWeakest(void);
void AiScript_FindKiller(void);
void AiScript_FindDefenseStopper(void);
void AiScript_CountCards(void);
void AiScript_FindFirstCard(void);
void AiScript_FindCard(void);
void AiScript_MatchType(void);
void AiScript_FindEquipTarget(void);
void AiScript_CheckRitual(void);
void AiScript_FindFirstMonster(void);
void AiScript_FindFirstType(void);
void AiScript_FindBestCombo(void);
void AiScript_EvaluateFusion(void);
void AiScript_SkipHand(void);
void AiScript_SkipField(void);
void AiScript_PlayFaceUp(void);
void AiScript_SetPosition(void);
void AiScript_FindBestAttack(void);
void AiScript_PushComboCard(void);
void AiScript_PushComboEmpty(void);
void AiScript_HandNop(void);
void AiScript_FieldNop(void);
void AiScript_AddCard(void);
void AiScript_ClearCards(void);
void AiScript_AddType(void);
void AiScript_ClearTypes(void);
void AiScript_SetSelectionRandom(void);
void AiScript_ClearSelectionRandom(void);
void func_80073464(void);
void func_80073474(void);
void AiScript_MoveCard(void);
void AiScript_LoadOpponentData(void);
void AiScript_Store(void);
void AiScript_SetRegister(void);
void AiScript_Add(void);
void func_8007368C(void);
void AiScript_Print(void);

#endif
