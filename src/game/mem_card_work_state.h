#ifndef GAME_MEM_CARD_WORK_STATE_H
#define GAME_MEM_CARD_WORK_STATE_H

#include "../types.h"

extern u16 gMemCardWork_wFlags;
extern u8 gMemCardWork_bSlotIndex;
extern u8 gMemCardWork_bOperationResult;
#ifdef VERSION_EUROPE
extern s32 gMemCardWork_nRequestResult asm("D_8009B3F0");
#else
extern s32 gMemCardWork_nRequestResult;
#endif
extern s32 gMemCardWork_nCompletedRequest;
extern u8 gMemCardWork_bFrameChecksum __attribute__((section(".data")));

#endif
