#include "../../types.h"

/* SLES-03947 build of src/game/mem_card_dialog_runtime.c. */

#define VERSION_EUROPE

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xB7
#define MemCardDialog_Update func_8003F568
#define D_80090F9C gEuropean_D_80090F9C
#define D_8009B3BC gEuropean_D_8009B3BC
#define D_8009B3C6 gEuropean_D_8009B3C6
#define D_8009B3DE gEuropean_D_8009B3DE
#define D_8009B3F0 gEuropean_D_8009B3F0
#define func_8008D680 func_8008E814
#define func_8008D6F0 func_8008B650
#define MemCardStart func_8008B5E0
#define MemCardStop func_8008E884
#define MemCardSync func_8008CA6C

#include "../mem_card_dialog_runtime.c"
