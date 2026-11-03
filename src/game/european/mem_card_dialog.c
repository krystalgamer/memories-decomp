#include "../../types.h"
#define MEM_CARD_DIALOG_STORAGE_ORDER
/* This build lays out the shared source's zero-initialized storage at
 * 0x800F0638-0x800F0DA0 in a different order from the North American one;
 * the shared source defines it in first-declaration order (#2602). */
extern u8 gMemCardDialog_DirectoryBuffer[];
extern u8 gMemCardDialog_FileNameBuffer[];
extern struct MemCardWorkArea gMemCardWork_Area;
#define GDIALOG_CHOICE_IN_DATA
#define MEM_CARD_DIRECTORY_IN_DATA
#define MEM_CARD_FIND_LOADED_ENTRY_RESULT_VIEW
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../text_box_blocking_build.h"
#include "../mem_card_dialog.h"
#define MEM_CARD_DIALOG_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C310-0x8009C360 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 gMemCardDialog_RetryCounter;
u8 D_8009C311_sles_03947;
u16 D_8009C312_sles_03947;
s32 gMemCardDialog_SyncStatus;
u8 gSaveData_PairSetupState;
u8 gMemCardDialog_Outcome;
u8 gMemCardDialog_ChannelIndex;
u8 D_8009C31B_sles_03947;
u32 D_8009C31C_sles_03947;
s32 gMemCardDialog_SyncCommand;
s32 gMemCardWork_nRequestResult;
s32 gMemCardDialog_SyncResult;
u8 D_8009C32C_sles_03947;
u8 gMemCardDialog_Port;
u16 gMemCard_wDialogFlags;
u8 gSaveData_AllowIdentityBypass;
u8 D_8009B3C1;
u16 gMemCardDialog_TransferSize;
u16 gMemCardDialog_TransferOffset;
u8 gMemCardWork_OperationMode;
u8 gMemCardDialog_MessageCode;
u16 gMemCardWork_wFlags;
u8 gMemCardWork_OperationState;
u8 D_8009C33B_sles_03947;
u8 *gMemCard_pPrimaryTransferCursor;
u8 gMemCardWork_bOperationResult;
u8 D_8009C341_sles_03947;
u16 D_8009C342_sles_03947;
DisplayObject *gMemCard_pDialogObject;
u8 gMemCardDialog_BlockCount;
u8 gMemCardWork_bSlotIndex;
u16 D_8009C34A_sles_03947;
u8 *gMemCard_pSecondaryTransferCursor;
u8 gMemCardDialog_StepIndex;
u8 gMemCardDialog_OperationState;
u16 D_8009C352_sles_03947;
s32 gMemCardWork_nCompletedRequest;
u16 D_8009C358_sles_03947;
u8 gSaveData_LoadPairState;
u8 gSaveData_PairMessageCode;
u32 D_8009C35C_sles_03947;

/* SLES-03947 build of src/game/mem_card_dialog.c. */
#define VERSION_EUROPE
#define MEM_CARD_MESSAGE_COMPLETION_MASK 0x2010
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xB7
#define func_8008CF10 func_8008E0A4
#define func_8008D070 func_8008E204
#define func_8008D680 func_8008E814
#define func_8008D6F0 func_8008B650
#define func_8008DCB4 func_8008EE48
#define MemCardCreateFile func_8008CB88
#define MemCardFormat func_8008CC84
#define MemCardStart func_8008B5E0
#define MemCardStop func_8008E884
#define MemCardSync func_8008CA6C

/* Values used by the SaveData_* save-transfer functions. */
#define SAVE_DATA_LOAD_BOX_FLAGS 0x1010
#define SAVE_DATA_LOAD_CHANNEL_FLAG 0x10
#define SAVE_DATA_LOAD_STATUS_ADDRESS 0x8009C015
#include "../mem_card_dialog.c"
