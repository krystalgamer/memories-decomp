#ifndef GAME_DUEL_PRESENTATION_CONTROLS_H
#define GAME_DUEL_PRESENTATION_CONTROLS_H

#include "../types.h"

#ifdef D_8009B1B9_AS_SIGNED
extern s8 gDuel_bPresentationCursor;
#else
extern u8 gDuel_bPresentationCursor;
#endif

extern s8 gDuel_bPlacementNeedsCardMoveSetup;
extern u8 gDuel_bFieldActionAutoCommit;

#endif
