#include "../../types.h"

/* SLES-03948 to SLES-03951 build of src/game/mem_card_dialog_runtime.c. */

#define VERSION_EUROPE
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xB7
#define DIALOG_BOX_X 0x08
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
#define DIALOG_BOX_WIDTH 0x130

#include "../mem_card_dialog_runtime.c"
