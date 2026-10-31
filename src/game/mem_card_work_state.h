#ifndef GAME_MEM_CARD_WORK_STATE_H
#define GAME_MEM_CARD_WORK_STATE_H

#include "../types.h"

extern u16 gMemCardWork_wFlags;
extern u8 gMemCardWork_bSlotIndex;
extern u8 gMemCardWork_bOperationResult;
extern s32 gMemCardWork_nRequestResult;
extern s32 gMemCardWork_nCompletedRequest;
extern u8 gMemCardWork_bFrameChecksum __attribute__((section(".data")));

#endif
