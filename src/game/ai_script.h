#ifndef MEMORIES_DECOMP_AI_SCRIPT_H
#define MEMORIES_DECOMP_AI_SCRIPT_H
#include "../types.h"
#include "duel_grid.h"
#include "ai_script_commands.h"

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
   ai_script_commands.h with the rest of the command table's handlers, and
   twenty-one sources reach them there. This header used to repeat those three
   prototypes word for word; it points at their home instead.
   src/game/ai_script_control_flow.c is the only file that includes this one,
   and it already includes ai_script_commands.h above it. */

void AiScript_EndHand(void);
void AiScript_EndField(void);

s32 AiScript_ReadByte(void);

s32 AiScript_ReadShort(void);

#endif
