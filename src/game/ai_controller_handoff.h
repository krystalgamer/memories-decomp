#ifndef GAME_AI_CONTROLLER_HANDOFF_H
#define GAME_AI_CONTROLLER_HANDOFF_H

#include "../types.h"
#include "duel_grid.h"

#ifdef D_8009B360_AS_SIDE_ARRAY
extern s8 gDuel_bSide0ControllerSelector[DUEL_SIDE_COUNT] __attribute__((section(".data")));
#elif defined(D_8009B360_IN_DATA)
extern s8 gDuel_bSide0ControllerSelector __attribute__((section(".data")));
#else
extern s8 gDuel_bSide0ControllerSelector;
#endif

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
