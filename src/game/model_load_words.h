#ifndef GAME_MODEL_LOAD_WORDS_H
#define GAME_MODEL_LOAD_WORDS_H

#include "../types.h"

extern u8 *const gModel_pSlot1PayloadBase;
extern u8 *const gModel_pAuxiliaryPayload;
/* Model_UpdateCreditsPresentation passes these absolute word loads to
 * argument 7 of File_RequestAsyncTransfer (SU.MRG sectors [1223, 1239)) and
 * to SD_SetMusicTrackBuffer(SDMusicPackage *), respectively; the latter is a data
 * argument, not an overlay entry point. */
extern const s32 gModel_CreditsModuleAddress;
extern void *const gModel_pCreditsMusicTrackBuffer;

#endif
