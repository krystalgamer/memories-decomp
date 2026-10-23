#ifndef GAME_MODEL_LOAD_WORDS_H
#define GAME_MODEL_LOAD_WORDS_H

#include "../types.h"

extern u8 *G32 gModel_pSlot1PayloadBase __attribute__((section(".data")));
extern s32 gModel_AuxiliaryPayloadAddress __attribute__((section(".data")));
extern s32 gModel_CreditsModuleAddress __attribute__((section(".data")));
extern void *G32 gModel_pCreditsMusicTrackBuffer __attribute__((section(".data")));

#endif
