#include "../../types.h"
#define SAVE_DATA_CAMPAIGN_SCENE_INDEX_ADDRESS 0x8009C638

/* SLES-03947 build of src/game/checkerboard_background.c. The US source is included as is. */

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define CHECKERBOARD_CLUT_X 0x340
#define CHECKERBOARD_SCREEN_HEIGHT 0x100

#define DUEL_EFFECT_CHANNEL_CREATE_FLAGS 0x1010
#define DIALOG_CHOICE_ADDRESS 0x8009C6D0
#define DIALOG_BOX_X 0x08
#define DIALOG_BOX_WIDTH 0x130
#define DUEL_EFFECT_CHANNEL_HEIGHT 0x30

/* SLES-03948 to SLES-03951 build of src/game/dialog_transition.c. */

#define VERSION_EUROPE
#define DIALOG_TRANSITION_SPOTLIGHT_Y 128
#define DIALOG_TRANSITION_CHANNEL_FLAG 8

#define DIALOG_MESSAGE_TRANSITION_CHANNEL_FLAG 8

/* SLES-03948 to SLES-03951 build of src/game/mem_card_dialog.c. */
#define MEM_CARD_MESSAGE_COMPLETION_MASK 0x2010
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xB7

/* Values used by the SaveData_* save-transfer functions. */
#define SAVE_DATA_LOAD_BOX_FLAGS 0x1010
#define SAVE_DATA_LOAD_CHANNEL_FLAG 0x10
#define SAVE_DATA_LOAD_STATUS_ADDRESS 0x8009C435
/* The card name carries the image's product code. The French, German and
 * Italian wrappers include this file with their own. */
#ifndef MEM_CARD_SAVE_FILE_NAME
#define MEM_CARD_SAVE_FILE_NAME ((u8 *)"BESLES-03951-YUGIOH")
#endif

#include "../mem_card.c"
