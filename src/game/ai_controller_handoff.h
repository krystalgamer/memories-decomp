#ifndef GAME_AI_CONTROLLER_HANDOFF_H
#define GAME_AI_CONTROLLER_HANDOFF_H

#include "../types.h"
#include "duel_grid.h"

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
extern s8 gDuel_bSide0ControllerSelector[DUEL_SIDE_COUNT] __attribute__((section(".data")));

#ifdef D_800EAE8F_IS_ARRAY
extern u8 gAi_bGuardianStarChoice[];
#else
extern u8 gAi_bGuardianStarChoice;
#endif

extern u8 gAi_bHandFaceDownRequested;

#ifdef D_800EAE91_AS_ARRAY
extern u8 gAi_bFieldSourceCode[];
#else
extern u8 gAi_bFieldSourceCode;
#endif

#ifdef D_800EAE92_AS_ARRAY
extern u8 gAi_bFieldTargetCode[];
#else
extern u8 gAi_bFieldTargetCode;
#endif

void AiScript_SetHandFaceDownRequest(void);
void AiScript_ClearHandFaceDownRequest(void);
void AiScript_LoadHandFaceDownRequest(void);

#endif
