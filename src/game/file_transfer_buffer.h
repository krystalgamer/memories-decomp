#ifndef GAME_FILE_TRANSFER_BUFFER_H
#define GAME_FILE_TRANSFER_BUFFER_H

#include "../types.h"

#ifdef D_8009B118_IS_POINTER_IN_DATA
extern u8 *G32 gFile_TransferBufferBase __attribute__((section(".data")));
#elif defined(D_8009B118_IN_DATA)
extern s32 gFile_TransferBufferBase __attribute__((section(".data")));
#else
extern s32 gFile_TransferBufferBase;
#endif

#endif
