#include "../../types.h"

/* SLES-03948 to SLES-03951 build of src/game/mem_card_dialog.c. */
#define VERSION_EUROPE
#define MEM_CARD_MESSAGE_COMPLETION_MASK 0x2010
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xB7
#define DIALOG_BOX_X 0x08
#define DIALOG_BOX_WIDTH 0x130

/* Values used by the SaveData_* save-transfer functions. */
#define SAVE_DATA_LOAD_BOX_FLAGS 0x1010
#define SAVE_DATA_LOAD_CHANNEL_FLAG 0x10
#define SAVE_DATA_LOAD_STATUS_ADDRESS 0x8009C435
#include "../mem_card_dialog.c"
