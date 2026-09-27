#include "../../types.h"

/* SLES-03947 build of src/game/mem_card_dialog_runtime.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_MEM_CARD_DIALOG_UPDATE_TRADE_SAVE
#define VERSION_EUROPE_MEM_CARD_DIALOG_STEP_SLIDE
#define VERSION_EUROPE_MEM_CARD_DIALOG_CREATE_OBJECT

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xB7

#include "../mem_card_dialog_runtime.c"
