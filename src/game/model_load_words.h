#ifndef GAME_MODEL_LOAD_WORDS_H
#define GAME_MODEL_LOAD_WORDS_H

#include "../types.h"

extern u8 *gModel_pSlot1PayloadBase;
extern s32 gModel_AuxiliaryPayloadAddress;
/* Model_UpdateCreditsPresentation passes these absolute word loads to
 * argument 7 of File_RequestAsyncTransfer (SU.MRG sectors [1223, 1239)) and
 * to SD_SetMusicTrackBuffer(u16 *), respectively; the latter is a data
 * argument, not an overlay entry point. */
extern s32 gModel_CreditsModuleAddress;
extern void *gModel_pCreditsMusicTrackBuffer;

#endif
