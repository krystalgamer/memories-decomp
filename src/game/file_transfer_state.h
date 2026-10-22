#ifndef GAME_FILE_TRANSFER_STATE_H
#define GAME_FILE_TRANSFER_STATE_H

#include "../types.h"

extern volatile u32 gFile_dwTransferState;
extern volatile u32 gFile_dwTransferState_abs __attribute__((section(".data")));
extern u32 gFile_dwCancelRequestState;
extern u32 gFile_dwCancelRequestState_abs __attribute__((section(".data")));

#endif
