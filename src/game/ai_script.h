#ifndef MEMORIES_DECOMP_AI_SCRIPT_H
#define MEMORIES_DECOMP_AI_SCRIPT_H
#include "../types.h"
#include "duel_grid.h"
#include "../ygo_types.h"
#include "ai.h"

#define AI_OPPONENT_DATA_FIELD_COUNT 9
#define AI_OPPONENT_COUNT 40
#define AI_OPPONENT_HAND_SEARCH_COUNT_FIELD 0

/* One nine-byte record per duellist. Ai_GetHandSearchCount reads values[0] as
   the configured hand/fusion search window, not a live hand count.
   AiScript_LoadOpponentData multiplies values[1] by
   100 for opcode field 0 and returns values[field + 1] for the rest, so the
   script reaches every entry by index and the record stays an array.
   src/game/ai_get_hand_search_count.c owns the table at 0x800917F0. */
typedef struct {
    s8 values[AI_OPPONENT_DATA_FIELD_COUNT];
} AiOpponentData;

typedef char AiOpponentData_size_must_be_9[
    sizeof(AiOpponentData) == AI_OPPONENT_DATA_FIELD_COUNT ? 1 : -1
];

extern AiOpponentData gDuel_aOpponentData[AI_OPPONENT_COUNT];

/* The signed opponent selector, element 1 of gDuel_bSide0ControllerSelector
 * (ai_controller_handoff.h). Every retail access is absolute, so every unit
 * takes this one .data declaration. */
extern s8 gDuel_bOpponentID;

/* The AI script opcode handlers, in the order gAiScript_apfnCommand holds
   them. Opcode 0 has no handler; every other opcode indexes this table.
   src/game/ai_script_vm.c owns the table at 0x800916E0. */
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
void AiScript_TestDefenseAtLeastAttack(void);
void AiScript_LoadCardType(void);
void AiScript_LoadCardID(void);
void AiScript_LoadCardState(void);
void AiScript_LoadLifePoint(void);
void AiScript_LoadOpponentID(void);
void AiScript_LoadTerrain(void);
void AiScript_LoadDeckSize(void);
void AiScript_LoadSelectionRandom(void);
void AiScript_TestSwordsActive(void);
void AiScript_LoadFirstFieldComboSlot(void);
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
void AiScript_SetUnreadFlag(void);
void AiScript_ClearUnreadFlag(void);
void AiScript_MoveCard(void);
void AiScript_LoadOpponentData(void);
void AiScript_Store(void);
void AiScript_SetRegister(void);
void AiScript_Add(void);
void AiScript_LoadGuardianStarChoice(void);
void AiScript_Print(void);

/* The signed byte just below gDuel_bOpponentID (0x8009B360). Main_StartDuel
 * stores its first argument here and its second into gDuel_bOpponentID;
 * Text_StartCampaignDuel resets it to -1 before it sets gDuel_bOpponentID;
 * Duel_InitSideStates and Duel_MarkCreatedCardKnown test it negative, the
 * latter together with gDuel_bActiveSide == 0 and a non-negative
 * gDuel_bOpponentID. The hand, field, card-move, ritual and result
 * controllers index it by the 0/1 side selector, so element 1 is the
 * opponent-id byte that follows; non-negative takes the AI path.
 *
 * Every retail access is lb or sb, and none is gp-relative: the -G8 units
 * use the assembler's macro form, which is what the .data placement gives
 * them, and the -G0 units are unaffected. Every unit takes this one
 * declaration. */
extern s8 gDuel_bSide0ControllerSelector[DUEL_SIDE_COUNT];

void AiScript_SetHandFaceDownRequest(void);
void AiScript_ClearHandFaceDownRequest(void);
void AiScript_LoadHandFaceDownRequest(void);

/* AiScript_Call, AiScript_Return and AiScript_SetRandom are declared in
   ai_script.h with the rest of the command table's handlers, and
   twenty-one sources reach them there. This header used to repeat those three
   prototypes word for word; it points at their home instead.
   src/game/ai_script_control_flow.c is the only file that includes this one,
   and it already includes ai_script.h above it. */

void AiScript_EndHand(void);
void AiScript_EndField(void);

s32 AiScript_ReadByte(void);

s32 AiScript_ReadShort(void);
#endif
