#include "../../types.h"
#define VERSION_JAPAN
#define SAVE_DATA_CAMPAIGN_SCENE_INDEX_ADDRESS 0x8009B16A

#define DIALOG_CHOICE_ADDRESS 0x8009B23D

/* SLPM-86398 build of src/game/mem_card_dialog.c. */
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xD4

/* Values used by the SaveData_* save-transfer functions. */
#define SAVE_DATA_LOAD_STATUS_ADDRESS 0x8009AFC1
#define SAVE_DATA_LOAD_PAIR_CANCEL_BUTTON PAD_BUTTON_CROSS
#define SAVE_DATA_LOAD_PAIR_CONFIRM_BUTTON PAD_BUTTON_CIRCLE

/* The Japanese image puts the options file name ("BISLPMP86398-YUGIOH", used
 * only by the late SaveData_RequestOptionsWrite) after "Fail File\n" and
 * before MemCardWork_UpdateOperation's first jump table, inside this unit's
 * .rodata. A literal in SaveData_RequestOptionsWrite would follow those
 * tables, so the five strings stay linker symbols ahead of this unit's
 * .rodata (config/slpm_86398/symbols.txt), declared in mem_card.h. */
#define MEM_CARD_SAVE_FILE_NAME gMemCard_szSaveFileName
#define MEM_CARD_FREE_BLOCK_FORMAT gMemCard_szFreeBlockCountFormat
#define MEM_CARD_MAKE_FILE_TRACE gMemCard_szMakeFileTrace
#define MEM_CARD_FAIL_FILE_TRACE gMemCard_szFailFileTrace

#include "../mem_card.c"
