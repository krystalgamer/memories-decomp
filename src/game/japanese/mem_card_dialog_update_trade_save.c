#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_MEM_CARD_DIALOG_UPDATE_TRADE_SAVE
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xD4
#define D_8009B3EB gJapanese_MemCardDialogStepState
#define D_8009B3F9 gJapanese_MemCardPort
#define D_8009B3F4 D_8009B2E4
#define D_8009B3EC D_8009B2DC
#define D_800EFE18 gJapanese_MemCardFileNameBuffer
#define D_800EFBC0 D_800ED298
#define gMemCard_pPrimaryTransferCursor gJapanese_PrimaryTransferCursor
#define gMemCard_pSecondaryTransferCursor gJapanese_SecondaryTransferCursor
#define D_8009B3C4 gJapanese_MemCardTransferOffset
#define D_8009B3C2 gJapanese_MemCardTransferSize
#define MemCardAccept func_8008A880
#define MemCardGetDirentry func_8008B668
#define MemCardReadFile func_8008B228
#define MemCardWriteFile func_8008B448
#define MemCardDialog_UpdateTradeSave func_8003E5B4
#include "../mem_card_dialog_runtime.c"
