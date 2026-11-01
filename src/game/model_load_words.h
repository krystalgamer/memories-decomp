#ifndef GAME_MODEL_LOAD_WORDS_H
#define GAME_MODEL_LOAD_WORDS_H

#include "../types.h"

extern u8 *G32 gModel_pSlot1PayloadBase __attribute__((section(".data")));
extern s32 gModel_AuxiliaryPayloadAddress __attribute__((section(".data")));
/* Model_UpdateCreditsPresentation passes these absolute word loads to
 * argument 7 of File_RequestAsyncTransfer (SU.MRG sectors [1223, 1239)) and
 * to SD_SetMusicTrackBuffer(u16 *), respectively; the latter is a data
 * argument, not an overlay entry point. */
extern s32 gModel_CreditsModuleAddress __attribute__((section(".data")));
extern void *G32 gModel_pCreditsMusicTrackBuffer __attribute__((section(".data")));

#endif
