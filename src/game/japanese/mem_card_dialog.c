#include "../../types.h"

/* SLPM-86398 build of src/game/mem_card_dialog.c. */
#define VERSION_JAPAN
#define DIALOG_TRANSITION_EFFECT_CHANNEL_STRIDE 0x60
#define MEM_CARD_DIALOG_EFFECT_CHANNEL_STRIDE 0x60
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xD4

/* Values used by the SaveData_* save-transfer functions. */
#define SAVE_DATA_LOAD_STATUS_ADDRESS 0x8009AFC1
#define SAVE_DATA_LOAD_PAIR_CANCEL_BUTTON PAD_BUTTON_CROSS
#define SAVE_DATA_LOAD_PAIR_CONFIRM_BUTTON PAD_BUTTON_CIRCLE
#include "../mem_card_dialog.c"
