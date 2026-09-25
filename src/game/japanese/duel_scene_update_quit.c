#include "../../types.h"

#define DUEL_QUIT_BOX_X 0x70
#define DUEL_QUIT_BOX_WIDTH 0x60
#define DUEL_QUIT_BOX_HEIGHT 0x20
/* Channel 3 of the 0x60-byte Japanese records is 0x800EB0F8, which splat
   labels D_800EB0F8 in the Japanese image; that name is the US channel
   array's in C, so the record gets its own name bound by an asm label. */
extern u8 gJapanese_DuelQuitChannel[] asm("D_800EB0F8");
#define D_800EB224 gJapanese_DuelQuitChannel
#define func_800235C0 func_80023408
#define gDuel_apfnSceneStateHandler D_80090848
#include "../func_80024200.c"
