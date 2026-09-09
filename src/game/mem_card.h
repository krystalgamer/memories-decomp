#ifndef MEMORIES_DECOMP_MEM_CARD_H
#define MEMORIES_DECOMP_MEM_CARD_H

#include "../types.h"

#define MEM_CARD_DIRECTORY_ENTRY_SIZE 40
#define MEM_CARD_BLOCK_SIZE 8192
#define MEM_CARD_BLOCK_COUNT 15
#define MEM_CARD_DIRECTORY_RETRIES 5

#define MEM_CARD_DIALOG_FLAG_RESULT_CREATED 0x40
#define MEM_CARD_DIALOG_FLAG_RESULT_READY 0x80
#define MEM_CARD_DIALOG_FLAG_CLOSING 0x800
#define MEM_CARD_DIALOG_FLAG_IO_PENDING 0x1000
#define MEM_CARD_DIALOG_FLAG_STARTED 0x2000
#define MEM_CARD_DIALOG_FLAG_OPENED 0x4000
#define MEM_CARD_DIALOG_FLAG_ACTIVE 0x8000

/* The result of the card's asynchronous IO, set from the callbacks
 * mem_card_io_result_callbacks.h declares and polled by the request state
 * machines. func_800440B4 resets it to -1 before starting a request.
 *
 * Three of the five consumers declare it volatile, and they are the ones
 * that read it several times in a row while waiting -- without the
 * qualifier gcc commons those reads into one register and the poll cannot
 * observe the callback. The other two write it once or read it once. */
#ifdef GMEMCARD_NIORESULT_IS_VOLATILE
extern volatile s32 gMemCard_nIOResult;
#else
extern s32 gMemCard_nIOResult;
#endif

extern u8 gMemCard_szSaveFileName[];

/* The create-state message value must stay wide so GCC keeps the retail
 * register-to-argument move; the definition and the other callers use the
 * measured byte/halfword contract. */
#ifdef GMEMCARD_RESULT_USES_WIDE_ARGS
void func_8003E46C(s32 value, s32 bits);
#else
void func_8003E46C(u8 value, u16 bits);
#endif

/* The memory-card dialog's flag word.
 *
 * Declared plain here because the readers need it plain: every other user
 * only tests bits or does a read-modify-write, and marking the object
 * volatile forces reloads that grow .text by 40 bytes.
 *
 * func_8003E46C needs the opposite and reaches the same word through its own
 * volatile linker name; see the comment there.
 */
extern u16 gMemCard_wDialogFlags;

#endif
