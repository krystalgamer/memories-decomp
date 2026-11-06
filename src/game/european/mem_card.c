#include "../../types.h"
#define SAVE_DATA_CAMPAIGN_SCENE_INDEX_ADDRESS 0x8009C218

#define SAVE_DATA_SEQUENCE_IN_MEM_CARD_DIALOG

/* SLES-03947 build of src/game/checkerboard_background.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define CHECKERBOARD_CLUT_X 0x340
#define CHECKERBOARD_SCREEN_HEIGHT 0x100

/* SLES-03947 build of src/game/duel_effect_create_channel.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_EFFECT_CHANNEL_CREATE_FLAGS 0x1010
#define DIALOG_CHOICE_ADDRESS 0x8009C2B0

/* SLES-03947 build of src/game/dialog_transition.c. */

#define VERSION_EUROPE

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define DIALOG_TRANSITION_SPOTLIGHT_Y 128
#define DIALOG_TRANSITION_CHANNEL_FLAG 8

#define FUNC_8003DA40_CHANNEL_FLAG 8

#define MEM_CARD_DIALOG_STORAGE_ORDER
/* This build lays out the shared source's zero-initialized storage at
 * 0x800F0638-0x800F0DA0 in a different order from the North American one;
 * the shared source defines it in first-declaration order (#2602). */
extern u8 gMemCardDialog_DirectoryBuffer[];
extern u8 gMemCardDialog_FileNameBuffer[];
extern struct MemCardWorkArea gMemCardWork_Area;
#define MEM_CARD_FIND_LOADED_ENTRY_RESULT_VIEW
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
s32 gSaveDataSequence;
s32 gMemCardDialog_SyncCommand;
s32 gMemCardWork_nRequestResult;
s32 gMemCardDialog_SyncResult;
u8 D_8009B3BC;
u8 gMemCardDialog_Port;
u16 gMemCard_wDialogFlags;
u8 gSaveData_AllowIdentityBypass;
u8 gDialog_bCallbackState;
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
u16 D_8009B3DE;
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
#define MEM_CARD_MESSAGE_COMPLETION_MASK 0x2010
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xB7

/* Values used by the SaveData_* save-transfer functions. */
#define SAVE_DATA_LOAD_BOX_FLAGS 0x1010
#define SAVE_DATA_LOAD_CHANNEL_FLAG 0x10
#define SAVE_DATA_LOAD_STATUS_ADDRESS 0x8009C015

#include "../mem_card.c"
