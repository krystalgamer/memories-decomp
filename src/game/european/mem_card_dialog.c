#include "../../types.h"

/* SLES-03947 build of src/game/mem_card_dialog.c. */
#define VERSION_EUROPE
/* MemCardWork_UpdateOperation keeps its out-parameter where the European symbols name
 * D_8009B3F0 (0x8009C324). The dialog functions name a different word
 * D_8009B3F0 (gEuropean_D_8009B3F0, 0x8009C320), so the out-parameter is
 * reached through its own name and assembler label. */
#define D_8009B3C8 European_D_8009B3C8
extern s32 European_D_8009B3C8 asm("D_8009B3F0");
#define MEM_CARD_MESSAGE_COMPLETION_MASK 0x2010
#define MEM_CARD_DIALOG_TRADE_SAVE_INITIAL_MESSAGE 0xB7
/* The splat name of its European address. */
#define func_8003E56C func_8003EF9C
#define D_8009AF70 gEuropean_D_8009AF70
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
