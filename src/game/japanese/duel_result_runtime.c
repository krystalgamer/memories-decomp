#include "../../types.h"

#define VERSION_JAPAN
#define DUEL_RESULT_OUTRO_START_SECTOR 0x1DB6
#define DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR 0x1D3E
#define D_8009B362 gJapanese_D_8009B362
#define D_8009B21C gJapanese_D_8009B21C
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define D_8009B360 gJapanese_DuelPlayerState

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_RESULT_REWARDS_CONFIRM_BUTTON PAD_BUTTON_CIRCLE

/* gFade_State under its second US name, as in fade_runtime.c: a #define
 * would give one identifier fade.h's two declarations, so an asm label
 * binds the array to the same symbol and the header completes its type. */
extern u8 D_800E9EC8_arr[] asm("gFade_State");
#include "../duel_result_runtime.c"
